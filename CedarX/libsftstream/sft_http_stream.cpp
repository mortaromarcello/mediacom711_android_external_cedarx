/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/
//#define LOG_NDEBUG 0
#define LOG_TAG "sft_http_stream"
#include <CDX_Debug.h>

#include <CDX_Common.h>
#include <cedarx_stream.h>
#include <cedarx_demux.h>

#if (CEDARX_ANDROID_VERSION >= 6)

#include <HTTPBase.h>
#include <NuCachedSource2.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/foundation/AMessage.h>

namespace android {

class SftHttpStream {
public:
	SftHttpStream();
	virtual ~SftHttpStream();

	virtual int Open(CedarXDataSourceDesc *datasrc_desc);
	virtual int Close();
	virtual int Sniff();
	virtual int Read(void *ptr, size_t size);
	virtual int GetCacheState(CDX_CACHE_STATE *cdx_cache_state);
	virtual int Seek(long long offset, int whence);
	virtual void SetDuration(int64_t duration);
	virtual int64_t Tell();
	virtual int64_t GetSize();
	virtual void Reset();
	virtual bool SniffDRM( const sp<DataSource> &source,
			String8 *mimeType, float *confidence,
	        sp<AMessage> *);

private:
	sp<DataSource> dataSource;
	sp<HTTPBase> mConnectingDataSource;
	sp<NuCachedSource2> mCachedSource;
	bool mUIDValid;
	uid_t mUID;
	int64_t mFilePos;
	int64_t mFileLen;
	int64_t mBitrate;
	int64_t mDurationUs;
	String8 mUri;
    KeyedVector<String8, String8> mUriHeaders;
	mutable Mutex mLock;
	int     mHasRegisterSniffers;
};

SftHttpStream::SftHttpStream():
		mFilePos(0),mFileLen(0),mBitrate(25000),mDurationUs(0),mHasRegisterSniffers(0)
{
	mUriHeaders.clear();
}

SftHttpStream::~SftHttpStream()
{

}

bool SftHttpStream::SniffDRM(
    const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *) {
    sp<DecryptHandle> decryptHandle = source->DrmInitialization();

    if (decryptHandle != NULL) {
        if (decryptHandle->decryptApiType == DecryptApiType::CONTAINER_BASED) {
            *mimeType = String8("drm+container_based+") + decryptHandle->mimeType;
        } else if (decryptHandle->decryptApiType == DecryptApiType::ELEMENTARY_STREAM_BASED) {
            *mimeType = String8("drm+es_based+") + decryptHandle->mimeType;
        } else if (decryptHandle->decryptApiType == DecryptApiType::WV_BASED) {
            *mimeType = MEDIA_MIMETYPE_CONTAINER_WVM;
            LOGI("SniffWVM: found match\n");
        }
        *confidence = 10.0f;

        return true;
    }

    return false;
}

int SftHttpStream::Open(CedarXDataSourceDesc *datasrc_desc)
{
	int flags = 0;

    bool isWidevineStreaming = false;
    AString sniffedMIME;
    mUri = datasrc_desc->source_url;

    if (!strncasecmp("widevine://", mUri.string(), 11)) {
        isWidevineStreaming = true;
        String8 newURI = String8("http://");
        newURI.append(mUri.string() + 11);

        mUri = newURI;
    }

	if (!strncasecmp("http://", mUri.string(), 7)
			|| !strncasecmp("https://", mUri.string(), 8)
			|| isWidevineStreaming) {

		if(strstr(mUri.string(),"m3u8") || strstr(mUri.string(),"espn")) {
			flags |= HTTPBase::kFlagUAIPAD;
		}

		if(datasrc_desc->url_headers) {
			mUriHeaders = *(KeyedVector<String8, String8> *)datasrc_desc->url_headers;
	        ssize_t index = mUriHeaders.indexOfKey(String8("x-hide-urls-from-log"));
	        if (index >= 0) {
	            // Browser is in "incognito" mode, suppress logging URLs.

	            // This isn't something that should be passed to the server.
	            mUriHeaders.removeItemsAt(index);
	            flags |= HTTPBase::kFlagIncognito;
	        }
		}

		mConnectingDataSource = HTTPBase::Create(flags /*(mFlags & INCOGNITO)
					? HTTPBase::kFlagIncognito
					: 0*/);

		if (mUIDValid) {
			mConnectingDataSource->setUID(mUID);
		}

		String8 cacheConfig;
		bool disconnectAtHighwatermark;
		NuCachedSource2::RemoveCacheSpecificHeaders(
				&mUriHeaders, &cacheConfig, &disconnectAtHighwatermark);

		mLock.unlock();
		status_t err = mConnectingDataSource->connect(mUri, &mUriHeaders);
		mLock.lock();

		if (err != OK) {
			mConnectingDataSource.clear();

			LOGI("mConnectingDataSource->connect() returned %d", err);
			return err;
		}

        if (!isWidevineStreaming) {
            // The widevine extractor does its own caching.

            mCachedSource = new NuCachedSource2(
                    mConnectingDataSource,
                    cacheConfig.isEmpty() ? NULL : cacheConfig.string(),
                    disconnectAtHighwatermark);

            dataSource = mCachedSource;
        } else {
            dataSource = mConnectingDataSource;
        }

        String8 mimeType;
        float confidence;
        sp<AMessage> dummy;
        bool success = SniffDRM(dataSource, &mimeType, &confidence, &dummy);

        if (success
                && !strcasecmp(
                    mimeType.string(), MEDIA_MIMETYPE_CONTAINER_WVM)) {
        	LOGI("wvm video");
        	datasrc_desc->media_type = CEDARX_MEDIATYPE_WVM_VIDEO;
        	datasrc_desc->sft_http_source = mConnectingDataSource.get();
        }

        //get file length
        mCachedSource->getSize(&mFileLen);
        LOGV("sft http stream length %lld", mFileLen);
        
        datasrc_desc->sft_stream_length = mFileLen;
	    datasrc_desc->sft_cached_source2 = mCachedSource.get();

		mConnectingDataSource.clear();
	}
	else {
		return -1;
	}

	return 0;
}

int SftHttpStream::Close()
{
	if(mConnectingDataSource != NULL){
		mConnectingDataSource->disconnect();
	}

	if(mCachedSource != NULL){
		mCachedSource.clear();
	}

	return 0;
}

int SftHttpStream::Sniff()
{
	String8 tmp;
	float confidence;
	AString sniffedMIME;
	sp<AMessage> meta;

	if(!mHasRegisterSniffers) {
		DataSource::RegisterDefaultSniffers();
		mHasRegisterSniffers = 1;
	}

	if (!mCachedSource->sniff(&tmp, &confidence, &meta)) {
		return -1;
	}

	sniffedMIME = tmp.string();
	LOGD("Sniff MIME:%s",sniffedMIME.c_str());

	return 0;
}

int SftHttpStream::Read(void * ptr, size_t size)
{
	ssize_t readBytes;

	if(mFileLen > 0 && mFilePos > mFileLen) {
		LOGW("read beyond file");
		return -1;
	}

	readBytes = mCachedSource->readAt(mFilePos, ptr, size);
//	LOGV("require size %x, file Pos %llx, readBytes %x", size, mFilePos, readBytes);
	mFilePos += readBytes;
	return readBytes;
}

int SftHttpStream::Seek(long long offset, int whence)
{
	int ret = 0;
	if(whence == SEEK_CUR)
	{
		mFilePos += offset;
	}
	else if(whence == SEEK_SET)
	{
		mFilePos = offset;
	}
	else if(whence == SEEK_END)
	{
		mFilePos = mFileLen + offset;
	}
	else
	{
		ret = -1;
	}

	if(mFilePos < 0)
		mFilePos = 0;

	//seek beyond end of file
	if((mFileLen > 0) && (mFilePos > mFileLen)) {
		mFilePos = mFileLen + 1;
		ret = -1;
	}

	return ret;
}

int64_t SftHttpStream::Tell()
{
	return mFilePos;
}

int64_t SftHttpStream::GetSize()
{
	return mFileLen;
}

void SftHttpStream::Reset()
{
	mFilePos = 0;
}

void SftHttpStream::SetDuration(int64_t duration)
{
	LOGV("set duration %lld", duration);
	mDurationUs = duration;
	mBitrate 	= mFileLen * 8000000ll / mDurationUs;
}

int SftHttpStream::GetCacheState(CDX_CACHE_STATE *cdx_cache_state)
{
	if (mCachedSource != NULL) {
        status_t finalStatus;
        size_t cachedDataRemaining = mCachedSource->approxDataRemaining(&finalStatus);
        bool eos = (finalStatus != OK);
        int32_t kbps = 0;

        cdx_cache_state->filled_size 		= cachedDataRemaining;
        cdx_cache_state->filled_percent 	= cachedDataRemaining * 100 / (2*1024*1024);
        cdx_cache_state->buffering_percent 	= 0;
        cdx_cache_state->eof_flag 			= eos;
        status_t err = mCachedSource->getEstimatedBandwidthKbps(&kbps);
		if (err == OK) {
			cdx_cache_state->bandwidth_kbps = kbps;
		}
		if (mBitrate && mDurationUs) {
			size_t cachedSize = mCachedSource->cachedSize();
			int64_t cachedDurationUs = cachedSize * 8000000ll / mBitrate;

			int percentage = 100.0 * (double)cachedDurationUs / mDurationUs;
			if (percentage > 100) {
				percentage = 100;
			}

			cdx_cache_state->buffering_percent = percentage;
			//LOGV("cache percent: %d%% bitrate:%lld cachedSize:%d",percentage,bitrate,cachedSize);
			//notifyListener_l(MEDIA_BUFFERING_UPDATE, percentage);
		} else {
			// We don't know the bitrate of the stream, use absolute size
			// limits to maintain the cache.
		}
		cdx_cache_state->buffering_time = cachedDataRemaining * 8000000ll / 600000;
//		LOGV("filled size:%d filled percent:%d buffer time:%d bandwidth:%dkbps",cdx_cache_state->filled_size, cdx_cache_state->filled_percent, cdx_cache_state->buffering_time, cdx_cache_state->bandwidth_kbps);
    }
	else
	{
		cdx_cache_state->filled_percent = 0;
	}

	return OK;
}

extern "C" {

static int stream_seek(struct cdx_stream_info *stream, cdx_off_t offset, int whence)
{
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	return sft_stream->Seek(offset, whence);
}

static cdx_off_t stream_tell(struct cdx_stream_info *stream)
{
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	return sft_stream->Tell();
}

static int stream_read(void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	unsigned int req_bytes;
	int read_bytes;
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	req_bytes = size * nmemb;
	if(req_bytes <= 0)
		return -1;

	read_bytes = sft_stream->Read(ptr, req_bytes);
	if(read_bytes < 0)
		read_bytes = 0;
	return read_bytes / size;
}

static int  stream_write(const void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	int ret = 0;
	return ret;
}

static long long stream_get_size(struct cdx_stream_info *stream)
{
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	return sft_stream->GetSize();
}

//below two function used for m3u/ts
static long long stream_seek_to_time(struct cdx_stream_info *stream, long long us)
{
	int64_t ret = 0;
	return ret;
}

static long long stream_get_total_duration(struct cdx_stream_info *stream)
{
	int64_t ret = 0;
	return ret;
}

static void stream_reset(struct cdx_stream_info *stream)
{
	int ret = 0;
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return ;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	LOGD("reset stream");
	sft_stream->Reset();

}

static int stream_control(struct cdx_stream_info *stream, void *arg, int cmd)
{
	int ret = 0;
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return -1;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	switch(cmd)
	{
	case CDX_DMX_CMD_GET_CACHE_STATE:
		return sft_stream->GetCacheState((CDX_CACHE_STATE *)arg);
		break;
	case CDX_DMX_CMD_SET_DURATION:
	{
		int64_t duration;
		duration = *((int64_t *)arg);
		sft_stream->SetDuration(duration);
		break;
	}
	case CDX_DMX_CMD_SNIFF_SFT_MIME:
	{
		ret = sft_stream->Sniff();
		break;
	}
	default:
		break;
	}
	return ret;
}

int sft_http_create_stream_handle(CedarXDataSourceDesc *datasource_desc, struct cdx_stream_info *stream)
{
	int ret;

	SftHttpStream *sft_stream = (SftHttpStream *)datasource_desc->sft_stream_handle;
	LOGV("sft_http_stream url: %s", datasource_desc->source_url);

	if(sft_stream == NULL)
	{
		sft_stream = new(SftHttpStream);
		ret = sft_stream->Open(datasource_desc);
		if(ret < 0)
		{
			LOGW("open sft_http_stream failed");
			delete sft_stream;
			return ret;
		}
		datasource_desc->sft_stream_handle 			= sft_stream;
	}

	stream->seek               = stream_seek;
	stream->tell               = stream_tell;
	stream->read               = stream_read;
	stream->seek_to_time       = stream_seek_to_time;
	stream->get_total_duration = stream_get_total_duration;
	stream->reset_stream       = stream_reset;
	stream->control_stream     = stream_control;
	stream->write 			 = stream_write;
	stream->getsize 			 = stream_get_size;

	stream->data_src_desc.sft_stream_handle 	= sft_stream;
	stream_reset(stream);

	return 0;
}

void sft_http_destory_stream_handle(struct cdx_stream_info *stream)
{
	LOGV("destroy sft_http_stream handle");
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return ;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	if(sft_stream)
	{
		sft_stream->Close();
		delete sft_stream;
		sft_stream = NULL;
	}

	if(stream != NULL)
		stream->data_src_desc.sft_stream_handle = NULL;
}

} //end extern "C"

} //end android
#else

#include <NuCachedSource2.h>
#include <NuHTTPDataSource.h>
namespace android {

class SftHttpStream {
public:
	SftHttpStream();
	virtual ~SftHttpStream();

	virtual int Open(CedarXDataSourceDesc *datasrc_desc);
	virtual int Close();
	virtual int Read(void *ptr, size_t size);
	virtual int GetCacheState(CDX_CACHE_STATE *cdx_cache_state);
	virtual int Seek(long long offset, int whence);
	virtual void SetDuration(int64_t duration);
	virtual int64_t Tell();
	virtual int64_t GetSize();
	virtual void Reset();
private:
	sp<NuHTTPDataSource> mConnectingDataSource;
	sp<NuCachedSource2> mCachedSource;
	bool mUIDValid;
	uid_t mUID;
	off_t mFilePos;
	off_t mFileLen;
	int64_t mBitrate;
	int64_t mDurationUs;
    KeyedVector<String8, String8> mUriHeaders;
	mutable Mutex mLock;
};

SftHttpStream::SftHttpStream():
		mFilePos(0),mFileLen(0),mBitrate(25000),mDurationUs(0)
{
//	DataSource::RegisterDefaultSniffers();
	mUriHeaders.clear();
}

SftHttpStream::~SftHttpStream()
{

}

int SftHttpStream::Open(CedarXDataSourceDesc *datasrc_desc)
{
    char *url = datasrc_desc->source_url;

    if (!strncasecmp("http://", url, 7)){
    	mConnectingDataSource = new NuHTTPDataSource;
    	if(mConnectingDataSource == NULL)
    		return -1;
    	mLock.unlock();
    	status_t err = mConnectingDataSource->connect(url, &mUriHeaders);
    	mLock.lock();
    	if (err != OK) {
    		mConnectingDataSource.clear();

    		LOGI("mConnectingDataSource->connect() returned %d", err);
    		return err;
    	}
    	mCachedSource = new NuCachedSource2(mConnectingDataSource);
    	mConnectingDataSource.clear();
    	mCachedSource->getSize(&mFileLen);
    	LOGV("file length %x", mFileLen);
    	datasrc_desc->sft_cached_source2 = mCachedSource.get();
    }
    else
    {
    	return -1;
    }

	return 0;
}

int SftHttpStream::Close()
{
	if(mConnectingDataSource != NULL){
		mConnectingDataSource->disconnect();
	}

	if(mCachedSource != NULL){
		mCachedSource.clear();
	}

	return 0;
}

int SftHttpStream::Read(void * ptr, size_t size)
{
	ssize_t readBytes;
	readBytes = mCachedSource->readAt(mFilePos, ptr, size);
//	LOGV("require size %x, file Pos %x, readBytes %x", size, mFilePos, readBytes);
	mFilePos +=readBytes;
	return readBytes;
}

int SftHttpStream::Seek(long long offset, int whence)
{
	int ret = 0;
	off_t tmp_offset;

	tmp_offset = offset & 0xffffffff;
	if(whence == SEEK_CUR)
	{
		mFilePos += tmp_offset;
	}
	else if(whence == SEEK_SET)
	{
		mFilePos = tmp_offset;
	}
	else if(whence == SEEK_END)
	{
		mFilePos = mFileLen + tmp_offset;
	}
	else
	{
		ret = -1;
	}

	if(mFilePos < 0)
		mFilePos = 0;

	//seek beyond end of file
	if((mFileLen > 0) && (mFilePos > mFileLen))
		mFilePos = mFileLen;

	return ret;
}

int64_t SftHttpStream::Tell()
{
	return (int64_t)mFilePos;
}

int64_t SftHttpStream::GetSize()
{
	return (int64_t)mFileLen;
}

void SftHttpStream::Reset()
{
	mFilePos = 0;
}

void SftHttpStream::SetDuration(int64_t duration)
{
	LOGV("set duration %lld", duration);
	mDurationUs = duration;
	mBitrate 	= mFileLen * 8000000ll / mDurationUs;
}

int SftHttpStream::GetCacheState(CDX_CACHE_STATE *cdx_cache_state)
{
	if (mCachedSource != NULL) {
		bool eos;
		size_t cachedDataRemaining = mCachedSource->approxDataRemaining(&eos);

        cdx_cache_state->filled_size 		= cachedDataRemaining;
        cdx_cache_state->filled_percent 	= cachedDataRemaining * 100 / (2*1024*1024);
        cdx_cache_state->buffering_percent 	= 0;
        cdx_cache_state->eof_flag 			= eos;

		if (mBitrate && mDurationUs) {
			size_t cachedSize = mCachedSource->cachedSize();
			int64_t cachedDurationUs = cachedSize * 8000000ll / mBitrate;

			int percentage = 100.0 * (double)cachedDurationUs / mDurationUs;
			if (percentage > 100) {
				percentage = 100;
			}

			cdx_cache_state->buffering_percent = percentage;
			//LOGV("cache percent: %d%% bitrate:%lld cachedSize:%d",percentage,bitrate,cachedSize);
			//notifyListener_l(MEDIA_BUFFERING_UPDATE, percentage);
		} else {
			// We don't know the bitrate of the stream, use absolute size
			// limits to maintain the cache.
		}
		cdx_cache_state->buffering_time = cachedDataRemaining * 8000000ll / 600000;
//		LOGV("filled size:%d filled percent:%d buffer time:%d bandwidth:%dkbps",cdx_cache_state->filled_size, cdx_cache_state->filled_percent, cdx_cache_state->buffering_time, cdx_cache_state->bandwidth_kbps);
    }
	else
	{
		cdx_cache_state->filled_percent = 0;
		cdx_cache_state->filled_size 	= 0;
	}
	return OK;
}

extern "C" {

static int stream_seek(struct cdx_stream_info *stream, cdx_off_t offset, int whence)
{
	SftHttpStream *sft_stream;

	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	return sft_stream->Seek(offset, whence);
}

static cdx_off_t stream_tell(struct cdx_stream_info *stream)
{
	SftHttpStream *sft_stream;

	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	return sft_stream->Tell();
}

static int stream_read(void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	unsigned int req_bytes;
	int 		 read_bytes;
	SftHttpStream *sft_stream;

	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	req_bytes = size * nmemb;
	if(req_bytes <= 0)
		return -1;

	read_bytes = sft_stream->Read(ptr, req_bytes);
	if(read_bytes < 0)
		read_bytes = 0;
	return read_bytes/size;
}

static int  stream_write(const void *ptr, size_t size, size_t nmemb, struct cdx_stream_info *stream)
{
	int ret = 0;
	return ret;
}

static long long stream_get_size(struct cdx_stream_info *stream)
{
	SftHttpStream *sft_stream;

	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	return sft_stream->GetSize();
}

//below two function used for m3u/ts
static long long stream_seek_to_time(struct cdx_stream_info *stream, long long us)
{
	int64_t ret = 0;
	return ret;
}

static long long stream_get_total_duration(struct cdx_stream_info *stream)
{
	int64_t ret = 0;
	return ret;
}

static void stream_reset(struct cdx_stream_info *stream)
{
	int ret = 0;
	SftHttpStream *sft_stream;

	if(stream == NULL)
		return;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;
	sft_stream->Reset();
}

static int stream_control(struct cdx_stream_info *stream, void *arg, int cmd)
{
	int ret = 0;
	SftHttpStream *sft_stream;

	if(stream == NULL)
		return -1;
	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	switch(cmd)
	{
	case CDX_DMX_CMD_GET_CACHE_STATE:
		return sft_stream->GetCacheState((CDX_CACHE_STATE *)arg);
		break;
	case CDX_DMX_CMD_SET_DURATION:
	{
		int64_t duration;
		duration = *((int64_t *)arg);
		sft_stream->SetDuration(duration);
		break;
	}
	default:
		break;
	}
	return ret;
}

int sft_http_create_stream_handle(CedarXDataSourceDesc *datasource_desc, struct cdx_stream_info *stream)
{
	int ret;
	SftHttpStream *sft_stream = (SftHttpStream *)datasource_desc->sft_stream_handle;
	LOGV("sft_http_stream url: %s", datasource_desc->source_url);

	if(sft_stream == NULL) {
		sft_stream = new(SftHttpStream);
		ret = sft_stream->Open(datasource_desc);
		if(ret < 0)
		{
			LOGW("open sft_http_stream failed");
			return ret;
		}
		datasource_desc->sft_stream_handle 	= sft_stream;
	}

	stream->seek               = stream_seek;
	stream->tell               = stream_tell;
	stream->read               = stream_read;
	stream->seek_to_time       = stream_seek_to_time;
	stream->get_total_duration = stream_get_total_duration;
	stream->reset_stream       = stream_reset;
	stream->control_stream     = stream_control;
	stream->write 			 = stream_write;
    stream->getsize 			 = stream_get_size;

//    datasource_desc->sft_stream_handle 			= sft_stream;
    stream->data_src_desc.sft_stream_handle 	= sft_stream;

    stream_reset(stream);

	return 0;
}

void sft_http_destory_stream_handle(struct cdx_stream_info *stream)
{
	LOGV("destroy sft_http_stream handle");
	SftHttpStream *sft_stream;
	if(stream == NULL)
		return ;

	sft_stream = (SftHttpStream *)stream->data_src_desc.sft_stream_handle;

	if(sft_stream)
	{
		sft_stream->Close();
		delete sft_stream;
		sft_stream = NULL;
	}

	if(stream != NULL)
		stream->data_src_desc.sft_stream_handle = NULL;
}

} //end extern "C"

} //end android
#endif //ifdef android4.0.1

