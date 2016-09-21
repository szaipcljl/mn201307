package com.intel.sensorhub;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import com.intel.sensorhub.CalibrateStepCallack;

public class SensorHubMain extends Activity {
	private static final String TAG = "SensorHubMain";

	private static final int CALIBRATE_STEP_START = 0;
	private static final int CALIBRATE_STEP_1 = 1;
	private static final int CALIBRATE_STEP_2 = 2;
	private static final int CALIBRATE_STEP_3 = 3;
	private static final int CALIBRATE_STEP_4 = 4;
	private static final int CALIBRATE_STEP_5 = 5;
	private static final int CALIBRATE_STEP_END = 6;
	private static final int CALIBRATE_STEP_ERROR = 10;

	private static final String MSG_RESULT_KEY = "Result";

	private Button mExitButton;
	private Button mOutputCfgButton;
	private Button mStepButton;
	private TextView DescribeTextShow;
	private TextView CompassDataText;
	private TextView AccDataText;
	private TextView GyroDataText;
	private TextView ProgressText;
	private TextView ResultTextShow;

	private StringBuilder DescribeString;
	private StringBuilder CompassDataString;
	private StringBuilder AccDataString;
	private StringBuilder GyroDataString;
	private StringBuilder ProgressTextString;
	private StringBuilder ResultString;

	private SensorManager mSensorManager;
	private Sensor mAccSensor;
	private Sensor mCompassSensor;
	private Sensor mGyroSensor;
	
	DataFileSave mDataFileSave;

	private int iNextCalibrateStep = CALIBRATE_STEP_1;
	private int iStepErrorCount;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.sensor_hub_main);

		iNextCalibrateStep = CALIBRATE_STEP_1;

		DescribeTextShow = (TextView) findViewById(R.id.describe_text);
		DescribeString = new StringBuilder();
		DescribeString.append(getString(R.string.calibrate_step_1));
		DescribeString.append(getString(R.string.calibrate_start_info));
		DescribeTextShow.setText(DescribeString);
		
		ProgressText = (TextView) findViewById(R.id.process_text);
		

		CompassDataText = (TextView) findViewById(R.id.compass_data);
		CompassDataText.setText(getString(R.string.sensor_no_data));

		AccDataText = (TextView) findViewById(R.id.acc_data);
		AccDataText.setText(getString(R.string.sensor_no_data));

		GyroDataText = (TextView) findViewById(R.id.gyro_data);
		GyroDataText.setText(getString(R.string.sensor_no_data));

		// ProgressText = (TextView) findViewById(R.id.calibrate_progress);
		// CalibrateProgressString = new StringBuilder(
		// "Calibrate is in step one,Calibrating...");
		// ProgressText.setText(CalibrateProgressString);

		ResultTextShow = (TextView) findViewById(R.id.result_text);
		ResultTextShow.setText(getString(R.string.Calibrate_result_testing));

		mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		mAccSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		mCompassSensor = mSensorManager
				.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
		mGyroSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

		iStepErrorCount = 0;
		SetButtonListener();
		
		mDataFileSave = new DataFileSave(this);

	}

	protected void SetButtonListener() {
		mExitButton = (Button) findViewById(R.id.exit_button);
		mExitButton.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				finish();
			}
		});
		attachOnTouchListener(mExitButton);

		mOutputCfgButton = (Button) findViewById(R.id.output_cfg_button);
		mOutputCfgButton.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
			}
		});
		attachOnTouchListener(mOutputCfgButton);

		mStepButton = (Button) findViewById(R.id.step_button);
		mStepButton.setText(R.string.button_step_start);
		mStepButton.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if (v.getId() != mStepButton.getId()) {
					return;
				}

				// Message step_msg = new Message();
				Message step_msg = mStepHandler.obtainMessage();

				switch (iNextCalibrateStep) {
				case CALIBRATE_STEP_START:
					mStepButton.setText(R.string.button_step_next);
					step_msg.what = CALIBRATE_STEP_START;
					mStepHandler.dispatchMessage(step_msg);
					break;
				case CALIBRATE_STEP_1:
					mStepButton.setText(R.string.button_step_calibrating);
					
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step1));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrating));
					ProgressText.setText(ProgressTextString);
					
					CalibrationStepOne mCalibrationStepOne = new CalibrationStepOne();
					StepOneCallback mStepOneCallback = new StepOneCallback();
					mCalibrationStepOne.SetCallbackFun(mStepOneCallback);
					new Thread(mCalibrationStepOne).start();
					mStepButton.setClickable(false);

					break;
				case CALIBRATE_STEP_2:
					mStepButton.setText(R.string.button_step_calibrating);
					
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step2));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrating));
					ProgressText.setText(ProgressTextString);
					
					CalibrationStepTwo mCalibrationStepTwo = new CalibrationStepTwo();
					StepTwoCallback mStepTwoCallback = new StepTwoCallback();
					mCalibrationStepTwo.SetCallbackFun(mStepTwoCallback);
					new Thread(mCalibrationStepTwo).start();
					mStepButton.setClickable(false);

					break;
				case CALIBRATE_STEP_3:
					mStepButton.setText(R.string.button_step_calibrating);
					
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step3));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrating));
					ProgressText.setText(ProgressTextString);
					
					CalibrationStepThree mCalibrationStepThree = new CalibrationStepThree();
					StepThreeCallback mStepThreeCallback = new StepThreeCallback();
					mCalibrationStepThree.SetCallbackFun(mStepThreeCallback);
					new Thread(mCalibrationStepThree).start();
					mStepButton.setClickable(false);
					break;
				case CALIBRATE_STEP_4:
					mStepButton.setText(R.string.button_step_calibrating);
					
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step4));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrating));
					ProgressText.setText(ProgressTextString);
					
					CalibrationStepFour mCalibrationStepFour = new CalibrationStepFour();
					StepFourCallback mStepFourCallback = new StepFourCallback();
					mCalibrationStepFour.SetCallbackFun(mStepFourCallback);
					new Thread(mCalibrationStepFour).start();
					mStepButton.setClickable(false);
					break;
				case CALIBRATE_STEP_5:
					mStepButton.setText(R.string.button_step_calibrating);
					
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step5));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrating));
					ProgressText.setText(ProgressTextString);
					
					CalibrationStepFive mCalibrationStepFive = new CalibrationStepFive();
					StepFiveCallback mStepFiveCallback = new StepFiveCallback();
					mCalibrationStepFive.SetCallbackFun(mStepFiveCallback);
					new Thread(mCalibrationStepFive).start();
					mStepButton.setClickable(false);
					break;
				case CALIBRATE_STEP_END:
					break;
				default:
					break;
				}
			}
		});

		attachOnTouchListener(mStepButton);

	}

	@SuppressLint("ClickableViewAccessibility") protected void attachOnTouchListener(Button btn) {
		if (btn == null)
			return;
		btn.setOnTouchListener(new View.OnTouchListener() {

			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				if (event.getAction() == MotionEvent.ACTION_DOWN) {
					v.setBackgroundColor(Color.argb(255, 0xCC, 0xCC, 0xCC));
				} else if (event.getAction() == MotionEvent.ACTION_UP) {
					v.setBackgroundColor(Color.argb(255, 0xAA, 0xAA, 0xAA));
				}
				return false;
			}
		});
	}

	private SensorEventListener mCompassEventListener = new SensorEventListener() {

		@Override
		public void onSensorChanged(SensorEvent event) {

			CompassDataString = new StringBuilder();
			CompassDataString.append("Compass\n");
			CompassDataString.append("X:" + event.values[0] + "\n");
			CompassDataString.append("Y:" + event.values[1] + "\n");
			CompassDataString.append("Z:" + event.values[2]);
			CompassDataText.setText(CompassDataString);
		}

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			// TODO Auto-generated method stub
		}
	};

	private SensorEventListener mGyroEventListener = new SensorEventListener() {

		@Override
		public void onSensorChanged(SensorEvent event) {

			GyroDataString = new StringBuilder();
			GyroDataString.append("Gyro\n");
			GyroDataString.append("X:" + event.values[0] + "\n");
			GyroDataString.append("Y:" + event.values[1] + "\n");
			GyroDataString.append("Z:" + event.values[2]);
			GyroDataText.setText(GyroDataString);
		}

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			// TODO Auto-generated method stub
		}
	};

	private SensorEventListener mAccEventListener = new SensorEventListener() {

		@Override
		public void onSensorChanged(SensorEvent event) {

			AccDataString = new StringBuilder();
			AccDataString.append("Acc\n");
			AccDataString.append("X:" + event.values[0] + "\n");
			AccDataString.append("Y:" + event.values[1] + "\n");
			AccDataString.append("Z:" + event.values[2]);
			AccDataText.setText(AccDataString);
		}

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			// TODO Auto-generated method stub
		}
	};

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		mSensorManager.registerListener(mAccEventListener, mAccSensor,
				SensorManager.SENSOR_DELAY_UI);
		mSensorManager.registerListener(mCompassEventListener, mCompassSensor,
				SensorManager.SENSOR_DELAY_UI);
		mSensorManager.registerListener(mGyroEventListener, mGyroSensor,
				SensorManager.SENSOR_DELAY_UI);

	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		mSensorManager.unregisterListener(mAccEventListener);
		mSensorManager.unregisterListener(mCompassEventListener);
		mSensorManager.unregisterListener(mGyroEventListener);
	}

	@Override
	public void finish() {
		super.finish();
	}

	@SuppressLint("HandlerLeak")  final Handler mStepHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			int iResult;
			Log.d(TAG, "mStepHandler msg.what:" + msg.what);
			mStepButton.setClickable(true);
			switch (msg.what) {
			case CALIBRATE_STEP_START:
				mStepButton.setText(R.string.button_step_next);
				DescribeString = new StringBuilder();
				DescribeString.append(getString(R.string.calibrate_step_1));
				DescribeString.append(getString(R.string.calibrate_next_info));
				DescribeTextShow.setText(DescribeString);
				iNextCalibrateStep = CALIBRATE_STEP_1;
				break;
			case CALIBRATE_STEP_1:
				iResult = msg.getData().getInt(MSG_RESULT_KEY);
				mStepButton.setText(R.string.button_step_next);
				Log.d(TAG, "mStepHandler msg.getData:" + iResult);
				DescribeString = new StringBuilder();
				if (iResult == 0) {
					iStepErrorCount = 0;
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step1));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_pass));
					ProgressText.setText(ProgressTextString);
					
					DescribeString.append(getString(R.string.calibrate_step_2));
					DescribeString
							.append(getString(R.string.calibrate_next_info));
					DescribeTextShow.setText(DescribeString);
					iNextCalibrateStep = CALIBRATE_STEP_2;
				} else {
					iStepErrorCount++;
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step1));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_fail));
					ProgressText.setText(ProgressTextString);
					if (iStepErrorCount < 5) {
						DescribeString
								.append(getString(R.string.calibrate_step_1));
						DescribeString
								.append(getString(R.string.calibrate_next_info));
						DescribeTextShow.setText(DescribeString);
						iNextCalibrateStep = CALIBRATE_STEP_1;
					} else {
						SendCalibrateErrorMsg();
					}
				}
				break;
			case CALIBRATE_STEP_2:
				iStepErrorCount = 0;
				ProgressTextString = new StringBuilder();
				ProgressTextString.append(getString(R.string.calibrate_precess_step2));
				ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_pass));
				ProgressText.setText(ProgressTextString);
				
				iResult = msg.getData().getInt(MSG_RESULT_KEY);
				mStepButton.setText(R.string.button_step_next);
				Log.d(TAG, "mStepHandler msg.getData:" + iResult);
				DescribeString = new StringBuilder();
				if (iResult == 0) {
					DescribeString.append(getString(R.string.calibrate_step_3));
					DescribeString
							.append(getString(R.string.calibrate_next_info));
					DescribeTextShow.setText(DescribeString);
					iNextCalibrateStep = CALIBRATE_STEP_3;
				} else {
					iStepErrorCount++;
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step2));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_fail));
					ProgressText.setText(ProgressTextString);
					if (iStepErrorCount < 5) {
						DescribeString
								.append(getString(R.string.calibrate_step_2));
						DescribeString
								.append(getString(R.string.calibrate_next_info));
						DescribeTextShow.setText(DescribeString);
						iNextCalibrateStep = CALIBRATE_STEP_2;
					} else {
						SendCalibrateErrorMsg();
					}
				}
				break;
			case CALIBRATE_STEP_3:
				iStepErrorCount = 0;
				ProgressTextString = new StringBuilder();
				ProgressTextString.append(getString(R.string.calibrate_precess_step3));
				ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_pass));
				ProgressText.setText(ProgressTextString);
				
				iResult = msg.getData().getInt(MSG_RESULT_KEY);
				mStepButton.setText(R.string.button_step_next);
				Log.d(TAG, "mStepHandler msg.getData:" + iResult);
				DescribeString = new StringBuilder();
				if (iResult == 0) {
					DescribeString.append(getString(R.string.calibrate_step_4));
					DescribeString
							.append(getString(R.string.calibrate_next_info));
					DescribeTextShow.setText(DescribeString);
					iNextCalibrateStep = CALIBRATE_STEP_4;
				} else {
					iStepErrorCount++;
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step3));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_fail));
					ProgressText.setText(ProgressTextString);
					
					if (iStepErrorCount < 5) {
						DescribeString
								.append(getString(R.string.calibrate_step_3));
						DescribeString
								.append(getString(R.string.calibrate_next_info));
						DescribeTextShow.setText(DescribeString);
						iNextCalibrateStep = CALIBRATE_STEP_3;
					} else {
						SendCalibrateErrorMsg();
					}
				}
				break;
			case CALIBRATE_STEP_4:
				iStepErrorCount = 0;
				ProgressTextString = new StringBuilder();
				ProgressTextString.append(getString(R.string.calibrate_precess_step4));
				ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_pass));
				ProgressText.setText(ProgressTextString);
				
				iResult = msg.getData().getInt(MSG_RESULT_KEY);
				mStepButton.setText(R.string.button_step_next);
				Log.d(TAG, "mStepHandler msg.getData:" + iResult);
				DescribeString = new StringBuilder();
				if (iResult == 0) {
					DescribeString.append(getString(R.string.calibrate_step_5));
					DescribeString
							.append(getString(R.string.calibrate_next_info));
					DescribeTextShow.setText(DescribeString);
					iNextCalibrateStep = CALIBRATE_STEP_5;
				} else {
					iStepErrorCount++;
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step4));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_fail));
					ProgressText.setText(ProgressTextString);
					
					if (iStepErrorCount < 5) {
						DescribeString
								.append(getString(R.string.calibrate_step_4));
						DescribeString
								.append(getString(R.string.calibrate_next_info));
						DescribeTextShow.setText(DescribeString);
						iNextCalibrateStep = CALIBRATE_STEP_4;
					} else {
						SendCalibrateErrorMsg();
					}
				}
				break;
			case CALIBRATE_STEP_5:
				iStepErrorCount = 0;
				ProgressTextString = new StringBuilder();
				ProgressTextString.append(getString(R.string.calibrate_precess_step5));
				ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_pass));
				ProgressText.setText(ProgressTextString);
				
				iResult = msg.getData().getInt(MSG_RESULT_KEY);
				mStepButton.setText(R.string.button_step_end);
				Log.d(TAG, "mStepHandler msg.getData:" + iResult);
				DescribeString = new StringBuilder();
				if (iResult == 0) {
					DescribeString
							.append(getString(R.string.calibrate_step_end));
					DescribeTextShow.setText(DescribeString);
					iNextCalibrateStep = CALIBRATE_STEP_END;
					ResultTextShow.setBackgroundColor(Color.GREEN);
					ResultTextShow
							.setText(getString(R.string.Calibrate_result_Pass));
				} else {
					iStepErrorCount++;
					ProgressTextString = new StringBuilder();
					ProgressTextString.append(getString(R.string.calibrate_precess_step5));
					ProgressTextString.append(getString(R.string.calibrate_precess_calibrate_fail));
					ProgressText.setText(ProgressTextString);
					
					if (iStepErrorCount < 5) {
						DescribeString
								.append(getString(R.string.calibrate_step_5));
						DescribeString
								.append(getString(R.string.calibrate_next_info));
						DescribeTextShow.setText(DescribeString);
						iNextCalibrateStep = CALIBRATE_STEP_5;
					} else {
						SendCalibrateErrorMsg();
					}
				}
				break;
			case CALIBRATE_STEP_ERROR:
				iResult = msg.getData().getInt(MSG_RESULT_KEY);
				mStepButton.setText(R.string.button_step_end);
				Log.d(TAG, "mStepHandler msg.getData:" + iResult);
				DescribeString = new StringBuilder();
				DescribeString.append(getString(R.string.calibrate_step_end));
				DescribeTextShow.setText(DescribeString);
				iNextCalibrateStep = CALIBRATE_STEP_END;
				ResultTextShow.setBackgroundColor(Color.RED);
				ResultTextShow
						.setText(getString(R.string.Calibrate_result_Fail));

				iNextCalibrateStep = CALIBRATE_STEP_ERROR;

				break;
			default:
				break;
			}
		}
	};

	private void SendCalibrateErrorMsg() {
		Message step_msg = mStepHandler.obtainMessage();
		step_msg.what = CALIBRATE_STEP_ERROR;
		Bundle msgBundle = new Bundle();
		msgBundle.putInt(MSG_RESULT_KEY, 33);
		step_msg.setData(msgBundle);
		mStepHandler.sendMessage(step_msg);
	}

	public class StepOneCallback implements CalibrateStepCallack {

		@Override
		public int DoStepCallback(int data) {
			// TODO Auto-generated method stub

			Message step_msg = mStepHandler.obtainMessage();
			// mStepButton.setText(R.string.button_step_next);
			step_msg.what = CALIBRATE_STEP_1;
			Bundle msgBundle = new Bundle();
			msgBundle.putInt(MSG_RESULT_KEY, data);
			step_msg.setData(msgBundle);
			mStepHandler.sendMessage(step_msg);
			return 0;
		}

	}

	public class StepTwoCallback implements CalibrateStepCallack {

		@Override
		public int DoStepCallback(int data) {
			// TODO Auto-generated method stub
			Message step_msg = mStepHandler.obtainMessage();
			// mStepButton.setText(R.string.button_step_next);
			step_msg.what = CALIBRATE_STEP_2;
			Bundle msgBundle = new Bundle();
			msgBundle.putInt(MSG_RESULT_KEY, data);
			step_msg.setData(msgBundle);
			mStepHandler.sendMessage(step_msg);
			return 0;
		}

	}

	public class StepThreeCallback implements CalibrateStepCallack {

		@Override
		public int DoStepCallback(int data) {
			// TODO Auto-generated method stub
			Message step_msg = mStepHandler.obtainMessage();
			// mStepButton.setText(R.string.button_step_next);
			step_msg.what = CALIBRATE_STEP_3;
			Bundle msgBundle = new Bundle();
			msgBundle.putInt(MSG_RESULT_KEY, data);
			step_msg.setData(msgBundle);
			mStepHandler.sendMessage(step_msg);
			return 0;
		}

	}

	public class StepFourCallback implements CalibrateStepCallack {

		@Override
		public int DoStepCallback(int data) {
			// TODO Auto-generated method stub
			Message step_msg = mStepHandler.obtainMessage();
			// mStepButton.setText(R.string.button_step_next);
			step_msg.what = CALIBRATE_STEP_4;
			Bundle msgBundle = new Bundle();
			msgBundle.putInt(MSG_RESULT_KEY, data);
			step_msg.setData(msgBundle);
			mStepHandler.sendMessage(step_msg);
			return 0;
		}

	}

	public class StepFiveCallback implements CalibrateStepCallack {

		@Override
		public int DoStepCallback(int data) {
			// TODO Auto-generated method stub
			Message step_msg = mStepHandler.obtainMessage();
			// mStepButton.setText(R.string.button_step_next);
			step_msg.what = CALIBRATE_STEP_5;
			Bundle msgBundle = new Bundle();
			msgBundle.putInt(MSG_RESULT_KEY, data);
			step_msg.setData(msgBundle);
			mStepHandler.sendMessage(step_msg);
			return 0;
		}

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.sensor_hub_main, menu);
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
}
