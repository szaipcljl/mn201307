package com.intel.sensorhub;

import android.util.Log;
import com.intel.sensorhub.CalibrateStepCallack;

public class CalibrationStepOne implements Runnable {
	private static final String TAG = "CalibrationStepOne";

	private CalibrateStepCallack mCalibrateStepCallack;

	@Override
	public void run() {
		// TODO Auto-generated method stub
		Log.d(TAG, "start CalibrationStepOne");
		SensorHubNativeInterface mSensorHubNativeInterface = new SensorHubNativeInterface();

		// Thread.sleep(2000);//step one
		try {
			mSensorHubNativeInterface.SetAGM_STEP_A();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		mCalibrateStepCallack.DoStepCallback(0);

		Log.d(TAG, "start CalibrationStepOne sleep 2000");
	}

	void SetCallbackFun(CalibrateStepCallack cb) {
		this.mCalibrateStepCallack = cb;
	}

}
