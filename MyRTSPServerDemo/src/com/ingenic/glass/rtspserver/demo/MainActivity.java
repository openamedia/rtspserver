package com.ingenic.glass.rtspserver.demo;

import java.io.IOException;
import java.nio.ByteBuffer;

import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.os.Build;

import com.ingenic.glass.rtspserver.aidl.IRTSPServerListener;
import com.ingenic.glass.rtspserver.aidl.IRTSPServer;

public class MainActivity extends Activity {

	private final static String TAG = "RTSPDEMO MainActivity";
	
	private IRTSPServer mServer = null;
	
	private ServiceConnection mServiceConnection = new ServiceConnection() {

		@Override
		public void onServiceConnected(ComponentName arg0, IBinder arg1) {
			Log.e(TAG, "onServiceConnected 0");
			
			mServer = IRTSPServer.Stub.asInterface(arg1);
			
			try {
				mServer.setListener(IRTSPServerListener.Stub.asInterface(mBinder));
			} catch (RemoteException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			
			Log.e(TAG, "onServiceConnected 1");

			try {
				mServer.start();
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			/*
			Log.e(TAG, "onServiceConnected 2");
			cam = Camera.open(0);
			try {
				cam.setPreviewDisplay(sv.getHolder());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}                    
			
			Log.e(TAG, "surfaceCreated 1");
			
			Camera.Parameters parameters = cam.getParameters();  
			parameters.setFlashMode("off"); // 无闪光灯  
			parameters.setWhiteBalance(Camera.Parameters.WHITE_BALANCE_AUTO);  
			parameters.setSceneMode(Camera.Parameters.SCENE_MODE_AUTO);  
			parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);   
			parameters.setPreviewFormat(ImageFormat.NV21);       
			//parameters.setPictureSize(camWidth, camHeight);  
			int camWidth = 640;
			int camHeight = 480;
			parameters.setPreviewSize(camWidth, camHeight);  
			cam.setParameters(parameters); 

			Log.e(TAG, "surfaceCreated 2");
			
			mc = MediaCodec.createEncoderByType("video/avc");
			MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", camWidth, camHeight);  
			mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 125000);  
			mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 15);  
			mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);  
			mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 5);
			mc.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
			mc.start();
			
			Log.e(TAG, "surfaceCreated 3");
			
			buf = new byte[camWidth * camHeight * 3 / 2];
			cam.addCallbackBuffer(buf);
			cam.setPreviewCallback(new PreviewCallback(){

				@Override
				public void onPreviewFrame(byte[] arg0, Camera arg1) {
					onFrame(arg0, 0, arg0.length, 0);
					
					cam.addCallbackBuffer(buf);
				}
				
			});
			cam.startPreview();
			*/
			
			/*
			sv.getHolder().addCallback(new Callback(){

				@Override
				public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2,
						int arg3) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void surfaceCreated(SurfaceHolder arg0) {
					Log.e(TAG, "surfaceCreated 0");
					
					cam = Camera.open();
					try {
						cam.setPreviewDisplay(arg0);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}                    
					
					Log.e(TAG, "surfaceCreated 1");
					
					Camera.Parameters parameters = cam.getParameters();  
					parameters.setFlashMode("off"); // 无闪光灯  
					parameters.setWhiteBalance(Camera.Parameters.WHITE_BALANCE_AUTO);  
					parameters.setSceneMode(Camera.Parameters.SCENE_MODE_AUTO);  
					parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);   
					parameters.setPreviewFormat(ImageFormat.NV21);       
					//parameters.setPictureSize(camWidth, camHeight);  
					int camWidth = 600;
					int camHeight = 480;
					parameters.setPreviewSize(camWidth, camHeight);  
					cam.setParameters(parameters); 

					Log.e(TAG, "surfaceCreated 2");
					
					mc = MediaCodec.createEncoderByType("Video/AVC");
					MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", camWidth, camHeight);  
					mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 125000);  
					mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 15);  
					mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);  
					mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 5);
					mc.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
					mc.start();
					
					Log.e(TAG, "surfaceCreated 3");
					
					buf = new byte[camWidth * camHeight * 3 / 2];
					cam.addCallbackBuffer(buf);
					cam.setPreviewCallback(new PreviewCallback(){

						@Override
						public void onPreviewFrame(byte[] arg0, Camera arg1) {
							onFrame(arg0, 0, arg0.length, 0);
							
							cam.addCallbackBuffer(buf);
						}
						
					});
					cam.startPreview();
				}

				@Override
				public void surfaceDestroyed(SurfaceHolder arg0) {
					// TODO Auto-generated method stub
					
				}
				
			});
			*/
			
		}

		@Override
		public void onServiceDisconnected(ComponentName arg0) {
			// TODO Auto-generated method stub
			try {
				mServer.stop();
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}
	 };
	 
		public void onFrame(byte[] buf, int offset, int length, int flag) {  
			   ByteBuffer[] inputBuffers = mc.getInputBuffers();  
			   ByteBuffer[] outputBuffers = mc.getOutputBuffers();  
			   int inputBufferIndex = mc.dequeueInputBuffer(-1);  
			   if (inputBufferIndex >= 0){
			       ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];  
			       inputBuffer.clear();  
			       inputBuffer.put(buf, offset, length);  
			       mc.queueInputBuffer(inputBufferIndex, 0, length, 0, 0);  
			   }  
			   
			   MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
			   int outputBufferIndex = mc.dequeueOutputBuffer(bufferInfo,0);
			   while (outputBufferIndex >= 0) {
			       ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
			       
			       
			      try {
					mServer.writeLiveH264(buf, offset, length, flag);
				} catch (RemoteException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			      // if (frameListener != null)
			      //     frameListener.onFrame(outputBuffer, 0, length, flag);
			       
			       
			       
			       mc.releaseOutputBuffer(outputBufferIndex, false);
			       outputBufferIndex = mc.dequeueOutputBuffer(bufferInfo, 0);
			   }
		}
	 
	 private IRTSPServerListener.Stub mBinder = new IRTSPServerListener.Stub() {

		@Override
		public void onError(String arg0) throws RemoteException {
			// TODO Auto-generated method stub
			
		}

	 };
	
	 Camera cam = null;
	 SurfaceView sv = null;
	 MediaCodec mc = null;
	 byte[] buf = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		if (savedInstanceState == null) {
			getFragmentManager().beginTransaction()
					.add(R.id.container, new PlaceholderFragment()).commit();
		}
		
		this.bindService(new Intent("com.ingenic.glass.rtspserver.service.Live555RTSPService"), mServiceConnection, Context.BIND_AUTO_CREATE);
		
		sv = (SurfaceView)this.findViewById(R.id.surfaceView1);
	}

	

	
    @Override
    public void onDestroy() {
    	this.unbindService(mServiceConnection);
    	
    	super.onDestroy();
    }

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {

		public PlaceholderFragment() {
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.fragment_main, container,
					false);
			return rootView;
		}
	}

}
