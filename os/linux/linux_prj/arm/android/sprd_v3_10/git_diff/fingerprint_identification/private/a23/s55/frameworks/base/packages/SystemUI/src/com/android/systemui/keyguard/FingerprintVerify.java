package com.android.systemui.keyguard;

import android.os.Handler;
import android.gxFP.FingerprintManager;
import android.gxFP.FingerprintManager.VerifySession;
import android.gxFP.IVerifyCallback;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;


public class FingerprintVerify
{
private FingerprintManager.VerifySession mVerifySession=null;
private FingerprintManager mFingerprintManager=null;
private FpVerifyCallback mCallback;
private static final int MSG_VERIFY_SUCESS = 257;
private static final int MSG_VERIFY_FAIL = 259;
private static final int MSG_FINGER_UNTOUCH =2;
public static final int MSG_WAKEUP_SCREEN =10;

public FingerprintVerify(FpVerifyCallback callback) {
    mCallback=callback;
}

public interface FpVerifyCallback{
		public void onFpVerifySuccess();
		public void onFpVerifyFail();
		public void  WakeupByFp();
}

public void startFingerVerify() {
    //mFingerprintManager=(FingerprintManager)myContext.getSystemService("gxFpService");
    mFingerprintManager=FingerprintManager.getFpManager();
    mVerifySession=mFingerprintManager.newVerifySession(mVerifyCallback);
    mVerifySession.enter();
}

public void stopFingerVerify() {
    mVerifySession.exit();
}

private final IVerifyCallback.Stub mVerifyCallback = new IVerifyCallback.Stub() {
    public void handleMessage(int msg, int arg0, int arg1,byte[] data)
        throws RemoteException {
            switch (msg) {
                case MSG_VERIFY_SUCESS:
                    mFpHandler.sendMessage(mFpHandler.obtainMessage(MSG_VERIFY_SUCESS,
                                arg0, arg1));
                    break;
                case MSG_VERIFY_FAIL:
                    mFpHandler.sendMessage(mFpHandler.obtainMessage(MSG_VERIFY_FAIL,
                                arg0, arg1));
                    break;
                case MSG_FINGER_UNTOUCH:
                    mFpHandler.sendMessage(mFpHandler.obtainMessage(MSG_FINGER_UNTOUCH,
                                arg0, arg1));
                    break;
                default:
                    break;
            }

        }
};

public Handler mFpHandler = new Handler() {
    public void handleMessage(Message msg) {
        switch(msg.what) {
            case MSG_VERIFY_SUCESS:
                 //dismiss(true);
                //   stopfingerverify();
                 //PowerManager mPM = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
                 //PowerManager.WakeLock mFpVerifyWakeLock=mPM.newWakeLock(PowerManager.FULL_WAKE_LOCK|PowerManager.ACQUIRE_CAUSES_WAKEUP|PowerManager.ON_AFTER_RELEASE,"fingerprint wakeup");
                 //     mPM.wakeUp(SystemClock.uptimeMillis());
                // mFpVerifyWakeLock.acquire();
                // mFpVerifyWakeLock.release();  
                mCallback.onFpVerifySuccess();
                break;
            case MSG_VERIFY_FAIL:
                Log.d("fp", "verify fail = %d"+msg);
                mCallback.onFpVerifyFail();
                break;
            case MSG_FINGER_UNTOUCH:
                Log.d("fp", "untouch = %d"+msg);
                break;
            case MSG_WAKEUP_SCREEN:
            		mCallback.WakeupByFp();
            default:
                break;
        }
    }
};
}
