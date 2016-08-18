package com.goodix.fpsetting;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Service;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.Vibrator;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import android.gxFP.IEnrollCallback;
import com.goodix.application.FpApplication;
import com.goodix.device.MessageType;
import android.gxFP.FingerprintManager.EnrollSession;
import com.goodix.util.AlgoResult;
import com.goodix.util.Fingerprint;
import com.goodix.util.L;
import com.goodix.util.Preferences;
import com.goodix.widget.FingerprintProgressBar;

public class RegisterActivity extends Activity {
	private static final long CANCEL_TIME_INTERVAL = 30000;
	private static final long RELEASE_TIME_INTERVAL = 100;
	private static final int FORECAST_PERCENT = 7;
	private static final String TAG = "RegisterActivity";
	private ViewGroup mRootGroup;
	private ImageView mPhoneImage;
	private TextView mTitleTxt;
	private TextView mTitleNoticeTxt;
	private TextView mSubInfoTxt;
	private TextView mSubInfoTxtOutside;
	private Button mContinueBtn;
	private Button mCompleteBtn;
	private FingerprintProgressBar mRegisterProgressBar;
	private static Handler mCancelHandler;
	private static Runnable mCancelRunable;
	private Handler mReleaseFingerHandler;
	private ReleaseTouchRunnable mReleaseFingerRunable;
	private int mPercent = 0;
	private EnrollSession mSession;
	private LinearLayout mAlgoLog;
	private TextView mTopView;
	private TextView mBehandView;
	private ImageView mImageOne;
	
	private AlertDialog mDialog;

	public static RegisterActivity instance;

	private ArrayList<Fingerprint> mDataList = null;

	/*
	 * private static TextView mLeftLogView; private static TextView
	 * mRightLogView;
	 */

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_register);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.title);
		instance = this;
		initView();
		initRegister();
		startCancelTimer();
		mDataList = getIntent().getParcelableArrayListExtra(TouchIDActivity.FRINGERPRINT_INDEX);
	}

	private int getFingerViewIndex(int index) {
		if (mDataList == null) {
			return index;
		}
		for (int i = 0; i < this.mDataList.size(); i++) {
			if (Integer.parseInt(this.mDataList.get(i).getUri()) == index) {
				return i + 1;
			}
		}
		return index;
	}

	@Override
	public void finish() {
		Log.v(TAG, "finish");
		super.finish();
	}

	@Override
	protected void onPause() {
		Log.v(TAG, "onPause");
		if(mSession!=null) {
			mSession.exit();
			cancelCancelTimer();
			finish();
		}
		super.onPause();
	}

	@Override
	protected void onRestart() {
		Log.v(TAG, "onRestart");
		super.onRestart();
	}

	@Override
	protected void onResume() {
		Log.v(TAG, "onResume");
		super.onResume();
	}

	@Override
	protected void onDestroy() {
		Log.v(TAG, "onDestroy");
		instance = null;
	/*	if (null != mSession) {
			mSession.exit();
		}*/
		cancelCancelTimer();
		super.onDestroy();
	}
	private void initRegister() {

		if (null == mSession) {
			mSession = FpApplication.getInstance().getFpServiceManager().newEnrollSession(mEnrollCallback);
		}
		mSession.enter();
	}
	private void initView() {
		TextView titleText = (TextView) findViewById(R.id.fp_description);
		titleText.setText(R.string.register_title);
		Button titleBackBtn = (Button) findViewById(R.id.title_back);
		titleBackBtn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finish();
			}
		});

		mRootGroup = (ViewGroup) findViewById(R.id.register_root_group);
		mRootGroup.setOnClickListener(new RootGroupClickListener());
		mPhoneImage = (ImageView) findViewById(R.id.register_phone);
		mTitleTxt = (TextView) findViewById(R.id.title_text);
		mTitleNoticeTxt = (TextView) findViewById(R.id.title_notice_text);
		mRegisterProgressBar = (FingerprintProgressBar) findViewById(R.id.register_progress);

		mContinueBtn = (Button) findViewById(R.id.register_continue);
		mContinueBtn.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				view.setVisibility(View.INVISIBLE);
				mSubInfoTxt.setText(R.string.capture_notice_put_on_screen);
				cancelWarning();
			}
		});

		mCompleteBtn = (Button) findViewById(R.id.register_completed);
		mCompleteBtn.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				finish();
			}
		});

		mSubInfoTxt = (TextView) findViewById(R.id.register_sub_info);

		mSubInfoTxt.setText(getStyle(String.format(getString(R.string.capture_notice_put_on_screen),getString(R.string.center_area)),getString(R.string.center_area)));

		mSubInfoTxtOutside = (TextView) findViewById(R.id.register_sub_info_outside);

		mAlgoLog = (LinearLayout) findViewById(R.id.register_info);
		if (Preferences.getEnableEM() == true) {
			mAlgoLog.setVisibility(View.VISIBLE);
		} else {
			mAlgoLog.setVisibility(View.GONE);
		}

		mTopView = (TextView) findViewById(R.id.top_textview);
		mBehandView = (TextView) findViewById(R.id.behand_textview);

		mImageOne = (ImageView) findViewById(R.id.image_one);

		mCancelHandler = new Handler();
		mCancelRunable = new CancelRunnable(this);

		mReleaseFingerHandler = new Handler();
		mReleaseFingerRunable = new ReleaseTouchRunnable(this);
	}

	private void showTextTranslateAnim(View v, int animID, int visible) {
		Animation animation = AnimationUtils.loadAnimation(this, animID);
		animation.setAnimationListener(new TitleExitAnimListener(v, visible));
		v.startAnimation(animation);
	}

	private void startCancelTimer() {
		if (null != mCancelHandler && null != mCancelRunable) {
			mCancelHandler.postDelayed(mCancelRunable, CANCEL_TIME_INTERVAL);
		}
	}

	private void cancelCancelTimer() {
		Log.v(TAG, "cancelCancelTimer");
		if (null != mCancelHandler && null != mCancelRunable) {
			mCancelHandler.removeCallbacks(mCancelRunable);
		}
	}

	private void resetCancelTimer() {
		Log.v(TAG, "resetCancelTimer");
		if (null != mCancelHandler && null != mCancelRunable) {
			mCancelHandler.removeCallbacks(mCancelRunable);
			mCancelHandler.postDelayed(mCancelRunable, CANCEL_TIME_INTERVAL);
		}
	}

	private void startReleaseFingerTimer() {
		Log.v(TAG, "startReleaseFingerTimer");
		if (null != mReleaseFingerHandler && null != mReleaseFingerRunable) {
			mReleaseFingerHandler.postDelayed(mReleaseFingerRunable, RELEASE_TIME_INTERVAL);
		}
	}

	private void cancelReleaseFingerTimer() {
		Log.v(TAG, "cancelReleaseFingerTimer");
		if (null != mReleaseFingerHandler && null != mReleaseFingerRunable) {
			if (true == mReleaseFingerRunable.bWarning) {
				Log.v(TAG, "Cancel Warning!");
				mReleaseFingerRunable.bWarning = false;
				cancelWarning();
			}
			mReleaseFingerHandler.removeCallbacks(mReleaseFingerRunable);
		}

	}

	private void startWarning(int textID) {
		mTitleNoticeTxt.setVisibility(View.VISIBLE);
		mTitleNoticeTxt.setText(textID);
		showTextTranslateAnim(mTitleNoticeTxt, R.anim.register_title_text_enter, View.VISIBLE);
		showTextTranslateAnim(mTitleTxt, R.anim.register_title_text_exit, View.INVISIBLE);
	}

	private SpannableStringBuilder getStyle(String text,String keyTex) {
		
		int length  = 0;
		int index = 0;
		if (TextUtils.isEmpty(text) || TextUtils.isEmpty(keyTex)) {
			return null;
		}
		index = text.indexOf(keyTex);
		length = keyTex.length();
		SpannableStringBuilder style = new SpannableStringBuilder(text);
		style.setSpan(new ForegroundColorSpan(Color.RED), index, index + length, Spannable.SPAN_EXCLUSIVE_INCLUSIVE); 
		return style;

	}

	private void startSubWarning(String text,String keyText) {

		mSubInfoTxtOutside.setVisibility(View.VISIBLE);
		mSubInfoTxtOutside.setText(getStyle(text,keyText));
		showTextTranslateAnim(mSubInfoTxtOutside, R.anim.register_title_text_enter, View.VISIBLE);
		showTextTranslateAnim(mSubInfoTxt, R.anim.register_title_text_exit, View.INVISIBLE);

	}

	private void cancelWarning() {
		showTextTranslateAnim(mTitleNoticeTxt, R.anim.register_title_text_exit, View.INVISIBLE);
		showTextTranslateAnim(mTitleTxt, R.anim.register_title_text_enter, View.VISIBLE);
	}

	private void CaptureResult(String uri) {
		Intent intent = new Intent(RegisterActivity.this, TouchIDActivity.class);
		intent.putExtra(TouchIDActivity.FRINGERPRINT_URI, uri);
		setResult(RESULT_OK, intent);
		// finish();
	}

	private class CancelRunnable implements Runnable {
		private WeakReference<RegisterActivity> mActivityReference;

		public CancelRunnable(RegisterActivity activity) {
			mActivityReference = new WeakReference<RegisterActivity>(activity);
		}

		@Override
		public void run() {
			RegisterActivity activity = (RegisterActivity) mActivityReference.get();
			if (null != activity) {
				
				try {
					if (null != activity.mSession) {
						activity.mSession.exit();
					}
					startWarning(R.string.register_register_failed);
				} catch (Exception e) {
					e.printStackTrace();
				}

			}
		}
	}

	private class ReleaseTouchRunnable implements Runnable {
		private WeakReference<RegisterActivity> mActivityReference;
		public boolean bWarning = false;

		public ReleaseTouchRunnable(RegisterActivity activity) {
			mActivityReference = new WeakReference<RegisterActivity>(activity);
		}

		@Override
		public void run() {
			Log.v(TAG, "ReleaseTouchRunnable:Run...");
			RegisterActivity activity = (RegisterActivity) mActivityReference.get();
			if (null != activity) {
				bWarning = true;
				startWarning(R.string.register_notice_the_hand);
			}
		}
	}

	private class TitleExitAnimListener implements AnimationListener {
		int visible;
		private WeakReference<View> mViewReference;

		public TitleExitAnimListener(View v, int visible) {
			this.visible = visible;
			mViewReference = new WeakReference<View>(v);
		}

		@Override
		public void onAnimationEnd(Animation arg0) {
			TextView textView = (TextView) mViewReference.get();
			if (null != textView) {
				textView.setVisibility(visible);
			}
		}

		@Override
		public void onAnimationRepeat(Animation arg0) {
		}

		@Override
		public void onAnimationStart(Animation arg0) {
		}
	}

	private class RootGroupClickListener implements View.OnClickListener {
		@Override
		public void onClick(View arg0) {
			// Show notice dialog;
			OnClickListener listener = new OnClickListener() {
				public void onClick(DialogInterface dialog, int arg1) {
					dialog.dismiss();
				}
			};
			showOneButtonDialog(false, R.string.register_touchhomekey, R.string.register_notice_content_one, R.string.register_dialog_key_good, listener);
		}
	}

	private void showOneButtonDialog(boolean bCancel, int titleID, int messageID, int keyID, OnClickListener listener) {
		AlertDialog.Builder builder = new Builder(this);
		builder.setCancelable(bCancel);
		builder.setMessage(messageID);
		builder.setTitle(titleID);
		builder.setPositiveButton(keyID, listener);
		builder.create().show();
	}

	private IEnrollCallback mEnrollCallback = new IEnrollCallback.Stub() {
		@Override
		public void handleMessage(int msg, int arg0, int arg1, byte[] data) throws RemoteException {
			Log.v(TAG, String.format("msg = %d , arg0 = %d ,arg1 = %d", msg, arg0, arg1));
			mHandler.sendMessage(mHandler.obtainMessage(msg, arg1, arg0, data));
	//		return false;
		}
	};

	private  void showNoteDialog(Context context, boolean bCancel, int titleID, int messageID, int keyID, OnClickListener listener) {
		if (mDialog != null && mDialog.isShowing()) {
			return;
		}
		AlertDialog.Builder builder = new Builder(context);
		builder.setCancelable(bCancel);
		builder.setMessage(messageID);
		builder.setTitle(titleID);
		//builder.setPositiveButton(keyID, listener);
		mDialog = builder.create();
		mDialog.show();

	}
	private static void showNoteDialog(Context context, boolean bCancel, int titleID, String messageID, int keyID, OnClickListener listener) {
		AlertDialog.Builder builder = new Builder(context);
		builder.setCancelable(bCancel);
		builder.setMessage(messageID);
		builder.setTitle(titleID);
		builder.setPositiveButton(keyID, listener);
		builder.create().show();

	}

	private RegisterHandler mHandler = new RegisterHandler(this);

	private static class RegisterHandler extends Handler {
		private final WeakReference<RegisterActivity> mActivityRef;
		private final static int MAX_ACTION_ERROR = 4;
		private int mBadImageCount = 0;
		private int mNoPieceTime = 0;
		private int mNoMoveTime = 0;

		private boolean bHasShowBadDialog = false;
		private boolean bHasShowNoPieceDialog = false;
		private boolean bHasShowMoveDialog = false;

		private boolean bHasShowAni = false;
		
		private boolean bToutch = false;

		private OnClickListener mListener = new OnClickListener() {
			public void onClick(DialogInterface dialog, int arg1) {
				dialog.dismiss();
			}
		};

		public RegisterHandler(RegisterActivity activity) {
			mActivityRef = new WeakReference<RegisterActivity>(activity);
		}

		@Override
		public void handleMessage(Message msg) {
			if (mActivityRef.get() == null)
				return;
			final RegisterActivity activity = (RegisterActivity) mActivityRef.get();
			if (null == activity) {
				return;
			}

			switch (msg.what) {
				case MessageType.MSG_TYPE_COMMON_NOTIFY_INFO :
					Object obj = msg.obj;
					if (obj != null) {
						byte[] loginfo = (byte[]) obj;
						String str = new String(loginfo);
						if (!AlgoResult.isMatchInfo(str)) {
							if (AlgoResult.isFilePath(str)) {
								activity.mTopView.setTextSize(13);
								activity.mTopView.setText(AlgoResult.bulidLog(activity, str, AlgoResult.FILTER_REGISTER, 0));
							}
							activity.mBehandView.setText(AlgoResult.bulidLog(activity, str, AlgoResult.FILTER_REGISTER, 0));
							activity.mBehandView.setTextSize(15);
						}
						int index = str.indexOf("=", str.indexOf("=") + 1);

						if (-1 != index) {
							String fileName = null;
							if (index != -1) {
								fileName = str.substring(index + 1, str.length() - 1);
								File file = new File(fileName);
								try {
									InputStream in = new FileInputStream(file);
									Bitmap map = BitmapFactory.decodeStream(in);

									activity.mImageOne.setImageBitmap(map);

									in.close();
								} catch (Exception e) {
									e.printStackTrace();
								}
							}
						}
					}
					break;
				case MessageType.MSG_TYPE_REGISTER_DUPLICATE_REG :
					L.d("index == " + activity.getFingerViewIndex(msg.arg1));
					showNoteDialog(activity, false, R.string.register_notice, String.format(activity.getResources().getString(R.string.register_duplicate), activity.getFingerViewIndex(msg.arg1)),
							R.string.register_dialog_key_good, new OnClickListener() {

								@Override
								public void onClick(DialogInterface dialog, int which) {
									// TODO Auto-generated method stub
									dialog.dismiss();
									activity.finish();
									activity.overridePendingTransition(0, 0);
								}
							});

					break;
				case MessageType.MSG_TYPE_REGISTER_PIECE :
				case MessageType.MSG_TYPE_REGISTER_NO_PIECE :
				case MessageType.MSG_TYPE_REGISTER_NO_EXTRAINFO :
				case MessageType.MSG_TYPE_REGISTER_LOW_COVER :
				case MessageType.MSG_TYPE_REGISTER_GET_DATA_FAILED :
				case MessageType.MSG_TYPE_REGISTER_BAD_IMAGE :
					Log.v(TAG, "RegisterHandler: Result");

					Vibrator vib = (Vibrator) activity.getSystemService(Service.VIBRATOR_SERVICE);
					vib.vibrate(100);
					if (msg.what == MessageType.MSG_TYPE_REGISTER_PIECE || msg.what == MessageType.MSG_TYPE_REGISTER_NO_PIECE) {
						activity.mPercent = msg.arg2;
					}
					if (msg.what == MessageType.MSG_TYPE_REGISTER_BAD_IMAGE || msg.what == MessageType.MSG_TYPE_REGISTER_GET_DATA_FAILED) {
						mBadImageCount++;
						//if (mBadImageCount >= MAX_ACTION_ERROR && bHasShowBadDialog == false) {
						  if (bToutch == true) {
								activity.showNoteDialog(activity, true, R.string.register_notice, R.string.register_bad_images, R.string.register_dialog_key_good, mListener);
								bHasShowBadDialog = true;
								mBadImageCount = 0;
						  } else {
								activity.showNoteDialog(activity, true, R.string.register_notice_steady, R.string.register_notiece_steady_content, R.string.register_dialog_key_good, mListener);

						  }
	
						//}
					} else if (msg.what != MessageType.MSG_TYPE_REGISTER_BAD_IMAGE && msg.what != MessageType.MSG_TYPE_REGISTER_GET_DATA_FAILED && msg.what != MessageType.MSG_TYPE_COMMON_TOUCH
							&& msg.what != MessageType.MSG_TYPE_COMMON_UNTOUCH) {
						mBadImageCount = 0;
					}

					if (msg.what == MessageType.MSG_TYPE_REGISTER_NO_EXTRAINFO) {
						mNoMoveTime++;
						//if (mNoMoveTime >= MAX_ACTION_ERROR && bHasShowMoveDialog == false) {
							activity.showNoteDialog(activity, true, R.string.register_notice, R.string.register_no_move, R.string.register_dialog_key_good, mListener);
							bHasShowMoveDialog = true;
							mNoMoveTime = 0;
						//}
					} else if (msg.what != MessageType.MSG_TYPE_REGISTER_NO_EXTRAINFO && msg.what != MessageType.MSG_TYPE_COMMON_TOUCH && msg.what != MessageType.MSG_TYPE_COMMON_UNTOUCH) {
						mNoMoveTime = 0;
					}

					if (msg.what == MessageType.MSG_TYPE_REGISTER_NO_PIECE) {
						mNoPieceTime++;

						if (mNoPieceTime >= MAX_ACTION_ERROR && bHasShowNoPieceDialog == false) {
							activity.showNoteDialog(activity, true, R.string.register_notice, R.string.register_no_piece_together, R.string.register_dialog_key_good, mListener);
							bHasShowNoPieceDialog = true;
							mNoPieceTime = 0;
						}
					} else if (msg.what != MessageType.MSG_TYPE_REGISTER_NO_PIECE && msg.what != MessageType.MSG_TYPE_COMMON_TOUCH && msg.what != MessageType.MSG_TYPE_COMMON_UNTOUCH) {
						mNoPieceTime = 0;
					}

					int index = msg.arg1;
					activity.mRegisterProgressBar.setProgress(activity.mPercent);

					Log.v(TAG, "RegisterHandler: mPercent" + activity.mPercent);
					if (activity.mPercent >= 100) {
						try {
							activity.mSession.save(index);
						} catch (Exception e) {
							e.printStackTrace();
						}
						activity.CaptureResult(Integer.toString(index));
						activity.cancelReleaseFingerTimer();
						activity.cancelCancelTimer();
						activity.startWarning(R.string.register_register_complete);
						activity.mSubInfoTxt.setText(R.string.register_complete_infomation);
						activity.mRootGroup.setOnClickListener(null);
						activity.mCompleteBtn.setVisibility(View.VISIBLE);
					} else {
						if (activity.mPercent >= 70 && bHasShowAni == false) {
							bHasShowAni = true;
							activity.startSubWarning(
									String.format(activity.getResources().getString(R.string.capture_notice_put_on_screen_outside),
											activity.getResources().getString(R.string.outside_area)),activity.getResources().getString(R.string.outside_area));
							// activity.mSubInfoTxt.setText(R.string.capture_notice_put_on_screen_outside);
						}
						if (bToutch == true) {
						    activity.startReleaseFingerTimer();
						}
					}
					break;
				case MessageType.MSG_TYPE_COMMON_TOUCH :
					Log.v(TAG, "RegisterHandler:MSG_TYPE_COMMON_TOUCH");
					bToutch = true;
					if (activity.mDialog != null && activity.mDialog.isShowing()) {
						activity.mDialog.dismiss();
					}
					activity.mRegisterProgressBar.setProgress(FORECAST_PERCENT + activity.mPercent);
					// activity.cancelCancelTimer();
					activity.resetCancelTimer();

					// Dissolve phone image.
					if (activity.mPhoneImage.getVisibility() == View.VISIBLE) {
						Animation animation = AnimationUtils.loadAnimation(activity, R.anim.register_phone_image_exit);
						animation.setAnimationListener(new PhoneImageExitAnimListener(activity.mPhoneImage));
						activity.mPhoneImage.startAnimation(animation);
						activity.mRegisterProgressBar.setSlideVisible(true);
					}
					break;
				case MessageType.MSG_TYPE_COMMON_UNTOUCH :
					bToutch = false;
					Log.v(TAG, "RegisterHandler:MSG_TYPE_COMMON_UNTOUCH");
					activity.cancelReleaseFingerTimer();
					break;

				default :
					break;
			}
		}

		public class PhoneImageExitAnimListener implements AnimationListener {
			private WeakReference<View> mViewReference;

			public PhoneImageExitAnimListener(View v) {
				mViewReference = new WeakReference<View>(v);
			}

			@Override
			public void onAnimationEnd(Animation arg0) {
				View widget = (View) mViewReference.get();
				if (null != widget) {
					widget.setVisibility(View.INVISIBLE);
				}
			}

			@Override
			public void onAnimationRepeat(Animation arg0) {
			}

			@Override
			public void onAnimationStart(Animation arg0) {
			}
		}
	}
}
