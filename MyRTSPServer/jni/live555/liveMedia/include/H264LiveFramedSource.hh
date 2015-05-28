#ifndef H264_LIVE_FRAMED_SOURCE_HH
#define H264_LIVE_FRAMED_SOURCE_HH

#include <FramedSource.hh>
#include "H264LiveBufferQueue.h"

class H264LiveFramedSource : public FramedSource
{
public:
    static H264LiveFramedSource* createNew(UsageEnvironment& env,
										   char const* fileName,
										   H264LiveBufferQueue* queue,
										   unsigned preferredFrameSize = 0,
										   unsigned playTimePerFrame = 0);
	
protected:
    H264LiveFramedSource(UsageEnvironment& env,
						 char const* fileName,
						 H264LiveBufferQueue* queue,
						 unsigned preferredFrameSize,
						 unsigned playTimePerFrame);
    // called only by createNew()
    ~H264LiveFramedSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    int TransportData( unsigned char* to, unsigned maxSize );

private:
	H264LiveBufferQueue* mBufferQueue;

	BufferQueue::Buffer mLastBuffer;
};

#endif
