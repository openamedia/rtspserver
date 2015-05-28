#define LOG_TAG "H264LiveFramePool"
#include "android/Log.h"

#include "H264LiveFramePool.h"

#define OUTPUT_MAX_NUM 5
#define SPS_PPS_MAX_SIZE 200
#define SPS_PPS_RESEND_TIME 10000

H264LiveFramePool::H264LiveFramePool(int inBufCount, int inBufCapacity)
	:mStarted(false),
	 mDone(false),
	 mThreadExited(false),
	 mInBufCount(inBufCount),
	 mInBufCapacity(inBufCapacity),
	 mInQueue(new H264LiveBufferQueue(inBufCount, inBufCapacity)),
	 mIsSPSPPSCompleted(false)
{
	mInBuf.data = malloc(mInBufCapacity);
	mInBuf.capacity = mInBufCapacity;

	mSPSPPSBuf.data = malloc(SPS_PPS_MAX_SIZE);
	mSPSPPSBuf.capacity = SPS_PPS_MAX_SIZE;	
	
	start();
}

H264LiveFramePool::~H264LiveFramePool() {
	stop();
	
	if(mInQueue != NULL){
		delete mInQueue;
		mInQueue = NULL;
	}

	for(List<H264LiveBufferQueue*>::iterator it = mOutQueues.begin(); it != mOutQueues.end(); ++it) {
		delete *it;
		*it = NULL;
	}
	mOutQueues.clear();

	free(mInBuf.data);
	mInBuf.data = NULL;
	
	free(mSPSPPSBuf.data);
	mSPSPPSBuf.data = NULL;
}

void H264LiveFramePool::start(){
	Mutex::Autolock ao(mLock);
	
	if(mStarted)
		return;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&mThread, &attr, ThreadWrapper, this);
	pthread_attr_destroy(&attr);

	mDone = false;
	mThreadExited = false;
	mStarted = true;
}

void H264LiveFramePool::stop(){
	Mutex::Autolock ao(mLock);
	
	if(!mStarted)
		return;

	mDone = true;
	while(!mThreadExited){
		mCondition.waitRelative(mLock, 20000000);
	}
	
	mStarted = false;
}


H264LiveBufferQueue* H264LiveFramePool::getInput() {
	return mInQueue;
}

H264LiveBufferQueue* H264LiveFramePool::createOutput() {
	return createOutput(mInBufCount, mInBufCapacity);
}

H264LiveBufferQueue* H264LiveFramePool::createOutput(int outBufCount, int outBufCapacity) {
	Mutex::Autolock ao(mLock);
	
	if(mOutQueues.size() >= OUTPUT_MAX_NUM){
		ALOGE("no more output bufferqueue can be created!!");
		return NULL;
	}

	H264LiveBufferQueue* que = new H264LiveBufferQueue(outBufCount, outBufCapacity);
	que->setState(H264LiveBufferQueue::STATE_PPSPS_NEED_INIT);
	mOutQueues.push_back(que);
	
	return que;
}

void* H264LiveFramePool::ThreadWrapper(void* me) {
	H264LiveFramePool* pool = (H264LiveFramePool*)me;
	pool->threadEntry();

	return NULL;
}

void H264LiveFramePool::threadEntry() {	
	for(;;){
		Mutex::Autolock ao(mLock);
		if(mDone)
			break;
		
		if(mOutQueues.empty()){
			mCondition.waitRelative(mLock, 20000000);
			continue;
		}

		status_t res = mInQueue->deque(mInBuf);
		if(res == TIMED_OUT){
			mCondition.waitRelative(mLock, 20000000);
			continue;
		}else if(res != OK){
			ALOGE("thread exit by inqueue deq returns a non timeout error!!");
			break;
		}
		
		FrameType frameType = getFrameType(mInBuf);
		if(frameType == FRAME_TYPE_SPS){
			writeSPS(mInBuf);
		}else if(frameType == FRAME_TYPE_PPS){
			writePPS(mInBuf);
		}
		
		for(List<H264LiveBufferQueue*>::iterator it = mOutQueues.begin(); it != mOutQueues.end(); ){
			H264LiveBufferQueue* que = *it;

			//destroy the relasing stated bufferqueue set by the bottom H264LiveFramedSource destructor.
			if(que->getState() == H264LiveBufferQueue::STATE_RELEASING){
				ALOGE("release a releasing buffer queue %p!!!", que);
				
				delete que;
				*it = NULL;
				
				it = mOutQueues.erase(it);
				continue;
			}

			//recover from dump.
			if(que->getState() == H264LiveBufferQueue::STATE_DUMP_FRAME){
				if(frameType == FRAME_TYPE_IDR){
					que->setState(H264LiveBufferQueue::STATE_FRAMING);
				}
			}

			//resend sps/pps timely
			if(que->getState() == H264LiveBufferQueue::STATE_FRAMING){
				unsigned int tikTime = 0;
				res = que->getTikTime(&tikTime);
				if(res == OK && tikTime >= SPS_PPS_RESEND_TIME){
					if(frameType == FRAME_TYPE_IDR){
						ALOGE("tik time's up! and resend the sps/pps!!!");
						que->setState(H264LiveBufferQueue::STATE_PPSPS_NEED_INIT);
					}
				}
			}
			
			if(que->getState() == H264LiveBufferQueue::STATE_PPSPS_NEED_INIT){
				ALOGE("set sps to beginning buffer queue!!");
				if(mIsSPSPPSCompleted){
					res = que->enque(mSPSPPSBuf);
					if(res == OK){
						que->setState(H264LiveBufferQueue::STATE_FIRST_NEED_IDR);
						que->setTiking();
					}else if(res == TIMED_OUT){
						ALOGE("timeout enque sps/pps");
						//que->setState(H264LiveBufferQueue::STATE_DUMP_FRAME);
					}
				}else{
					ALOGE("Error: no completed SPS/PPS BUF for init the buffer queue %p!!", que);
				}
			}

			if(que->getState() == H264LiveBufferQueue::STATE_FIRST_NEED_IDR){
				if(frameType == FRAME_TYPE_IDR){
					//do nothing, just set next state and enq.
					res = que->enque(mInBuf);
					if(res == OK){
						que->setState(H264LiveBufferQueue::STATE_FRAMING);
					}else if(res == TIMED_OUT){
						ALOGE("timeout enque idr");
						//que->setState(H264LiveBufferQueue::STATE_DUMP_FRAME);
					}
				}else{
					ALOGE("reject the non idr in STATE_H264_FIRST_NEED_IDR for bufferqueue %p", que);
				}
			}else if(que->getState() == H264LiveBufferQueue::STATE_FRAMING){
				res = que->enque(mInBuf);
				if(res == TIMED_OUT){
					ALOGE("timeout enque normal frame");
					//que->setState(H264LiveBufferQueue::STATE_DUMP_FRAME);
				}
			}else if(que->getState() == H264LiveBufferQueue::STATE_DUMP_FRAME){
				ALOGE("dump frame!!!!!!!");
			}else if(que->getState() != H264LiveBufferQueue::STATE_PPSPS_NEED_INIT){
				ALOGE("Error: unknown buf queue %p state of %d!!!", que, que->getState());
			}
			
			++it;
			continue;
		}
	}

	Mutex::Autolock ao(mLock);
	mThreadExited = true;
	mCondition.signal();
}

H264LiveFramePool::FrameType H264LiveFramePool::getFrameType(const BufferQueue::Buffer& buf){

	FrameType type = FRAME_TYPE_OTHER;

	//0x00 00 00 01 67/68/27/28 ...
	unsigned char flag = ((unsigned char*)buf.data)[4];	
	switch(flag){
	case 0x67:
	case 0x27:
		type = FRAME_TYPE_SPS;
		break;
	case 0x68:
	case 0x28:
		type = FRAME_TYPE_PPS;
		break;
	case 0x65:
	case 0x25:
		type = FRAME_TYPE_IDR;
		break;
	default:
		break;
	}

	return type;
}

//mSPSPPSBuf.data = { SPS, PPS }
void H264LiveFramePool::writeSPS(const BufferQueue::Buffer& buf) {
	ALOGE("writeSPS");
	
	mIsSPSPPSCompleted = false;
	
	if(buf.size > mSPSPPSBuf.capacity){
		ALOGE("fail to writeSPS by buf size:%d out of range:%d.", buf.size, mSPSPPSBuf.capacity);
		return;
	}

	memcpy(mSPSPPSBuf.data, buf.data, buf.size);
	mSPSPPSBuf.size = buf.size;

	checkPPSPSCompleted();
}

void H264LiveFramePool::writePPS(const BufferQueue::Buffer& buf) {
	int ppsOffset = mSPSPPSBuf.size;
	
	if(buf.size > (mSPSPPSBuf.capacity - ppsOffset)){
		ALOGE("fail to writeSPS by buf size:%d out of range:%d.", buf.size, mSPSPPSBuf.capacity);
		return;
	}

	memcpy((unsigned char*)mSPSPPSBuf.data + ppsOffset, buf.data, buf.size);
	mSPSPPSBuf.size += buf.size;

	checkPPSPSCompleted();	
}

void H264LiveFramePool::checkPPSPSCompleted() {
	bool spsfound  = false;
	bool ppsfound = false;

	ALOGE("checkPPSPSCompleted!!");
	
	for(int i = 0; i < (mSPSPPSBuf.size - 5); ++i){
		unsigned char* flag = (unsigned char*)mSPSPPSBuf.data + i;
		if(flag[0] == 0x00 && flag[1] == 0x00 &&
		   flag[2] == 0x00 && flag[3] == 0x01 &&
		   (flag[4] == 0x67 || flag[4] == 0x27)){
			spsfound = true;
			ALOGE("checkPPSPSCompleted sps found!!");
			continue;
		}
		
		if(flag[0] == 0x00 && flag[1] == 0x00 &&
		   flag[2] == 0x00 && flag[3] == 0x01 &&
		   (flag[4] == 0x68 || flag[4] == 0x28)){
			ppsfound = true;
			ALOGE("checkPPSPSCompleted pps found!!");
			break;
		}
	}

	if(spsfound && ppsfound){
		mIsSPSPPSCompleted = true;
	}
}
