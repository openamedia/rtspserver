#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

#include <stdint.h>
#include "android/List.h"
#include "android/Mutex.h"
#include "android/Condition.h"
#include "android/Errors.h"

//a thread safe one.
class BufferQueue {
 public:
	struct Buffer {
		Buffer();
		~Buffer();
		
		void* data;
		int offset;
		int size;
		int capacity;
		int64_t pts;
		bool eos;
	};
	
	status_t enque(const Buffer& buf, int waitTimeMS = 0);
	status_t deque(Buffer& buf, int waitTimeMS = 0);
	void clear();

	int getBufCount();
	int getBufCapacity();

 protected:
	BufferQueue(int bufCount, int bufCapacity);
	~BufferQueue();
	
 private:
	Mutex mLock;
	Condition mCondition;

	int mBufCount;
	int mBufCapacity;
	
	List<Buffer> mTotal;
	List<Buffer> mBusy;
	List<Buffer> mFree;

	BufferQueue(const BufferQueue&);
	BufferQueue& operator=(const BufferQueue&);
};

#endif
