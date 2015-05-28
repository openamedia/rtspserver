#define LOG_TAG "BufferQueue"
#include "android/Log.h"

#include "BufferQueue.h"

BufferQueue::Buffer::Buffer()
	:data(NULL),
	 offset(0),
	 size(0),
	 capacity(0),
	 pts(0),
	 eos(false)
{

}

BufferQueue::Buffer::~Buffer()
{}


BufferQueue::BufferQueue(int bufCount, int bufCapacity)
	:mBufCount(bufCount),
	 mBufCapacity(bufCapacity)
{
	for(int i = 0; i < bufCount; ++i){
		Buffer buf;
		buf.data = (unsigned char*)malloc(bufCapacity);
		buf.capacity = bufCapacity;
		mTotal.push_back(buf);
		mFree.push_back(buf);
	}
}

BufferQueue::~BufferQueue(){
	for(List<Buffer>::iterator it = mTotal.begin(); it != mTotal.end(); it++){
		free(it->data);
		it->data = NULL;
	}	
}

status_t BufferQueue::enque(const Buffer& inbuf, int waitTimeMS){
	Mutex::Autolock ao(mLock);

	for(int times = 0; mFree.empty(); ++times){
		if(times >= 1){
			return TIMED_OUT;
		}
		mCondition.waitRelative(mLock, waitTimeMS * 1000000);
	}
	
	Buffer buf = *(mFree.begin());

	//out of range
	if(buf.capacity < inbuf.size){
		ALOGE("Warning: BufferQueue::enque a inbufsize:%d out of quebuf capacity:%d", inbuf.size, buf.capacity);
		return ERROR_OUT_OF_RANGE;
	}

	mFree.erase(mFree.begin());

	memcpy(buf.data, inbuf.data, inbuf.size);
	buf.size = inbuf.size;
	buf.pts = inbuf.pts;
	buf.eos = inbuf.eos;

	//ALOGE("BufferQueue::enq: buf[4]:%0x, buf.size:%d, this:%p", ((unsigned char*)buf.data)[4], buf.size, this);
	
	mBusy.push_back(buf);

	mCondition.signal();
	
	return OK;
}

status_t BufferQueue::deque(Buffer& outbuf, int waitTimeMS){
	Mutex::Autolock ao(mLock);
	
	for(int times = 0; mBusy.empty(); ++times){
		if(times >= 1){
			return TIMED_OUT;
		}
		mCondition.waitRelative(mLock, waitTimeMS * 1000000);
	}

	Buffer buf = *(mBusy.begin());

	//out of range
	if(buf.size > outbuf.capacity){
		ALOGE("Warning: BufferQueue::deque a quebufsize:%d out of outbuf capacity:%d", buf.size, outbuf.capacity);
		return ERROR_OUT_OF_RANGE;
	}

	if(buf.eos){
		return ERROR_END_OF_STREAM;
	}

	mBusy.erase(mBusy.begin());

	memcpy(outbuf.data, buf.data, buf.size);
	outbuf.size = buf.size;
	outbuf.pts = buf.pts;
	buf.size = 0;
	buf.pts = 0;
	
	mFree.push_back(buf);

	mCondition.signal();
	
	return OK;	
}

void BufferQueue::clear() {
	Mutex::Autolock ao(mLock);
	
	mBusy.clear();
	mFree.clear();

	for(List<Buffer>::iterator it = mTotal.begin(); it != mTotal.end(); it++){
		mFree.push_back(*it);
	}

	mCondition.signal();
}

int BufferQueue::getBufCount() {
	return mBufCount;
}

int BufferQueue::getBufCapacity() {
	return mBufCapacity;
}
