#ifndef LIVE555_MEDIA_SERVER_H_
#define LIVE555_MEDIA_SERVER_H_

#include <pthread.h>

#include "android/Mutex.h"
#include "android/Condition.h"
#include "H264LiveFramePool.h"

class TaskScheduler;
class UsageEnvironment;
class UserAuthenticationDatabase;
class RTSPServer;

class Live555MediaServer {
public:
	Live555MediaServer();
	~Live555MediaServer();
	
	void start();
	void stop();

	void writeH264Live(void* data, int size, int64_t pts);

private:
	TaskScheduler* mScheduler;
	UsageEnvironment* mEnv;
	UserAuthenticationDatabase* mAuthDB;
	RTSPServer* mRtspServer;
	volatile char mDoneFlag;
	bool mStarted;
	
	pthread_t mThread;
	bool mThreadExited;
	static void* ThreadWrapper(void* me);
	void threadEntry();

	Mutex mLock;
	Condition mCondition;
	
	H264LiveFramePool* mH264Pool;

	void releaseResource();

	Live555MediaServer(const Live555MediaServer&);
	Live555MediaServer& operator=(const Live555MediaServer&);
};


#endif /* LIVE555_RTSP_SERVER_H_ */
