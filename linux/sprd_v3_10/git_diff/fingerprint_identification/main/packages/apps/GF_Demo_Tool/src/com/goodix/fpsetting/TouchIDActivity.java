/************************************************************************
 * <p>Title: TouchIDActivity.java</p>
 * <p>Description: </p>
 * <p>Copyright (C), 1997-2014, Shenzhen Goodix Technology Co.,Ltd.</p>
 * <p>Company: Goodix</p>
 * @author  peng.hu
 * @date    2014-9-23
 * @version  1.0
 ************************************************************************/
package com.goodix.fpsetting;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.text.format.Time;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.ScaleAnimation;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.gxFP.IVerifyCallback;
import com.goodix.application.FpApplication;
import com.goodix.device.MessageType;
import com.goodix.service.FingerprintHandleService;
import com.goodix.service.FingerprintHandleService.ServiceBinder;
import android.gxFP.FingerprintManager.VerifySession;
import com.goodix.util.AlgoResult;
import com.goodix.util.Fingerprint;
import com.goodix.util.L;
import com.goodix.util.Preferences;

/**
 * <p>
 * Title: TouchIDActivity
 * </p>
 * <p>
 * Description:
 * </p>
 */

public class TouchIDActivity extends Activity {
	
	public static final String TAG = "TouchIDActivity";
	public static final String FRINGERPRINT_URI = "fp_uri";
	public static final String FRINGERPRINT_INDEX = "fp_uri_index";
	/* remote service connected */
	private static final int MSG_SERVICE_CONNECTED = 1;
	/* fingerprint data is ready */
	private static final int MSG_DATA_IS_READY = 2;
	/* verify success */
	private static final int MSG_VERIFY_SUCCESS = 3;
	/* verify failed */
	private static final int MSG_VERIFY_FAILED = 4;

	private static final int MSG_DEBUG_INFO = 5;
	/* fingerprint manager */
	//private FingerprintManager mFpManagerService;
	/* root container of fingerprint items */
	private ViewGroup mContainer = null;
	/* Database service */
	private FingerprintHandleService mFingerPrintHandleService;
	/* is in edit model */
	private boolean bEditFingerprint = false;
	/* flag that fingerprint item been edited. */
	private int mEditorIndex = -1;
	/* Message handler */
	private MyHandler mHandler;
	/* The list of fingerprint */
	private ArrayList<Fingerprint> mDataList = null;

	private LayoutInflater mInflater;

	private HolderEditor mEditorHolder = null;

	//private FpManagerServiceConnection mServiceConn;

	private FpHandleServiceConnection mHandleServiceConn;
	/* fingerprint mananger service session */
	private VerifySession mSession = null;
	/* Control that showing description or not */
	private CheckBox mShowDescription = null;
	/* Edit fingerprint information */
	private ViewGroup mEditorPanel = null;
	/* Add new fingerprint , start register activity */
	private Button mInsertButton = null;
	/* All fingerprint item views */
	
	private CheckBox mShowMessage = null;
	
	private ArrayList<View> mListViews;

	public static TouchIDActivity instance = null;


	private LinearLayout mAlgoLog;

	private TextView mTopView;

	private TextView mBehandView;

	private ImageView mImageOne;

	private ToggleButton mSwitchLockScreen;

	private ViewGroup mInsertPanel = null;

	private ViewGroup mTouchIDListPanel = null;
	
	private boolean mIsEnableShow = false;
	
	/*
	 * private static TextView mLeftLogView; private static TextView
	 * mRightLogView;
	 */

	
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_touchid);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.title);
		instance = this;

		mHandler = new MyHandler(this);
		updateView();
		mListViews = new ArrayList<View>();
		Intent intent = new Intent(TouchIDActivity.this,FingerprintHandleService.class);
		mHandleServiceConn = new FpHandleServiceConnection();
		getApplicationContext().bindService(intent, mHandleServiceConn,Context.BIND_AUTO_CREATE);
		
		this.mInflater = LayoutInflater.from(this);
	}

	private void updateView() {
		TextView titleText = (TextView) findViewById(R.id.fp_description);
		titleText.setText(R.string.fingerprint_manager);
		Button titleBackBtn = (Button) findViewById(R.id.title_back);
		titleBackBtn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finish();
			}
		});

		mAlgoLog = (LinearLayout) findViewById(R.id.touchid_info);
		if (Preferences.getEnableEM() == true) {
			mAlgoLog.setVisibility(View.VISIBLE);
		} else {
			mAlgoLog.setVisibility(View.GONE);
		}
		mTopView = (TextView) findViewById(R.id.top_textview);
		mBehandView = (TextView) findViewById(R.id.behand_textview);
		mImageOne = (ImageView) findViewById(R.id.image_one);

		boolean enable = Preferences.getIsEnableFpUnlockscreen(this);
		mSwitchLockScreen = (ToggleButton) findViewById(R.id.switch_lockscreen);
		mSwitchLockScreen.setChecked(enable);
		mSwitchLockScreen
				.setOnCheckedChangeListener(new OnCheckedChangeListener() {
					public void onCheckedChanged(CompoundButton buttonView,
							boolean isChecked) {
						Preferences.setIsEnableFpUnlockscreen(
								TouchIDActivity.this, isChecked);
					}
				});

		mTouchIDListPanel = (ViewGroup) findViewById(R.id.fp_list_space);
		mShowDescription = (CheckBox) findViewById(R.id.enable_des);
		mShowDescription
				.setOnCheckedChangeListener(new OnEnableDesCheckedChangeListener());

		boolean bEnable = Preferences.getIsEnableDescription(this);
		mShowDescription.setChecked(bEnable);
		mShowMessage = (CheckBox)findViewById(R.id.enable_show_mes);
		mShowMessage.setOnCheckedChangeListener(new OnEnableDesCheckedChangeListener());
		mIsEnableShow = Preferences.getIsEnableShowMessage(this);
		mShowMessage.setChecked(bEnable);
	}

	private class OnEnableDesCheckedChangeListener implements OnCheckedChangeListener {
		@Override
		public void onCheckedChanged(CompoundButton checkBox, boolean enable) {
			switch (checkBox.getId()) {
				case R.id.enable_des:
					L.d("mShowDescription changed");
					Preferences.setIsEnableDescription(TouchIDActivity.this, enable);
					// mTouchIDListController.showDescriptions(enable);
					showDescriptions(enable);
					break;
				case R.id.enable_show_mes:
					L.d("mShowMessage changed");
				    Preferences.setIsEnableShowMessage(TouchIDActivity.this, enable);
				    mIsEnableShow = enable;
					showMatchedMessage(mIsEnableShow,0, 0);
					break;
				default:
					break;

			}

		}
	}
	
	private int getFingerViewIndex(int index) {
		if (mDataList == null) {
			return -1;
		}
		for (int i = 0; i < this.mDataList.size(); i++) {
			if (Integer.parseInt(this.mDataList.get(i).getUri()) == index) {
				return i;
			}
		}
		return -1;
	}
	
	public void showMatchedMessage(boolean enable,int index,int percent) {
		TextView mHolder;
		int viewIndex = getFingerViewIndex(index);
		if (null != mListViews) {
			for (int i = 0; i < mListViews.size(); i++) {
				mHolder = (TextView) mListViews.get(i).findViewById(R.id.fp_show_message);
				mHolder.setVisibility((enable && viewIndex == i)? View.VISIBLE : View.GONE);
				mHolder.setText(String.valueOf(percent));
			}
		}
	}

	public void showDescriptions(boolean enable) {
		View mHolder;
		if (null != mListViews) {
			for (int i = 0; i < mListViews.size(); i++) {
				mHolder = (View) mListViews.get(i).findViewById(R.id.fp_description);
				mHolder.setVisibility(enable ? View.VISIBLE : View.GONE);
			}
		}
	}

	@Override
	protected void onPause() {
		L.d("TouchiIdACTIVITY : onPause");
		if(mSession!=null)
			mSession.exit();
		super.onPause();

	}

	@Override
	protected void onResume() {
		L.d("TouchiIdACTIVITY : onResume");
		if(mSession!=null)
			mSession.enter();
		super.onResume();
	}

	@Override
	protected void onRestart() {
		L.d("TouchiIdACTIVITY : onRstart");
		super.onRestart();
	}
	

	@Override
	protected void onStop() {
		L.d("TouchiIdACTIVITY : onStop");
		super.onStop();
	}

	/**
	 * <p>
	 * Title: FpHandleServiceConnection
	 * </p>
	 * <p>
	 * Description:
	 * </p>
	 */

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK: {
			return super.onKeyDown(keyCode, event);
		}
		default:
			return super.onKeyDown(keyCode, event);
		}
	}

	@Override
	protected void onDestroy() {
		Log.v(TAG, "TouchIDActivity : onDestroy");
		L.d("TouchIDActivity : onDestroy");
		super.onDestroy();
		instance = null;
		if (null != mSession) {
			mSession.exit();
		}
		/* unbind */
		getApplicationContext().unbindService(mHandleServiceConn);
		/* start FingerprintHandlerService */
	}

	@Override
	public void onActivityResult(int request_code, int resultcode, Intent intent) {
		super.onActivityResult(request_code, resultcode, intent);
		if (resultcode == RESULT_OK)
			onCaptureActivityResult(request_code, resultcode, intent);
	}

	public void onCaptureActivityResult(int request_code, int resultcode,
			Intent intent) {
		if (resultcode == Activity.RESULT_OK) {
			Time time = new Time();
			time.setToNow();

			String name, description, uri;
			int mKey = getKey(mDataList);
			name = "Fingerprint" + mKey;
			description = "Description(" + time.hour + ":" + time.minute + ":" + time.second + ")";
			uri = intent.getStringExtra(TouchIDActivity.FRINGERPRINT_URI);

			Fingerprint fp = new Fingerprint(mKey, name, description, uri);
			if (this.mFingerPrintHandleService.insert(fp)) {
				this.mDataList.add(fp);

				// create view
				View item = loadNormalItem(fp);
				item.setOnClickListener(new OnFpItemOnClickListener(fp.getKey()));
				if (false == Preferences.getIsEnableDescription(this)) {
					item.findViewById(R.id.fp_description).setVisibility(
							View.GONE);
				}
				mListViews.add(item);
				this.mContainer.addView(item, mListViews.size() - 1);

				if (this.mFingerPrintHandleService.getDatabaseSpace() <= 0) {
					mInsertButton.setEnabled(false);
					mInsertButton.setTextColor(this.getResources().getColor(
							R.color.gray_level_three));
				}
			} else {
				Toast.makeText(this,getResources().getString(R.string.addfinger_faied_toast),Toast.LENGTH_SHORT).show();
			}
		}
	}
	
	private class FpHandleServiceConnection implements ServiceConnection {
		@Override
		public void onServiceConnected(ComponentName arg0, IBinder binder) {
			TouchIDActivity.this.mFingerPrintHandleService = ((ServiceBinder) binder).getService();
			Log.v(TAG, "FpHandleServiceConnection : onServiceConnected");
			mHandler.sendMessage(Message.obtain(mHandler,MSG_SERVICE_CONNECTED, 0, 0));
		}

		@Override
		public void onServiceDisconnected(ComponentName arg0) {
		}
	}

	/**
	 * @Title: getKey
	 * @Description:
	 * @param @return
	 * @return int
	 * @throws
	 */
	private int getKey(ArrayList<Fingerprint> dataList) {
		int mKey = 0;
		for (int i = 0; i < dataList.size(); i++)
			mKey = Math.max(dataList.get(i).getKey(), mKey);
		return ++mKey;
	}

	private void updateFingerprintItemsView() {
		if (null == mDataList) {
			return;
		}

		// create mContainer
		mContainer = (ViewGroup) this.mInflater.inflate(
				R.layout.fingerprint_list_container, null);
		mContainer.setLongClickable(true);
		mContainer.setLongClickable(true);
		// mContainer.setOnTouchListener(new OnTouchContaierListner());

		// create items
		for (int i = 0; i < mDataList.size(); i++) {
			View item = loadNormalItem(mDataList.get(i));
			item.setOnClickListener(new OnFpItemOnClickListener(mDataList
					.get(i).getKey()));
			mListViews.add(item);
			mContainer.addView(item);
		}

		// create add item.
		this.mInsertPanel = (ViewGroup) this.mInflater.inflate(
				R.layout.fingerprint_list_item_add, null);
		mInsertButton = (Button) mInsertPanel.findViewById(R.id.btn_add);
		mInsertButton.setOnClickListener(new OnAddBtnOnClickListener());
		if (this.mFingerPrintHandleService.getDatabaseSpace() > 0) {
			mInsertButton.setEnabled(true);
			mInsertButton.setTextColor(getResources().getColor(R.color.apple_blue));
		} else {
			mInsertButton.setEnabled(false);
			mInsertButton.setTextColor(getResources().getColor(R.color.gray_level_three));
		}
		mContainer.addView(mInsertPanel);
		mTouchIDListPanel.addView(mContainer);
		showDescriptions(Preferences.getIsEnableFpUnlockscreen(this));
	}

	private class OnAddBtnOnClickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			Intent intent = new Intent(TouchIDActivity.this,RegisterActivity.class);
			intent.putParcelableArrayListExtra(FRINGERPRINT_INDEX, mDataList);
			startActivityForResult(intent, 3);
		}
	}

	private View loadNormalItem(Fingerprint mFp) {
		View item = this.mInflater.inflate(
				R.layout.fingerprint_list_item_normal, null);
		((TextView) item.findViewById(R.id.fp_name)).setText(mFp.getName());
		((TextView) item.findViewById(R.id.fp_description)).setText(mFp
				.getDescription());
		return item;
	}

	private class OnFpItemOnClickListener implements View.OnClickListener {
		int mKey = -1;

		public OnFpItemOnClickListener(int key) {
			mKey = key;
		}

		@Override
		public void onClick(View v) {
			int index = -1;
			for (int i = 0; i < mDataList.size(); i++) {
				if (mKey == mDataList.get(i).getKey()) {
					index = i;
					break;
				}
			}
			if (-1 != index) {
				editorItem(index);
			}
		}
	}

	private void cancelEdit() {
		if (mEditorIndex != -1 && bEditFingerprint == true) {
			mContainer.removeViewAt(mEditorIndex);
			mContainer.addView(mListViews.get(mEditorIndex), mEditorIndex);
			mEditorIndex = -1;
			bEditFingerprint = false;
		}
	}

	private class HolderEditor {
		public EditText name;
		public EditText description;
		public Button delete;
		public Button ok;
		public Button cancel;
	}

	private void editorItem(int index) {
		if (true == bEditFingerprint) {
			cancelEdit();
			return;
		}
		bEditFingerprint = true;
		this.mEditorIndex = index;

		if (mEditorPanel == null) {
			mEditorPanel = (ViewGroup) this.mInflater.inflate(
					R.layout.fingerprint_list_item_editor, null);
			mEditorHolder = new HolderEditor();
			mEditorHolder.name = (EditText) mEditorPanel
					.findViewById(R.id.editor_name);
			mEditorHolder.description = (EditText) mEditorPanel
					.findViewById(R.id.edit_description);
			mEditorHolder.delete = (Button) mEditorPanel
					.findViewById(R.id.btn_delete);
			mEditorHolder.delete
					.setOnClickListener(new OnDeleteBtnOnClickListener());
			mEditorHolder.cancel = (Button) mEditorPanel
					.findViewById(R.id.btn_cancel);
			mEditorHolder.cancel
					.setOnClickListener(new OnCancelBtnOnClickListener());
			mEditorHolder.ok = (Button) mEditorPanel.findViewById(R.id.btn_ok);
			mEditorHolder.ok.setOnClickListener(new OnOkBtnOnClickListener());
			// add listener
		}
		mEditorHolder.name.setText(mDataList.get(index).getName());
		mEditorHolder.description
				.setText(mDataList.get(index).getDescription());
		mContainer.removeViewAt(mEditorIndex);
		mContainer.addView(mEditorPanel, mEditorIndex);
		startEditAnimation();
	}

	private void startEditAnimation() {
		AlphaAnimation animation = new AlphaAnimation(0, 1);
		animation.setDuration(500);
		mEditorPanel.startAnimation(animation);
	}

	private class OnCancelBtnOnClickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			cancelEdit();
		}
	}

	private class OnOkBtnOnClickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			mDataList.get(mEditorIndex).setName(
					mEditorHolder.name.getText().toString());
			mDataList.get(mEditorIndex).setDescription(
					mEditorHolder.description.getText().toString());
			mFingerPrintHandleService.update(mDataList.get(mEditorIndex));
			TextView name = (TextView) mListViews.get(mEditorIndex)
					.findViewById(R.id.fp_name);
			TextView des = (TextView) mListViews.get(mEditorIndex)
					.findViewById(R.id.fp_description);
			name.setText(mDataList.get(mEditorIndex).getName());
			des.setText(mDataList.get(mEditorIndex).getDescription());
			mContainer.removeViewAt(mEditorIndex);
			mContainer.addView(mListViews.get(mEditorIndex), mEditorIndex);
			mEditorIndex = -1;
			bEditFingerprint = false;
		}
	}

	private class OnDeleteBtnOnClickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			mListViews.remove(mEditorIndex);
			mContainer.removeViewAt(mEditorIndex);
			mFingerPrintHandleService.delete(mDataList.get(mEditorIndex)
					.getKey());
			try {
				int index = Integer.parseInt(mDataList.get(mEditorIndex)
						.getUri());
				FpApplication.getInstance().getFpServiceManager().delete(index);
			} catch (Exception e) {
				e.printStackTrace();
			}
			mDataList.remove(mEditorIndex);
			mInsertButton.setEnabled(true);
			mInsertButton.setTextColor(getResources().getColor(
					R.color.apple_blue));
			mEditorIndex = -1;
			bEditFingerprint = false;
		}
	}

	private ArrayList<Fingerprint> loadData(ArrayList<Fingerprint> dataList,
			int fpFlag) {
		if (null == dataList || (fpFlag >> 16) <= 0) {
			return null;
		}
		ArrayList<Fingerprint> tempList = new ArrayList<Fingerprint>();
		int mKey = getKey(dataList);
		int count = (fpFlag >> 16 & 0xFFFF);
		boolean[] bRegister = new boolean[count];

		for (int i = 0; i < count; i++) {
			bRegister[i] = (((fpFlag >> i) & 0x1) > 0) ? true : false;
			if (bRegister[i] == true) {
				boolean bFind = false;
				for (int j = 0; j < dataList.size(); j++) {
					if (Integer.parseInt(dataList.get(j).getUri()) == i + 1) {
						Fingerprint fp = dataList.remove(j);
						tempList.add(fp);
						bFind = true;
						break;
					}
				}
				if (bFind == false) {
					Fingerprint fp = new Fingerprint(mKey, "unknown",
							"unknown", Integer.toString(i + 1));
					mKey++;
					tempList.add(fp);
					this.mFingerPrintHandleService.insert(fp);
				}
			}
		}

		for (int i = 0; i < dataList.size(); i++) {
			this.mFingerPrintHandleService.delete(dataList.get(i).getKey());
		}
		return tempList;
	}

	private void startInitFingerprintThread() {
		new InitFingerprintThread().start();
	}

	private class InitFingerprintThread extends Thread {
		public void run() {
			if (null != mFingerPrintHandleService && null != FpApplication.getInstance().getFpServiceManager()) {
				int flag = FpApplication.getInstance().getFpServiceManager().query();
				ArrayList<Fingerprint> dataList = loadData(mFingerPrintHandleService.query(), flag);
				mHandler.sendMessage(Message.obtain(mHandler,MSG_DATA_IS_READY, 0, 0, dataList));
			}
		}
	}

	private IVerifyCallback mVerifyCallBack = new IVerifyCallback.Stub() {
		@Override
		public void handleMessage(int msg, int arg0, int arg1, byte[] data)
				throws RemoteException {
			Log.v(TAG,String.format("%s , arg0 = %d , arg1 = %d",MessageType.getString(msg), arg0, arg1));
			switch (msg) {
			case MessageType.MSG_TYPE_RECONGNIZE_SUCCESS:
				mHandler.sendMessage(mHandler.obtainMessage(MSG_VERIFY_SUCCESS,
						arg0, arg1, data));
				break;
			case MessageType.MSG_TYPE_RECONGNIZE_FAILED:
				mHandler.sendMessage(mHandler.obtainMessage(MSG_VERIFY_FAILED,
						arg0, arg1, data));
				break;

			case MessageType.MSG_TYPE_COMMON_NOTIFY_INFO:
				mHandler.sendMessage(mHandler.obtainMessage(MSG_DEBUG_INFO,
						arg0, arg1, data));
				break;

			default:
				break;
			}
		//	return false;
		}
	};

	private void unmatchAnimation(View v) {
		ScaleAnimation anim;
		anim = new ScaleAnimation(1f, 0.995f, 1f, 0.995f,
				Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF,
				0.5f);
		anim.setRepeatCount(1);
		anim.setDuration(300);
		anim.setAnimationListener(new unMatchedAnimationListener(v));
		v.startAnimation(anim);
	}

	private class unMatchedAnimationListener implements AnimationListener {
		View view = null;

		unMatchedAnimationListener(View v) {
			this.view = v;
		}

		@Override
		public void onAnimationEnd(Animation animation) {
			view.setBackgroundResource(R.drawable.setting_button_left_padding);
		}

		@Override
		public void onAnimationRepeat(Animation animation) {
		}

		@Override
		public void onAnimationStart(Animation animation) {
			view.setBackgroundResource(R.drawable.touchid_worning_bg);
		}
	}

	private class MatchedAnimationListener implements AnimationListener {
		View view = null;

		MatchedAnimationListener(View v) {
			this.view = v;
		}

		@Override
		public void onAnimationEnd(Animation animation) {
			view.setBackgroundColor(0xFFFFFFFF);
		}

		@Override
		public void onAnimationRepeat(Animation animation) {
		}

		@Override
		public void onAnimationStart(Animation animation) {
			view.setBackgroundColor(0xFFDDDDDD);
		}
	}

	private void showAnimation(View v) {
		ScaleAnimation anim_time;
		anim_time = new ScaleAnimation(1f, 0.99f, 1f, 0.99f,
				Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF,
				0.5f);
		// anim_time.setFillAfter(true);
		anim_time.setRepeatCount(2);
		anim_time.setDuration(300);
		anim_time.setAnimationListener(new MatchedAnimationListener(v));
		v.startAnimation(anim_time);
	}

	public void showMatchedAnimation(int index) {
		for (int i = 0; i < this.mDataList.size(); i++) {
			if (Integer.parseInt(this.mDataList.get(i).getUri()) == index) {
				showAnimation(mListViews.get(i));
				break;
			}
		}
	}

	private int getViewCount() {
		if (mListViews != null) {
			return mListViews.size();
		}
		return 0;
	}

	private static class MyHandler extends Handler {
		private WeakReference<TouchIDActivity> mActivityRef;

		public MyHandler(TouchIDActivity activity) {
			mActivityRef = new WeakReference<TouchIDActivity>(activity);
		}

		public void handleMessage(Message msg) {
			if (mActivityRef.get() == null)
				return;
			TouchIDActivity activity = (TouchIDActivity) mActivityRef.get();
			if (null == activity) {
				return;
			}

			switch (msg.what) {
			case MSG_SERVICE_CONNECTED:
				if (null != activity.mFingerPrintHandleService
						/*&& null != activity.mFpManagerService*/) {
					activity.startInitFingerprintThread();
				}
				break;
			case MSG_DATA_IS_READY: {
				if (msg.obj instanceof ArrayList) {
					activity.mDataList = (ArrayList<Fingerprint>) msg.obj;
					/* update view */
					activity.updateFingerprintItemsView();
					if (null == activity.mSession) {
						activity.mSession = FpApplication.getInstance().getFpServiceManager().newVerifySession(activity.mVerifyCallBack);
						activity.mSession.enter();
					}
				}
			}
				break;
			case MSG_VERIFY_SUCCESS:
				activity.showMatchedMessage(activity.mIsEnableShow,msg.arg2, msg.arg1);
				if (msg.arg2 > 0) {
					activity.showMatchedAnimation(msg.arg2);
				} else {
					activity.unmatchAnimation(activity.mTouchIDListPanel);
				}
				break;

			case MSG_VERIFY_FAILED:
				activity.showMatchedMessage(activity.mIsEnableShow,msg.arg2, msg.arg1);
				activity.unmatchAnimation(activity.mTouchIDListPanel);
				break;

			case MSG_DEBUG_INFO:
				byte[] data = (byte[]) msg.obj;
				if (data != null) {
					String str = new String(data);
					if (AlgoResult.isFilePath(str)) {
						activity.mTopView.setText(AlgoResult.bulidLog(activity,str, AlgoResult.FILTER_RECOGNIZE,activity.getViewCount()));
					}
					activity.mBehandView.setText(AlgoResult.bulidLog(activity,str, AlgoResult.FILTER_RECOGNIZE,activity.getViewCount()));

					int index = str.indexOf("=");

					if (-1 != index) {
						String fileName = null;
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
				break;

			default:
				break;
			}
		}
	}
}
