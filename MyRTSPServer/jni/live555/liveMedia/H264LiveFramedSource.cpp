/*
 * Filename:  H264LiveFramedSource.cpp
 * Auther:  mlj
 * Create date: 2013/ 1/22
 */
#define LOG_TAG "H264LiveFramedSource"
#include "android/Log.h"


#include "H264LiveFramedSource.hh"
#include "common/Defs.h"

H264LiveFramedSource::H264LiveFramedSource( UsageEnvironment& env,  
											char const* fileName,
											H264LiveBufferQueue* queue,
											unsigned preferredFrameSize, 
											unsigned playTimePerFrame )
    : FramedSource(env),
	  mBufferQueue(queue)
{
	ALOGE("H264LiveFramedSource::H264LiveFramedSource this:%p, mBufferQueue:%p", this, mBufferQueue);
}

H264LiveFramedSource* H264LiveFramedSource::createNew( UsageEnvironment& env,
													   char const* fileName,
													   H264LiveBufferQueue* queue,
													   unsigned preferredFrameSize /*= 0*/, 
													   unsigned playTimePerFrame /*= 0*/ )
{
    return new H264LiveFramedSource(env, fileName, queue, preferredFrameSize, playTimePerFrame);
}

H264LiveFramedSource::~H264LiveFramedSource()
{
	if(mLastBuffer.data != NULL){
		free(mLastBuffer.data);
		mLastBuffer.data = NULL;
	}
	
	mBufferQueue->setState(H264LiveBufferQueue::STATE_RELEASING);//tell the above H264LiveFramePool to destroy itself in next round feeding.
	ALOGE("H264LiveFramedSource::~H264LiveFramedSource this:%p, mBufferQueue:%p", this, mBufferQueue);
}

void H264LiveFramedSource::doGetNextFrame()
{
	if(mLastBuffer.size != 0){
		int copySize = mLastBuffer.size<(int)fMaxSize?mLastBuffer.size:(int)fMaxSize;
		memcpy(fTo, (unsigned char*)mLastBuffer.data + mLastBuffer.offset, copySize);
		mLastBuffer.offset += copySize;
		mLastBuffer.size -= copySize;
		
		fFrameSize = copySize;
		
		//ALOGE("doGetNextFrame copy from lastbuffer size:%d, fMaxSize:%d", fFrameSize, fMaxSize);
	}else{
		BufferQueue::Buffer buf;
		buf.data = fTo;
		buf.capacity = fMaxSize;
		status_t res = mBufferQueue->deque(buf, H264_DEQUE_FRAME_DEFAULT_WAIT_TIME);
		if(res == OK){
			if(buf.size > (int)fMaxSize){
				ALOGE("Error: bufsize:%d out of range of fMaxSize:%d", buf.size, fMaxSize);
				fFrameSize = 0;
				return;
			}
		
			fFrameSize = buf.size;

			//ALOGE("H264LiveFramedSource::doGetNextFrame deq buf[4]:%0x, buf.size:%d, mBufferQueue:%p", ((unsigned char*)buf.data)[4], buf.size, mBufferQueue);
			//ALOGE("H264LiveFramedSource::doGetNextFrame framesize:%d, fMaxSize:%d", fFrameSize, fMaxSize);
		}else if(res == TIMED_OUT){
			//continue
			//ALOGE("continue and wait for next incoming frame.");
			fFrameSize = 0;
		}else if(res == ERROR_OUT_OF_RANGE){
			//ALOGE("doGetNextFrame ERROR_OUT_OF_RANGE");
			int capacity = mBufferQueue->getBufCapacity();
			if(mLastBuffer.data == NULL){
				mLastBuffer.data = malloc(capacity);
				mLastBuffer.capacity = capacity;
			}

			mLastBuffer.offset = 0;
			mLastBuffer.size = 0;
			
			res = mBufferQueue->deque(mLastBuffer, H264_DEQUE_FRAME_DEFAULT_WAIT_TIME);
			//check res no way can be ERROR_OUT_OF_RANGE anymore.
		}else if(res == ERROR_END_OF_STREAM){
			//get out when eos
			ALOGE("Error: bufqueue returns error ERROR_END_OF_STREAM");
			fFrameSize = 0;
			return;
		}
	}

#if 0
	if(fFrameSize != 0){
		FILE* file = fopen("/data/raw.264", "ab");
		fwrite(fTo, 1, fFrameSize, file);
		fclose(file);
	}
#endif

	
    nextTask() = envir().taskScheduler().scheduleDelayedTask( 0, (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数

    return;
}
