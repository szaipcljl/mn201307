package com.intel.sensorhub;

import android.util.Log;

public class CalibrationStepFour  implements Runnable {
	private static final String TAG = "CalibrationStepFour";
	
	private CalibrateStepCallack mCalibrateStepCallack;

	@Override
	public void run() {
		// TODO Auto-generated method stub
		Log.d(TAG, "start CalibrationStepFour");
		SensorHubNativeInterface mSensorHubNativeInterface = new SensorHubNativeInterface();

		// Thread.sleep(2000);//step one
		try {
			mSensorHubNativeInterface.SetAGM_STEP_D();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		mCalibrateStepCallack.DoStepCallback(0);
		
		Log.d(TAG, "start CalibrationStepFour sleep 2000");
	}
	
	void SetCallbackFun(CalibrateStepCallack cb)
	{
		this.mCalibrateStepCallack= cb;
	}

}
