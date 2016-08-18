package com.goodix.fpsetting;

import android.content.res.Configuration;
import com.goodix.util.Preferences;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.ToggleButton;

/*
 * <p>
 * Title: PreferencesActivity
 * </p>
 * <p>
 * Description: Ö¸ÎÆID ºÍ ÃÜÂëÉèÖÃ½çÃæ
 * </p>
 */
public class PreferencesActivity extends Activity {
	public static final int CHECK_PASSWORD = 1;
	public static final int CHANGE_PASSWORD = 2;

	private static final int REQUEST_CODE_PASSWORD = 1;
	private static final int REQUEST_CODE_SERIALPORT = 2;
	private boolean bPasswordStatus = false;
	private Button mChangePswBtn = null;
	private ToggleButton mSettingPswBtn = null;
	private Button mTitleBackBtn = null;
    private View mFpManagerPanel = null;
    private TextView mFpManagerText;
	private Button mAboutMe = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_preferences);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.title);
		bPasswordStatus = Preferences.getPasswordIsBeUsed(this);
		loadView();
		updateScreen();
		startService();
	}
    
	@Override
	protected void onDestroy() {
		super.onDestroy();
	}
    
    /**
     * <p>
     * Title: startService
     * </p>
     * <p>
     * Description: 
     * </p>
     */
	private void startService() {

	}
    
	@Override
	public void onActivityResult(int request_code, int resultcode, Intent data) {
		super.onActivityResult(request_code, resultcode, data);
		if (resultcode == RESULT_OK) { 
			if (null != data && request_code == REQUEST_CODE_PASSWORD) {
				boolean checkresult = data.getBooleanExtra(
						PasswordActivity.CHECK_RETURN, false);
				if (true == checkresult) {
					this.bPasswordStatus = !Preferences
							.getPasswordIsBeUsed(this);
					Preferences.setPasswordIsBeUsed(this, this.bPasswordStatus);
					updateScreen();
				}
			}
			if (null != data && request_code == REQUEST_CODE_SERIALPORT) {
				updateScreen();
			}
		}
	}
    
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
    
    /**
     * <p>
     * Title: loadView
     * </p>
     * <p>
     * Description: ³õÊ¼»¯views
     * </p>
     */
    private void loadView() {
    
        mChangePswBtn = (Button) findViewById(R.id.btn_change_psw);
        mChangePswBtn.setOnClickListener(new OnChangePswClickListener());
        mChangePswBtn.setEnabled(false);
        

        mFpManagerText = (TextView) findViewById(R.id.preferences_txt_mng);
        
        mFpManagerPanel = (View) findViewById(R.id.preferences_fpmanager_panel);
        mFpManagerPanel.setOnClickListener(new OnSetTouchIDClickListener());

        mTitleBackBtn = (Button) findViewById(R.id.title_back);
		mTitleBackBtn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finish();
			}
		});
        
        

		mAboutMe = (Button) findViewById(R.id.aboutme);
		mAboutMe.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(PreferencesActivity.this,AboutActivity.class);
				startActivity(intent);

			}
		});

	}
    /*
     * <p>
     * Title: updateScreen
     * </p>
     * <p>
     * Description: 
     * </p>
     */
	private void updateScreen() {
		if (false == bPasswordStatus) {
			mChangePswBtn.setEnabled(false);
			mChangePswBtn.setTextColor(getResources().getColor(
					R.color.gray_level_three));

			mFpManagerPanel.setClickable(false);
			mFpManagerText.setTextColor(getResources().getColor(
					R.color.gray_level_three));
		} else {

			mChangePswBtn.setEnabled(true);
            mChangePswBtn.setTextColor(getResources().getColor( R.color.apple_blue));

            mFpManagerPanel.setClickable(true);
            mFpManagerText.setTextColor(getResources().getColor(
                    R.color.apple_blue));
        }
        
    }
    
    /*
     * <p>
     * Title: OnChangePswClickListener
     * </p>
     * <p>
     * Description: ¸ü¸ÄÃÜÂë °´¼üclick ¼àÌýÆ÷
     * </p>
     */
	private class OnChangePswClickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			Intent intent = new Intent(PreferencesActivity.this,
					PasswordActivity.class);
			intent.putExtra(PasswordActivity.START_TYPE_KEY,
					PasswordActivity.CHANGE_PASSWORD);
			startActivityForResult(intent, 2);
		}
	}
    
    /*
     * <p>
     * Title: OnSetTouchIDClickListener
     * </p>
     * <p>
     * Description: ±à¼­´¥ÃþID °´¼üclick ¼àÌýÆ÷
     * </p>
     */
	private class OnSetTouchIDClickListener implements View.OnClickListener {
		@Override
		public void onClick(View v) {
			Intent intent = new Intent(PreferencesActivity.this,
					TouchIDActivity.class);
			startActivity(intent);
		}
	}
    

}
