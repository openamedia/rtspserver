#include "Live555MediaServer.h"

#include "android/Log.h"

#include <BasicUsageEnvironment.hh>
#include "DynamicRTSPServer.hh"
#include "version.hh"
#include "common/Defs.h"
#include "H264LiveBufferQueue.h"

Live555MediaServer::Live555MediaServer()
	:mScheduler(NULL),
	 mEnv(NULL),
	 mAuthDB(NULL),
	 mRtspServer(NULL),
	 mDoneFlag(0),
	 mStarted(false),
	 mThreadExited(true),
	 mH264Pool(new H264LiveFramePool(H264_ENQUE_DEFAULT_FRAME_NUM, H264_QUEUE_FRAME_DEFAULT_SIZE))
{
}

Live555MediaServer::~Live555MediaServer(){
	stop();
}

void Live555MediaServer::start(){
	ALOGE("Live555MediaServer::start");
	Mutex::Autolock ao(mLock);

	if(mStarted){
		ALOGW("Live555MediaServer already started!");
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&mThread, &attr, ThreadWrapper, this);
	pthread_attr_destroy(&attr);

	mThreadExited = false;
	mStarted = true;
}

void Live555MediaServer::stop(){
	ALOGE("Live555MediaServer::stop");
	Mutex::Autolock ao(mLock);

	if(!mStarted){
		ALOGW("Live555MediaServer not started yet!");
		return;
	}

	mDoneFlag = 1;
	while(!mThreadExited){
		mCondition.waitRelative(mLock, 20000);
	}
	
	releaseResource();
	
	mStarted = false;
}

void Live555MediaServer::releaseResource(){
	if(mRtspServer != NULL){
		Medium::close(mRtspServer);
		mRtspServer = NULL;
	}

	if(mAuthDB != NULL){
		delete mAuthDB;
		mAuthDB = NULL;
	}

	if(mEnv != NULL){
		mEnv->reclaim();//jim: TODO. not sure whether the "delete this" in base class fnc could call to the inherited's destructors.
		mEnv = NULL;
	}
	
	if(mScheduler != NULL){
		delete mScheduler;
		mScheduler = NULL;
	}
}

//TODO: need More complicated strategy.
void Live555MediaServer::writeH264Live(void* data, int size, int64_t pts){
	//ALOGE("Live555MediaServer::writeH264Live!!!");
	
	BufferQueue::Buffer buf;
	buf.data = data;
	buf.size = size;
	buf.capacity = size;
	buf.pts = pts;
	
	H264LiveBufferQueue* inQueue = mH264Pool->getInput();
	inQueue->enque(buf, H264_ENQUE_FRAME_DEFAULT_WAIT_TIME);
}

void* Live555MediaServer::ThreadWrapper(void* me){
	Live555MediaServer* thiz = (Live555MediaServer*)me;
	thiz->threadEntry();

	return NULL;
}

void Live555MediaServer::threadEntry(){	
	// Begin by setting up our usage environment:
	mScheduler = BasicTaskScheduler::createNew();
	mEnv = BasicUsageEnvironment::createNew(*mScheduler);

	mAuthDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	mAuthDB = new UserAuthenticationDatabase;
	mAuthDB->addUserRecord("username1", "password1"); // replace these with real strings
	// Repeat the above with each <username>, <password> that you wish to allow
	// access to the server.
#endif

	// Create the RTSP server.  Try first with the default port number (554),
	// and then with the alternative port number (8554):
	portNumBits rtspServerPortNum = 554;
	mRtspServer = DynamicRTSPServer::createNew(*mEnv, rtspServerPortNum, mAuthDB, mH264Pool);
	if (mRtspServer == NULL) {
		rtspServerPortNum = 8554;
		mRtspServer = DynamicRTSPServer::createNew(*mEnv, rtspServerPortNum, mAuthDB, mH264Pool);
	}
	if (mRtspServer == NULL) {
		ALOGE("Failed to create RTSP server: %s", mEnv->getResultMsg());
		exit(1);
	}

	ALOGE("LIVE555 Media Server\n"
		  "\tversion %s"
		  " (LIVE555 Streaming Media library version %s)",
		  MEDIA_SERVER_VERSION_STRING, LIVEMEDIA_LIBRARY_VERSION_STRING);
	
	char* urlPrefix = mRtspServer->rtspURLPrefix();
	ALOGE("Play streams from this server using the URL\n\t"
		  "%s"
		  "<filename>\nwhere <filename> is a file present in the current directory.\n"
		  "Each file's type is inferred from its name suffix:\n"
		  "\t\".264\" => a H.264 Video Elementary Stream file\n"
		  "\t\".265\" => a H.265 Video Elementary Stream file\n"
		  "\t\".aac\" => an AAC Audio (ADTS format) file\n"
		  "\t\".ac3\" => an AC-3 Audio file\n"
		  "\t\".amr\" => an AMR Audio file\n"
		  "\t\".dv\" => a DV Video file\n"
		  "\t\".m4e\" => a MPEG-4 Video Elementary Stream file\n"
		  "\t\".mkv\" => a Matroska audio+video+(optional)subtitles file\n"
		  "\t\".mp3\" => a MPEG-1 or 2 Audio file\n"
		  "\t\".mpg\" => a MPEG-1 or 2 Program Stream (audio+video) file\n"
		  "\t\".ogg\" or \".ogv\" or \".opus\" => an Ogg audio and/or video file\n"
		  "\t\".ts\" => a MPEG Transport Stream file\n"
		  "\t\t(a \".tsx\" index file - if present - provides server 'trick play' support)\n"
		  "\t\".vob\" => a VOB (MPEG-2 video with AC-3 audio) file\n"
		  "\t\".wav\" => a WAV Audio file\n"
		  "\t\".webm\" => a WebM audio(Vorbis)+video(VP8) file\n"
		  "See http://www.live555.com/mediaServer/ for additional documentation.", urlPrefix);

	// Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
	// Try first with the default HTTP port (80), and then with the alternative HTTP
	// port numbers (8000 and 8080).

	if (mRtspServer->setUpTunnelingOverHTTP(80) || mRtspServer->setUpTunnelingOverHTTP(8000) || mRtspServer->setUpTunnelingOverHTTP(8080)) {
		ALOGE("(We use port %d for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)", mRtspServer->httpServerPortNum());
	} else {
		ALOGE("(RTSP-over-HTTP tunneling is not available.)");
	}
	
	mEnv->taskScheduler().doEventLoop((char*)&mDoneFlag); // does not return

	Mutex::Autolock ao(mLock);
	mThreadExited = true;
	mCondition.signal();
}
