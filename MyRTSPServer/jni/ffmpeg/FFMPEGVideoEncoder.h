#ifndef FFMPEG_VIDEO_ENCODER_H
#define FFMPEG_VIDEO_ENCODER_H

class FFMPEGVideoEncoder {
 public:
	enum VIDEO_ENC_TYPE {
		VIDEO_ENC_H264,
	}

	enum COLOR_FMT_TYPE {
		COLOR_FMT_YUV_420P,
	}

	struct VideoEncParam {
		VIDEO_ENC_TYPE type;
		COLOR_FMT_TYPE fmt;
		int bitrate;
		int width;
		int height;
	}
	
	FFMPEGVideoEncoder(VideoEncParam param);
	~FFMPEGVideoEncoder();

	bool isValid();	

	status_t encode(const void* indata, int insize, void* outdata, int* outsize);
	
 private:
	bool mIsValid;

	AVFormatContext* pFormatCtx;
	AVCodec* pCodecH264;
	AVCodecContext *c;
	AVFrame *m_pYUVFrame;

	FFMPEGVideoEncoder(const FFMPEGVideoEncoder&);
	FFMPEGVideoEncoder& operator=(const FFMPEGVideoEncoder&);
};


#endif
