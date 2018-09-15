/************************************************************************
 * <p>Title: FingerprintManagerService.java</p>
 * <p>Description: </p>
 * <p>Copyright (C), 1997-2014, Shenzhen Goodix Technology Co.,Ltd.</p>
 * <p>Company: Goodix</p>
 * @author  peng.hu
 * @date    2014-9-20
 * @version  1.0
 ************************************************************************/
package com.goodix.service;

import java.util.Vector;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.RemoteException;
import android.util.Log;
import java.util.Timer;
import java.util.TimerTask;
import android.os.Vibrator; 

import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;

import android.gxFP.IEnrollCallback;
import android.gxFP.IFingerprintManager;
import android.gxFP.IVerifyCallback;

import com.goodix.device.CommandType;
import com.goodix.device.FpDevice;
import com.goodix.device.MessageType;
//import com.goodix.fpsetting.LockScreenActivity.LockIVerifyCallback;


/**
 * <p>
 * Title: FingerprintManagerService.java
 * </p>
 * <p>
 * Description: manager fingerprint device
 * </p>
 */
public class gxFpService extends Service {

	private static final String TAG = "FpSetting";
	
	private Timer VerifyTimer=null;
	private boolean ffmod=true;
	private FpDevice device;
 	 private Context mContext;
	private IBinder mPreToken;
	//add by hy 
	private Vibrator vibrator;
	private long[] pattern = {50,100};
	//add by hy end
	final Object mLock = new Object();
	PowerManager.WakeLock mFpVerifyWakeLock=null;
	

	
	private class Client {
		public static final int TYPE_ENROLL = 1;
		public static final int TYPE_VERIFY = 2;
		public IBinder token;
		public int type;
		public Object callback;

		public Client(IBinder token, int type, Object callback) {
			this.token = token;
			this.type = type;
			this.callback = callback;
		}
	}
	
/*	    public gxFpService(Context context) {
		mContext = context;
		Log.v(TAG, "gxfpservice start...");
		systemReady(); 
	   }*/

	    @Override
	    public IBinder onBind(Intent intent) {
		return null;
	    }
    
	@Override
    	public void onCreate() {
         	super.onCreate();
		mContext=getApplicationContext();

		systemReady();



		Log.v("fpservice", "application start");
		try{
		Object object = new Object();
		
		Method addService = Class.forName("android.os.ServiceManager").getMethod("addService", String.class,android.os.IBinder.class);
		Log.v("fpservice", "start fpservice");
		addService.invoke(object, "gxFpService",stub);
		Log.v("fpservice", "add service");
		//System.out.println(obj.toString());
		}catch(ClassNotFoundException ex){
		Log.v("fpservice", "ClassNotFoundException");
		}catch(NoSuchMethodException ex){
		Log.v("fpservice", "NoSuchMethodException");
		}catch(IllegalAccessException ex){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException ex){
		Log.v("fpservice", "InvocationTargetException");
		}
		Log.v("fpservice", "application finish");







	}


	public void systemReady(){
			Log.v(TAG, "gxfpservice system ready...");
		Thread Tready = new Thread(new Runnable (){
	         	public void run() {
	         		device=FpDevice.open();
				HandlerThread mDispatchMessageThread = new HandlerThread("dispatch");
				mDispatchMessageThread.start();
				mDispathMessageHandler = new DispatchMessageHandler(mDispatchMessageThread.getLooper());
				device.setDispathcMessageHandler(mDispathMessageHandler);
				registerScreenActionReceiver();
				// for set chip work mode
				mPmGer = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE); 
				mFpVerifyWakeLock = mPmGer.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "fp verify");  
				mFpVerifyWakeLock.setReferenceCounted(false);
			}
        	});
        Tready.start();
     }
         
         

        
    
	private enum ManagerStatus {
		MANAGER_INIT, 
		MANAGER_IDLE, 
		MANAGER_ENROLL, 
		MANAGER_VERIFY,
	}

	private ManagerStatus mManagerStatus = ManagerStatus.MANAGER_IDLE;

	private enum EventStatus {
		EVENT_IDLE, 
		EVENT_TOUCH, 
		EVENT_UNTOUCH_NO_RESULT, 
		EVENT_RESULT_NO_UNTOUCH,
		EVENT_COMPLETE,
	}

	private EventStatus mEventStatus = EventStatus.EVENT_IDLE;

	private Vector<Client> mClientList = new Vector<Client>();

	private Handler mDispathMessageHandler;

	private PowerManager mPmGer ;


	private void registerScreenActionReceiver() {
		final IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_SCREEN_OFF);
		filter.addAction(Intent.ACTION_SCREEN_ON);
//		filter.addAction(Intent.ACTION_BOOT_COMPLETED);
		filter.setPriority(1001);
		mContext.registerReceiver(receiver, filter);
	}


	private final BroadcastReceiver receiver = new BroadcastReceiver() {

		@Override
		public void onReceive(final Context context, final Intent intent) {
			        synchronized(mLock) {
			String strAction = intent.getAction();
			Log.d(TAG,"FingerprintManagerService .onReceive .Intent.strAction == "+ intent.getAction());
			if (strAction.equals(Intent.ACTION_SCREEN_ON)
					|| strAction.equals(Intent.ACTION_SCREEN_OFF) ||strAction.equals(Intent.ACTION_BOOT_COMPLETED)  ) {
				UpdateStatus(); 
			} 

			}
		}
	};
	
	private void setVerifyTimer()
	{
						synchronized(mLock) {	
			Log.v(TAG, "timer reset...");
			if(mFpVerifyWakeLock.isHeld()==true){
					Log.v(TAG, "verifywakelock release...");
				  mFpVerifyWakeLock.release();
				}
			mFpVerifyWakeLock.acquire(5001);
			if(VerifyTimer!=null)
				 VerifyTimer.cancel();
			VerifyTimer=new Timer(); 
			VerifyTimer.schedule(new TimerTask(){
				@Override
				public void run() {
					Log.v(TAG, "timer cancel...");
					if(mPmGer.isScreenOn()==false) {
						ffmod=true;
						UpdateStatus();
					}
					if(VerifyTimer!=null) {
					VerifyTimer.cancel();
					VerifyTimer=null;
				}
				}},5000);
		}
	}
	
		


	
	public void UpdateStatus() {
		/*
		 * 1. Get pre and next status of mananger. 2. exit pre . 3. Enter next
		 * status.
		 */
		synchronized(mLock) {	
		if(device==null) {
				Log.v(TAG, "FingerprintManagerService : UpdateStatus.gxservice not ready..");
				return;
		}
		int errcode ;
		boolean bScreenOn=mPmGer.isScreenOn();
		if(bScreenOn==true) {
			if(VerifyTimer!=null) {
				 VerifyTimer.cancel();
				 VerifyTimer=null;
				}
		}
		PowerManager.WakeLock mFpWakeLock = mPmGer.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "fp updatestatus");
    mFpWakeLock.acquire();
    
		Log.v(TAG, "FingerprintManagerService : UpdateStatus");
		Log.d(TAG,"FingerprintManagerService : UpdateStatus.mManagerStatus == "+ mManagerStatus);
		ManagerStatus nextStatus = ManagerStatus.MANAGER_IDLE;
		IBinder nextToken = null;
		if (!mClientList.isEmpty()) {
			Client client = mClientList.lastElement();
			if (client.type == Client.TYPE_ENROLL) {
				Log.v(TAG, "nextStatus = ManagerStatus.MANAGER_ENROLL");
				Log.d(TAG,"nextStatus = ManagerStatus.MANAGER_ENROLL");
				nextStatus = ManagerStatus.MANAGER_ENROLL;
			} else if (client.type == Client.TYPE_VERIFY) {
				Log.v(TAG, "nextStatus = ManagerStatus.MANAGER_VERIFY");
				Log.d(TAG,"nextStatus = ManagerStatus.MANAGER_VERIFY");
				nextStatus = ManagerStatus.MANAGER_VERIFY;
			}
			nextToken = client.token;
		} 
		
		
		if (nextStatus == mManagerStatus && nextToken == mPreToken) {
			if (bScreenOn == false && mManagerStatus == ManagerStatus.MANAGER_VERIFY && ffmod==true) {
				Log.d(TAG,"UpdateStatus set FF mode");
				mManagerStatus = ManagerStatus.MANAGER_IDLE;
				device.cancelRecognize_native();
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_FF);
				mEventStatus = EventStatus.EVENT_IDLE;
			}
			mFpWakeLock.release();
			return;
		}

		switch (mManagerStatus) {
		case MANAGER_IDLE:
			break;
		case MANAGER_ENROLL:
			Log.v(TAG, "device.cancelRegister()");
			mManagerStatus = ManagerStatus.MANAGER_IDLE;
		//	device.cancelRegister_native();
			if((errcode=device.cancelRegister_native())!=0){
				Log.v(TAG, "device.cancelRegister() fail!! errcode="+errcode);
			try {
					Thread.sleep(5);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				errcode=device.cancelRegister_native();
				Log.v(TAG, "device.cancelRegister() fail!! errcode="+errcode);
			}
			
			if (nextStatus != ManagerStatus.MANAGER_ENROLL&& nextStatus != ManagerStatus.MANAGER_VERIFY) {
				Log.d(TAG,"set key mode");
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_KEY);
			}			
			
			
		try {
			Thread.sleep(50);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
			break;
		case MANAGER_VERIFY:
			Log.v(TAG, "device.cancelRecognize();");
			Log.d(TAG,"device.cancelRecognize()");
			mManagerStatus = ManagerStatus.MANAGER_IDLE;
			if((errcode=device.cancelRecognize_native())!=0){
				Log.v(TAG, "device.cancelRecognize() fail!! errcode="+errcode);
			try {
					Thread.sleep(5);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				errcode=device.cancelRecognize_native();
				Log.v(TAG, "device.cancelRecognize() fail!! errcode="+errcode);
			}
			if (nextStatus != ManagerStatus.MANAGER_ENROLL&& nextStatus != ManagerStatus.MANAGER_VERIFY) {
				Log.d(TAG,"set key mode");
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_KEY);
			}
		try {
			Thread.sleep(50);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
			break;
		default:
			break;
		}
/*
		try {
			Thread.sleep(30);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
*/		
		switch (nextStatus) {
		case MANAGER_IDLE:
	/*		if (bScreenOn == false) {
				Log.d(TAG,"set FF mode add");
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_FF);
			} else {*/
			Log.d(TAG,"set key mode add");
			device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_KEY);
//		}
			break;
			
		case MANAGER_ENROLL:
			if (bScreenOn == true) {
			Log.v(TAG, "device.register();");
			Log.d(TAG, "device.register();");
			device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_IMG);
			device.getPermission_native("1234");
			device.register_native();
			} else {
				Log.d(TAG,"set FF mode");
				mManagerStatus = ManagerStatus.MANAGER_IDLE;
				mPreToken = nextToken;
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_FF);
	/*		try {
					Thread.sleep(1);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}*/
				mFpWakeLock.release();
				return;
			}
			break;

		case MANAGER_VERIFY:
			Log.v(TAG, "device.recognize();");
			if (bScreenOn == true || ffmod == false) {
				Log.d(TAG,"set imag mode,ffmod="+ffmod);
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_IMG);
				device.recognize_native();
			} else {
				Log.d(TAG,"set FF mode");
				mManagerStatus = ManagerStatus.MANAGER_IDLE;
				mPreToken = nextToken;
				device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_FF);
	/*		try {
					Thread.sleep(1);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}*/
				mFpWakeLock.release();
				return;
	//			device.setMode_native(CommandType.GOODIX_ENGTEST_CMD_SET_MODE_FF);
		//		device.recognize_native();
			}
			break;
		default:
			break;
		}
		mManagerStatus = nextStatus;
		mPreToken = nextToken;
		mEventStatus = EventStatus.EVENT_IDLE;
		Log.d(TAG,"FingerprintManagerService : UpdateStatus.end");
		/*	try {
					Thread.sleep(1);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}*/
		mFpWakeLock.release();
	}
}


	private boolean isTokenExist(IBinder token) {
		if (!mClientList.isEmpty()) {
			for (Client client : mClientList) {
				if (client.token == token) {
					return true;
				}
			}
		}
		return false;
	}

	private boolean removeClient(IBinder token) {
		if (!mClientList.isEmpty()) {
			for (Client client : mClientList) {
				if (client.token == token) {
					mClientList.remove(client);
					return true;
				}
			}
		}
		return false;
	}




private IFingerprintManager.Stub stub = new IFingerprintManager.Stub() {

		//for off screen fingerprint verify	
			public void setFFmod(boolean mod)
			{
						synchronized(mLock) {	
							Log.v(TAG, "setFFmod..");

				ffmod=mod;//if mod=false,disable ffmod for screen off fingerprint verify 
				if(mod==false) {
					setVerifyTimer();
					UpdateStatus();
					ffmod=true;
				} else
					UpdateStatus();
			}
			}
		//for off screen fingerprint verify	end
		@Override
		public int verify(IBinder token, IVerifyCallback callback)
				throws RemoteException {
					mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_VERIFY", "you are not allow to verify fp");
			        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : verify");
			/**
			 * 1. check token , the token has existed in list. 2. add to list.
			 * 3. update status.
			 */
			if (!isTokenExist(token)) {
				Log.v(TAG, "FingerprintManagerService : add client to list");
				mClientList.add(new Client(token, Client.TYPE_VERIFY, callback));
				if(mPmGer.isScreenOn()==false) {
					ffmod=false;
					UpdateStatus();
					ffmod=true;
					setVerifyTimer();
				} else 
					UpdateStatus();
			}
			return 0;
			}
		}

		@Override
		public int cancelVerify(IBinder token) throws RemoteException {
			/*
			 * 1. check the client is last one or not. 2. delete client. 3.
			 * update status if the token is last one.
			 */
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_VERIFY", "you are not allow to verify fp");
			        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : cancelVerify");
		  if (mClientList.size() > 0) {
				if (mClientList.lastElement().token == token) {
					mClientList.removeElement(mClientList.lastElement());
					UpdateStatus();
				} else {
					Log.d(TAG,"cancelVerify : removeClient");
					removeClient(token);
				}
		}
			return 0;
			}
		}

		@Override
		public int enroll(IBinder token, IEnrollCallback callback)
				throws RemoteException {
			/**
			 * 1. check token , the token has existed in list. 2. add to list.
			 * 3. update status.
			 */
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_ENROLL", "you are not allow to enroll fp");
			        synchronized(mLock) {			
			Log.v(TAG, "FingerprintManagerService : enEnroll");
			if (!isTokenExist(token)) {
				mClientList.add(new Client(token, Client.TYPE_ENROLL, callback));
				UpdateStatus();
			}
			return 0;
			}
		}

		@Override
		public int resetEnroll(IBinder token) throws RemoteException {
			/*
			 * 1. check the client is last one or not. 2. delete client. 3.
			 * update status if the token is last one.
			 */
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_ENROLL", "you are not allow to enroll fp");
			        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : resetEnroll");
			if (mClientList.size() > 0) {
				if (mClientList.lastElement().token == token) {
					/* reset device enroll */
					device.resetRegister_native();
				}
			}
			return 0;
			}
		}

		@Override
		public int cancelEnroll(IBinder token) throws RemoteException {
			/*
			 * 1. check the client is last one or not. 2. delete client. 3.
			 * update status if the token is last one.
			 */
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_ENROLL", "you are not allow to enroll fp");
			        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : cancelEnroll");
			if (mClientList.size() > 0) {
				if (mClientList.lastElement().token == token) {
					mClientList.removeElement(mClientList.lastElement());
					UpdateStatus();
				} else {
					removeClient(token);
				}
			}
			return 0;
			}
		}

		@Override
		public int saveEnroll(IBinder token, int index) throws RemoteException {
			/*
			 * 1. Check is the last one token. 2. Check is complete register. 3.
			 * save register.
			 */
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_ENROLL", "you are not allow to enroll fp");
					        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : saveEnroll");
			if (mClientList.size() > 0) {
				Client client = mClientList.lastElement();
				if (client.token == token) {
					device.saveRegister_native(index);
					UpdateStatus();
				}
				/* ignore the request of client which not on top of stack */
			}
			return 0;
			}
		}

		@Override
		public int query() throws RemoteException {
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_VERIFY", "you are not allow to verify fp");
			        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : query template.");
			// TODO : delete
			// device.getPermission("1234");
			return device.query_native();
			}
		}

		public int delete(int i) throws RemoteException {
			 mContext.enforceCallingOrSelfPermission("android.permission.FINGERPRINT_ENROLL", "you are not allow to enroll fp");
			        synchronized(mLock) {
			Log.v(TAG, "FingerprintManagerService : delete template.");
			if (mManagerStatus == ManagerStatus.MANAGER_VERIFY) {
				mManagerStatus = ManagerStatus.MANAGER_IDLE;
				device.cancelRecognize_native();
			}
			if (mManagerStatus != ManagerStatus.MANAGER_ENROLL) {
				device.cancelRegister_native();
			}
			// TODO : delete
			device.getPermission_native("1234");
			int result = device.delete_native(i);
			UpdateStatus();
			return result;
			}
		}
        		
		@Override
		public String getInfo() throws RemoteException {
			        synchronized(mLock) {
			// TODO Auto-generated method stub
			return device.getInfo_native();
			}
		}
};

	

	public void sendMessageToClient(Message msg) {
		if (mClientList.isEmpty()) {
			return;
		}
		Client client = mClientList.lastElement();
		try {
			if (client.type == Client.TYPE_VERIFY) {
				((IVerifyCallback) mClientList.lastElement().callback).handleMessage(msg.what, msg.arg1, msg.arg2,(byte[]) msg.obj);
			} else if (client.type == Client.TYPE_ENROLL) {
				((IEnrollCallback) mClientList.lastElement().callback).handleMessage(msg.what, msg.arg1, msg.arg2,(byte[]) msg.obj);
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	private class DispatchMessageHandler extends Handler {
		public DispatchMessageHandler(Looper looper) {
			super(looper);
		}

		public void handleVerifyMessage(Message msg) {
				if(VerifyTimer!=null)//reset timer
				 	setVerifyTimer();
			Log.v(TAG, "FingerprintManagerService : handleVerifyMessage");
			Log.d(TAG,"FingerprintManagerService : handleVerifyMessage MessageType = " + MessageType.getString(msg.what));
			if (mManagerStatus != ManagerStatus.MANAGER_VERIFY) {
				return;
			}
			switch (msg.what) {
			case MessageType.MSG_TYPE_COMMON_TOUCH:
				if (mEventStatus == EventStatus.EVENT_IDLE) {
					mEventStatus = EventStatus.EVENT_TOUCH;
					/* send message */
					sendMessageToClient(msg);
				}
				break;
			case MessageType.MSG_TYPE_COMMON_UNTOUCH: {
				if (mEventStatus == EventStatus.EVENT_TOUCH
						|| mEventStatus == EventStatus.EVENT_IDLE) {
					mEventStatus = EventStatus.EVENT_UNTOUCH_NO_RESULT;
				} else if (mEventStatus == EventStatus.EVENT_RESULT_NO_UNTOUCH) {
					mEventStatus = EventStatus.EVENT_COMPLETE;
				}
				sendMessageToClient(msg);
			}
				break;

			case MessageType.MSG_TYPE_COMMON_NOTIFY_INFO: {
				sendMessageToClient(msg);
			}
				break;

			case MessageType.MSG_TYPE_RECONGNIZE_SUCCESS:
			case MessageType.MSG_TYPE_RECONGNIZE_TIMEOUT:
			case MessageType.MSG_TYPE_RECONGNIZE_FAILED:
			case MessageType.MSG_TYPE_RECONGNIZE_BAD_IMAGE:
			case MessageType.MSG_TYPE_RECONGNIZE_GET_DATA_FAILED:
			case MessageType.MSG_TYPE_RECONGNIZE_NO_REGISTER_DATA: {
				if (mEventStatus == EventStatus.EVENT_TOUCH
						|| mEventStatus == EventStatus.EVENT_IDLE) {
					mEventStatus = EventStatus.EVENT_RESULT_NO_UNTOUCH;
				} else if (mEventStatus == EventStatus.EVENT_UNTOUCH_NO_RESULT) {
					mEventStatus = EventStatus.EVENT_COMPLETE;
				}
				sendMessageToClient(msg);
				break;
			}
			default:
				break;
			}
			if (mEventStatus == EventStatus.EVENT_COMPLETE) {
				Log.v(TAG, "EVENT_COMPLETE : device.recognize()");
				mEventStatus = EventStatus.EVENT_IDLE;
				Log.d(TAG,"EVENT_COMPLETE : device.recognize()");
				device.recognize_native();

			}
		}

		private int percent = 0;

		public void handleEnrollMessage(Message msg) {
			Log.v(TAG, "FingerprintManagerService : handleEnrollMessage");
			Log.v(TAG, "MessageType = " + MessageType.getString(msg.what));
			Log.d(TAG,"FingerprintManagerService : handleEnrollMessage MessageType = " + MessageType.getString(msg.what));

			if (mManagerStatus != ManagerStatus.MANAGER_ENROLL) {
				return;
			}
			switch (msg.what) {
			case MessageType.MSG_TYPE_COMMON_TOUCH:
				if (mEventStatus == EventStatus.EVENT_IDLE) {
					mEventStatus = EventStatus.EVENT_TOUCH;
					sendMessageToClient(msg);
					/* send message */
				}
				break;
			case MessageType.MSG_TYPE_COMMON_UNTOUCH: {
				if (mEventStatus == EventStatus.EVENT_TOUCH
						|| mEventStatus == EventStatus.EVENT_IDLE) {
					mEventStatus = EventStatus.EVENT_UNTOUCH_NO_RESULT;
				} else if (mEventStatus == EventStatus.EVENT_RESULT_NO_UNTOUCH) {
					mEventStatus = EventStatus.EVENT_COMPLETE;
				}
				sendMessageToClient(msg);
			}
				break;

			case MessageType.MSG_TYPE_COMMON_NOTIFY_INFO: 
			case MessageType.MSG_TYPE_REGISTER_DUPLICATE_REG:
				sendMessageToClient(msg);
			
				break;

			case MessageType.MSG_TYPE_REGISTER_PIECE:
			case MessageType.MSG_TYPE_REGISTER_NO_PIECE:
			case MessageType.MSG_TYPE_REGISTER_NO_EXTRAINFO:
			case MessageType.MSG_TYPE_REGISTER_LOW_COVER:
			case MessageType.MSG_TYPE_REGISTER_BAD_IMAGE:
			case MessageType.MSG_TYPE_REGISTER_GET_DATA_FAILED:
			case MessageType.MSG_TYPE_ERROR: {
				if (mEventStatus == EventStatus.EVENT_TOUCH
						|| mEventStatus == EventStatus.EVENT_IDLE) {
					mEventStatus = EventStatus.EVENT_RESULT_NO_UNTOUCH;
					Log.d(TAG,"mEventStatus = EventStatus.EVENT_RESULT_NO_UNTOUCH");

				} else if (mEventStatus == EventStatus.EVENT_UNTOUCH_NO_RESULT) {
					mEventStatus = EventStatus.EVENT_COMPLETE;
				}
				percent = msg.arg1;
				sendMessageToClient(msg);
			}
				break;

			default:
				break;
			}
			if (mEventStatus == EventStatus.EVENT_COMPLETE && percent < 100) {
				Log.v(TAG, "EVENT_COMPLETE : device.register()");
				Log.d(TAG,"EVENT_COMPLETE : device.register()");

				mEventStatus = EventStatus.EVENT_IDLE;
			     // device.getPermission("1234");
				device.register_native();
				percent = 0;
			}
		}

		public void handleMessage(Message msg) {
			Log.v(TAG, "FingerprintManagerService : handleMessage mManagerStatus == " + mManagerStatus);
			if (mManagerStatus == ManagerStatus.MANAGER_VERIFY) {
				handleVerifyMessage(msg);
			} else if (mManagerStatus == ManagerStatus.MANAGER_ENROLL) {
				handleEnrollMessage(msg);
			} else
			//add by hy
			{
				if(mClientList.size()>0){
				if((mPmGer.isScreenOn()==false)&& (mClientList.lastElement().type== Client.TYPE_VERIFY)) {
					if(msg.what==MessageType.MSG_TYPE_COMMON_UNTOUCH){
				  Log.v(TAG, "msg received on screen off,try ffmod.. " );
					Thread upThread=new Thread(new Runnable(){
						@Override
						public void run() {
				//			synchronized(mLock) {	
					//		ffmod=false;
							UpdateStatus();
					//		ffmod=true;
				//		}
						}
				  });
				 upThread.start();
				}else if(msg.what==MessageType.MSG_TYPE_COMMON_TOUCH) {
						vibrator = (Vibrator) mContext.getSystemService(Context.VIBRATOR_SERVICE);
		    		vibrator.vibrate(pattern,-1);
				}
				
			}
			}
		}
			//add by hy end
		}
	}
}
