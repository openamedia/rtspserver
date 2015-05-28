#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH

#include "H264VideoFileServerMediaSubsession.hh"
#include "common/BufferQueue.h"
#include "H264LiveFramePool.h"

class H264LiveVideoServerMediaSubssion: public H264VideoFileServerMediaSubsession {

public:
	static H264LiveVideoServerMediaSubssion*
	createNew( UsageEnvironment& env,
               char const* fileName,
			   Boolean reuseFirstSource,
			   H264LiveFramePool* pool);

protected: // we're a virtual base class
	H264LiveVideoServerMediaSubssion( UsageEnvironment& env, char const* fileName, Boolean reuseFirstSource, H264LiveFramePool* pool);
	~H264LiveVideoServerMediaSubssion();

protected: // redefined virtual functions
	FramedSource* createNewStreamSource(unsigned clientSessionId,
										unsigned& estBitrate);
public: 
	char fFileName[100];

private:
	H264LiveFramePool* mPool;
	
};


#endif
