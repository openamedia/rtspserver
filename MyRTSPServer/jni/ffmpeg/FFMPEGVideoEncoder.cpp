#include "FFMPEGVideoEncoder.h"

FFMPEGVideoEncoder::FFMPEGVideoEncoder(VideoEncParam param){

	bool inited = false;
	do{
		av_register_all();
		pFormatCtx = avformat_alloc_context();
		pCodecH264 = avcodec_find_encoder(CODEC_ID_H264);
		if(pCodecH264 == NULL){  
			ALOGE("h264 codec not found\n");  
			break;
		}

		AVRational rate;
		rate.num = 1;
		rate.den = 25;
		c = avcodec_alloc_context3(pCodecH264);
		c->bit_rate = param.bitrate;
		c->width =param.width;
		c->height = param.height;
		c->time_base= rate;
		c->gop_size = 10;
		c->max_b_frames=1;
		c->thread_count = 1;
		if(param.fmt == COLOR_FMT_YUV_420P){
			c->pix_fmt = PIX_FMT_YUV420P;
		}else{
			ALOGE("Error: unknown fmt!!");
			break;
		}

		if(avcodec_open2(c, pCodecH264,NULL)<0){
			ALOGE("Error: can not open codec!");
			break;
		}

		inited = true;
	}while(0);

	if(!inited){
		
	}

	m_pYUVFrame = new AVFrame;
	
}

FFMPEGVideoEncoder::~FFMPEGVideoEncoder(){
	
}

status_t FFMPEGVideoEncoder::encode(const void* indata, int insize, void* outdata, int* outsize){
	
}
