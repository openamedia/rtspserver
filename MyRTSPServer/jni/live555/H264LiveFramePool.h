#ifndef H264_LIVE_FRAME_POOL_H
#define H264_LIVE_FRAME_POOL_H

#include "android/List.h"
#include "H264LiveBufferQueue.h"
#include <pthread.h>

class H264LiveFramePool {
 public:
	H264LiveFramePool(int inBufCount, int inBufCapacity);
	~H264LiveFramePool();
	
	H264LiveBufferQueue* getInput();

	H264LiveBufferQueue* createOutput();
	H264LiveBufferQueue* createOutput(int outBufCount, int outBufCapacity);

	void start();
	void stop();
	
 private:
	Mutex mLock;
	Condition mCondition;

	bool mStarted;
	
	pthread_t mThread;
	bool mDone;
	bool mThreadExited;
	static void* ThreadWrapper(void* me);
	void threadEntry();

	int mInBufCount;
	int mInBufCapacity;
	H264LiveBufferQueue* mInQueue;
	BufferQueue::Buffer mInBuf;
	
	List<H264LiveBufferQueue*> mOutQueues;

	BufferQueue::Buffer mSPSPPSBuf;
	bool mIsSPSPPSCompleted;

	enum FrameType {
		FRAME_TYPE_SPS,
		FRAME_TYPE_PPS,
		FRAME_TYPE_IDR,
		FRAME_TYPE_OTHER,
	};

	FrameType getFrameType(const BufferQueue::Buffer& buf);
	void writeSPS(const BufferQueue::Buffer& buf);
	void writePPS(const BufferQueue::Buffer& buf);
	void checkPPSPSCompleted();
	
	H264LiveFramePool(const H264LiveFramePool&);
	H264LiveFramePool& operator=(const H264LiveFramePool&);
};


#endif
