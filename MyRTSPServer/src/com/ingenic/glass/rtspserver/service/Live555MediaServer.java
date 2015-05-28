package com.ingenic.glass.rtspserver.service;

import java.lang.ref.WeakReference;

public class Live555MediaServer {
	
	static {
		System.loadLibrary("rtspjni");
		nativeInit();
	}
	
	private static Live555MediaServer mSingletonInstance = null;
	
	public static Live555MediaServer create(){
		if(mSingletonInstance == null)
			mSingletonInstance = new Live555MediaServer();
		
		return mSingletonInstance;
	}
	
	private Live555MediaServer(){
		nativeSetup(new WeakReference<Live555MediaServer>(this));
	}
	
	public void start(){
		nativeStart();
	}
	
	public void stop(){
		nativeStop();
	}
	
	public void release(){
		nativeRelease();
	}
	
	public void writeLiveH264(byte[] buf, int offset, int length, int flag){
		nativeWriteLiveH264(buf, offset, length, flag);
	}
	
	private int mNativeContext = 0;
	
	private static native final void nativeInit();
	private native final void nativeSetup(Object aplayer_this);
	private native void nativeStart();
	private native void nativeStop();
	private native void nativeRelease();
	private native void nativeWriteLiveH264(byte[] buf, int offset, int length, int flag);
	
    private static void postEventFromNative(Object server_ref, int what, int arg1, int arg2, byte[] bytes){
    	Live555MediaServer ap = (Live555MediaServer)((WeakReference)server_ref).get();
    	if (ap == null) {
    		return;
    	}

    	/*
    	if (ap.mEventHandler != null) {
    		Message m = ap.mEventHandler.obtainMessage(what, arg1, arg2);
    		m.obj = bytes;
    		ap.mEventHandler.sendMessage(m);
    	}
    	*/
    }

}
