#ifndef LIVE_SOURCE_H
#define LIVE_SOURCE_H

#include <stdint.h>

class LiveSource
{
 public:
	virtual status_t write(void* data, int size, int64_t pts) = 0;

 protected:
	LiveSource(){}
	virtual ~LiveSource(){}
	
 private:
	LiveSource(const LiveSource&);
	LiveSource& operator=(const LiveSource&);
};


#endif
