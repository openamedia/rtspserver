package com.ingenic.glass.rtspserver.service;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import com.ingenic.glass.rtspserver.aidl.IRTSPServerListener;
import com.ingenic.glass.rtspserver.aidl.IRTSPServer;

public class Live555RTSPService extends Service {
	private static final String TAG = "Live555RTSPService";
	
	private IRTSPServerListener mRemoteListener = null;
	
	private Live555MediaServer mServer = null;
	
	public Live555RTSPService() {
		Log.e(TAG, "Live555RTSPService::Live555RTSPService");
		mServer = Live555MediaServer.create();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}
	
	private final static int MSG_START = 0;
	private final static int MSG_STOP = 1;
	private final static int MSG_SET_LISTENER = 2;
	private final static int MSG_WRITE_LIVE_H264 = 3;
	
    private Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch(msg.what){
            case MSG_START:
            {
            	Log.e(TAG, "Live555RTSPService::MSG_START");
            	mServer.start();
            	break;
            }
            case MSG_STOP:
            {
            	Log.e(TAG, "Live555RTSPService::MSG_STOP");
            	mServer.stop();
            	break;
            }
            case MSG_SET_LISTENER:
            {
            	Log.e(TAG, "Live555RTSPService::MSG_SET_LISTENER");
            	mRemoteListener = (IRTSPServerListener)msg.obj;
            	break;
            }
            case MSG_WRITE_LIVE_H264:
            {
            	Log.e(TAG, "Live555RTSPService::MSG_WRITE_LIVE_H264");
            	byte[] buf = (byte[])msg.obj;
            	int offset = msg.arg1;
            	int length = msg.arg2;
            	mServer.writeLiveH264(buf, offset, length, 0);
            	break;
            }
            default:
            	break;
            }
        }
    };

	private IRTSPServer.Stub mBinder = new IRTSPServer.Stub() {

		@Override
		public void setListener(IRTSPServerListener arg0)
				throws RemoteException {
			Message msg = mHandler.obtainMessage();
			msg.what = MSG_SET_LISTENER;
			msg.obj = arg0;
			mHandler.sendMessage(msg);
		}

		@Override
		public void start() throws RemoteException {
			Message msg = mHandler.obtainMessage();
			msg.what = MSG_START;
			mHandler.sendMessage(msg);
		}

		@Override
		public void stop() throws RemoteException {
			Message msg = mHandler.obtainMessage();
			msg.what = MSG_STOP;
			mHandler.sendMessage(msg);	
		}

		@Override
		public void writeLiveH264(byte[] arg0, int arg1, int arg2, int arg3)
				throws RemoteException {
			Message msg = mHandler.obtainMessage();
			msg.what = MSG_WRITE_LIVE_H264;
			msg.obj = arg0;
			msg.arg1 = arg1;
			msg.arg2 = arg2;
			mHandler.sendMessage(msg);
		}

	};
	
}
