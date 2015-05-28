#define LOG_TAG "H264LiveBufferQueue"
#include "android/Log.h"

#include <time.h>

#include "H264LiveBufferQueue.h"

H264LiveBufferQueue::H264LiveBufferQueue(int bufCount, int bufCapacity)
	:BufferQueue(bufCount, bufCapacity),
	 mState(H264LiveBufferQueue::STATE_UNKNOWN),
	 mTikingStartTime(0)
{
	
}

H264LiveBufferQueue::~H264LiveBufferQueue(){

}


void H264LiveBufferQueue::setState(STATE state){
	Mutex::Autolock ao(mLiveLock);
	
	mState = state;
}

H264LiveBufferQueue::STATE H264LiveBufferQueue::getState(){
	Mutex::Autolock ao(mLiveLock);
	
	return mState;
}

// Returns current time in microseconds
static unsigned int GetTimer(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void H264LiveBufferQueue::setTiking(){
	Mutex::Autolock ao(mLiveLock);
	
    mTikingStartTime = GetTimer();
}

status_t H264LiveBufferQueue::getTikTime(unsigned int* timeMS){
	Mutex::Autolock ao(mLiveLock);

	if(mTikingStartTime == 0){
		ALOGE("tiking time not set yet!!");
		return NO_INIT;
	}
	
	*timeMS = GetTimer() - mTikingStartTime;

	return OK;
}

