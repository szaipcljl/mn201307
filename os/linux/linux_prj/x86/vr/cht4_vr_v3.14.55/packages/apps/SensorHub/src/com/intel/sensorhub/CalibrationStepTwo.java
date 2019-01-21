package com.intel.sensorhub;

import android.util.Log;
import com.intel.sensorhub.CalibrateStepCallack;

public class CalibrationStepTwo implements Runnable {
	private static final String TAG = "CalibrationStepTwo";

	private CalibrateStepCallack mCalibrateStepCallack;

	@Override
	public void run() {
		// TODO Auto-generated method stub
		Log.d(TAG, "start CalibrationStepTwo");
		SensorHubNativeInterface mSensorHubNativeInterface = new SensorHubNativeInterface();

		// Thread.sleep(2000);//step one
		try {
			mSensorHubNativeInterface.SetAGM_STEP_B();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		mCalibrateStepCallack.DoStepCallback(0);

		Log.d(TAG, "start CalibrationStepTwo sleep 2000");
	}

	void SetCallbackFun(CalibrateStepCallack cb) {
		this.mCalibrateStepCallack = cb;
	}

}
