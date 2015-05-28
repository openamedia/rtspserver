#ifndef H264_LIVE_BUFFER_QUEUE_H
#define H264_LIVE_BUFFER_QUEUE_H

#include "common/BufferQueue.h"
#include "android/Errors.h"

class H264LiveBufferQueue : public BufferQueue {
 public:
	H264LiveBufferQueue(int bufCount, int bufCapacity);
	~H264LiveBufferQueue();
	
	enum STATE {
		STATE_UNKNOWN = 0,
		
		STATE_PPSPS_NEED_INIT,
		STATE_FIRST_NEED_IDR,
		STATE_DUMP_FRAME,

		STATE_FRAMING,
		STATE_RELEASING,
	};
	
	void setState(STATE state);
	STATE getState();

	void setTiking();
	status_t getTikTime(unsigned int* timeMS);

 private:
	Mutex mLiveLock;
	
	STATE mState;
	unsigned int mTikingStartTime;

	H264LiveBufferQueue(const H264LiveBufferQueue&);
	H264LiveBufferQueue& operator=(const H264LiveBufferQueue&);
};

#endif
