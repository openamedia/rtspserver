#include <stdint.h>
#include <jni.h>
#include "com_ingenic_glass_rtspserver_service_Live555MediaServer.h"
#include "android/Log.h"
#include "live555/Live555MediaServer.h"

static jfieldID JCLASS_FIELD_ID_NATIVE_CONTEXT = 0;
static jmethodID JCLASS_METHOD_ID_POST_EVENT_FROM_NATIVE = 0;

static JavaVM* g_jvm = NULL;


//call back to java
static Live555MediaServer* getMediaServer(JNIEnv* env, jobject thiz){
	Live555MediaServer* mp = (Live555MediaServer*)env->GetIntField(thiz, JCLASS_FIELD_ID_NATIVE_CONTEXT);
	return mp;
}

//call back to java
static void setMediaServer(JNIEnv* env, jobject thiz, const Live555MediaServer* server){
	Live555MediaServer* old = getMediaServer(env, thiz);
	if(old != NULL){
		delete old;
		old = NULL;
	}

	if(server){
		env->SetIntField(thiz, JCLASS_FIELD_ID_NATIVE_CONTEXT, (int)server);
	}
}

JNIEXPORT void JNICALL Java_com_ingenic_glass_rtspserver_service_Live555MediaServer_nativeInit(JNIEnv *env, jclass clazz){
	JCLASS_FIELD_ID_NATIVE_CONTEXT = env->GetFieldID(clazz, "mNativeContext", "I");
	JCLASS_METHOD_ID_POST_EVENT_FROM_NATIVE = env->GetStaticMethodID(clazz, "postEventFromNative", "(Ljava/lang/Object;III[B)V");
}

JNIEXPORT void JNICALL Java_com_ingenic_glass_rtspserver_service_Live555MediaServer_nativeSetup(JNIEnv *env, jobject thiz, jobject weak_this){
	Live555MediaServer* server = new Live555MediaServer;
	if (server == NULL) {
        ALOGE("create Live555MediaServer failed by Out of memory!");
        return;
    }

	setMediaServer(env, thiz, server);
}


JNIEXPORT void JNICALL Java_com_ingenic_glass_rtspserver_service_Live555MediaServer_nativeStart(JNIEnv *env, jobject thiz){
	Live555MediaServer* server = getMediaServer(env, thiz);
	if(server == NULL){
		ALOGE("no Live555MediaServer found for start");
		return;
	}

	server->start();
}


JNIEXPORT void JNICALL Java_com_ingenic_glass_rtspserver_service_Live555MediaServer_nativeStop(JNIEnv *env, jobject thiz){
	Live555MediaServer* server = getMediaServer(env, thiz);
	if(server == NULL){
		ALOGE("no Live555MediaServer found for start");
		return;
	}

	server->stop();
}

JNIEXPORT void JNICALL Java_com_ingenic_glass_rtspserver_service_Live555MediaServer_nativeWriteLiveH264(JNIEnv *env, jobject thiz, jbyteArray jbuf, jint joffset, jint jlength, jint jflag){
	Live555MediaServer* server = getMediaServer(env, thiz);
	if(server == NULL){
		ALOGE("no Live555MediaServer found for start");
		return;
	}

    jbyte* bytes = env->GetByteArrayElements(jbuf, NULL);

	server->writeH264Live(bytes, jlength, 0);

	env->ReleaseByteArrayElements(jbuf, bytes, 0);//many times running without release will cause a dvm error of "W/dalvikvm(  946): ReferenceTable overflow (max=1024)".
}


JNIEXPORT void JNICALL Java_com_ingenic_glass_rtspserver_service_Live555MediaServer_nativeRelease(JNIEnv *env, jobject thiz){
	setMediaServer(env, thiz, NULL);
}
