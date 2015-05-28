#include "H264LiveVideoServerMediaSubssion.hh"
#include "H264LiveFramedSource.hh"
#include "H264VideoStreamFramer.hh"

H264LiveVideoServerMediaSubssion*
H264LiveVideoServerMediaSubssion::createNew( UsageEnvironment& env,
                                             char const* fileName, 
                                             Boolean reuseFirstSource,
											 H264LiveFramePool* pool)	
{
    return new H264LiveVideoServerMediaSubssion( env, fileName, reuseFirstSource, pool);
}

H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion( UsageEnvironment& env, char const* fileName, Boolean reuseFirstSource, H264LiveFramePool* pool)
	: H264VideoFileServerMediaSubsession( env, fileName, reuseFirstSource ),
	  mPool(pool)
{
    strcpy(fFileName,fileName);
}


H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion()
{
}

FramedSource* H264LiveVideoServerMediaSubssion::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
    /* Remain to do : assign estBitrate */
    estBitrate = 1000; // kbps, estimate

    // Create the video source:
    H264LiveFramedSource* liveSource = H264LiveFramedSource::createNew(envir(), fFileName, mPool->createOutput());
    if (liveSource == NULL){
		return NULL;
	}

    // Create a framer for the Video Elementary Stream:
    return H264VideoStreamFramer::createNew(envir(), liveSource);
}
