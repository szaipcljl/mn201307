package com.intel.sensorhub;

import android.util.Log;

public class CalibrationStepFive  implements Runnable {
	private static final String TAG = "CalibrationStepFive";
	
	private CalibrateStepCallack mCalibrateStepCallack;

	@Override
	public void run() {
		// TODO Auto-generated method stub
		Log.d(TAG, "start CalibrationStepFive");
		SensorHubNativeInterface mSensorHubNativeInterface = new SensorHubNativeInterface();
		int step_result = -1;
		// Thread.sleep(2000);//step one
		try {
			step_result = mSensorHubNativeInterface.SetAGM_STEP_E();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (step_result==0)
		{
			step_result = mSensorHubNativeInterface.SetAGM_STEP_F();
		}
		mCalibrateStepCallack.DoStepCallback(step_result);
		
		Log.d(TAG, "CalibrationStepFive end");
	}
	
	void SetCallbackFun(CalibrateStepCallack cb)
	{
		this.mCalibrateStepCallack= cb;
	}

}
