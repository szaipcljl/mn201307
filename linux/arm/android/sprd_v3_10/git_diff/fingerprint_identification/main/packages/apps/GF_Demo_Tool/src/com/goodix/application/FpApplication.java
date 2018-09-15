/************************************************************************
 * <p>Title: CrashApplication.java</p>
 * <p>Description: </p>
 * <p>Copyright (C), 1997-2014, Shenzhen Goodix Technology Co.,Ltd.</p>
 * <p>Company: Goodix</p>
 * @author   peng.hu
 * @date     2014-5-20
 * @version  1.0
 ************************************************************************/
package com.goodix.application;

import android.app.Application;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;


import android.gxFP.FingerprintManager;
import com.goodix.util.L;

/**
 * <p>
 * Title: CrashApplication.java
 * </p>
 * <p>
 * Description: Record Crash CallStack
 * </p>
 */
public class FpApplication extends Application {

	private FingerprintManager mFingerprintManager;
	private static FpApplication mApplication;

	@Override
	public void onCreate() {
		super.onCreate();
		mApplication = this;
		CrashHandler crashHandler = CrashHandler.getInstance();
		crashHandler.init(this);
		initFpMangerService();
	}

	private void initFpMangerService() {
		if (mFingerprintManager == null) {
	//		mFingerprintManager=(FingerprintManager)this.getSystemService("gxFpService");
			mFingerprintManager=FingerprintManager.getFpManager();
		}
	}



	public FingerprintManager getFpServiceManager() {
		if (mFingerprintManager == null) {
			L.d("mFingerprintManager is null!!!!!");
			initFpMangerService();
		}
		return mFingerprintManager;
	}

	public synchronized static FpApplication getInstance() {
		return mApplication;
	}
}
