package com.goodix.fpsetting;

import com.goodix.application.FpApplication;
import com.goodix.util.L;
import com.goodix.util.Preferences;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * <p>
 * Title: PasswordActivity
 * </p>
 * <p>
 * Description:
 * </p>
 */
public class PasswordActivity extends Activity
{
    public static final String CHECK_PASSWORD        = "check.password";
    public static final String CHECK_PASSWORD_RESULT = "check.password.result";
    public static final String CHANGE_PASSWORD       = "change.password";
    public static final String CHECK_RETURN          = "check.return";
    public static final String START_TYPE_KEY        = "start_type";
    public static final String FIRST_START        = "first_start";
    
    private static final int PSW_LENGTH = 4;
    private int mAlreadyInputLength = 0;
    private int[] mOldPassword = new int[PSW_LENGTH];
    private int[] mNewPassword = new int[PSW_LENGTH];
    private int[] mTempPassword = new int[PSW_LENGTH];
    private boolean bEqualOldPassword = false;
    private boolean bEqualPrePassword = true;
    private String START_TYPE;
    private InputState mInputState = InputState.INPUT_NO_INPUT;
    private ImageView[] mImageviews = new ImageView[PSW_LENGTH];
    private Button mTitleBackButton;
    private TextView mInputInfoView = null;
    private TextView mInputErrerInfoView = null;
    
    private int mErrorInputTimes = 0;
    
    
    public enum InputState
    {
        INPUT_NO_INPUT,
        INPUT_NORMAL_INPUT, 
        INPUT_OLD_PASSWORD, 
        INPUT_NEW_PASSWORD_PRE, 
        INPUT_NEW_PASSWORD_AG
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
        setContentView(R.layout.activity_password);
        getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.setting_title);
        loadView();
        initActivity();
/*		if (this.START_TYPE == CHECK_PASSWORD
				&& this.mInputState == InputState.INPUT_NORMAL_INPUT) {
			if (!Preferences.getIsEnablePswSetting(PasswordActivity.this)) {
				Intent intent1 = new Intent(PasswordActivity.this,PreferencesActivity.class);
				startActivity(intent1);
				finish();
			}
		}*/
        updateScreen();
    }

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	private void initActivity() {
		Intent intent = getIntent();
		this.START_TYPE = intent.getStringExtra(START_TYPE_KEY);
		boolean isFirstStart=Preferences.getPassword(PasswordActivity.this)>12344?true:false;
		if (null == this.START_TYPE) {
			if(isFirstStart) {
				this.mInputState = InputState.INPUT_NEW_PASSWORD_PRE;
				this.bEqualOldPassword = false;
				this.bEqualPrePassword = true;
				this.mErrorInputTimes = 0;
				this.START_TYPE= FIRST_START;
			} else {
				this.START_TYPE = CHECK_PASSWORD;
				this.mInputState = InputState.INPUT_NORMAL_INPUT;
				this.bEqualOldPassword = false;
				this.bEqualPrePassword = true;
				this.mErrorInputTimes = 0;
			}
		} else if (CHANGE_PASSWORD.equals(this.START_TYPE)) {
			this.mInputState = InputState.INPUT_OLD_PASSWORD;
			TextView titleText = (TextView) findViewById(R.id.fp_description);
			titleText.setText(R.string.password_change_password);
		} else if (CHECK_PASSWORD_RESULT.equals(this.START_TYPE)) {
			this.mInputState = InputState.INPUT_NORMAL_INPUT;
			this.bEqualOldPassword = false;
			this.bEqualPrePassword = true;
			this.mErrorInputTimes = 0;
		} else
			finish();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	private void loadView() {
		mImageviews[0] = (ImageView) findViewById(R.id.password_one);
		mImageviews[1] = (ImageView) findViewById(R.id.password_two);
		mImageviews[2] = (ImageView) findViewById(R.id.password_three);
		mImageviews[3] = (ImageView) findViewById(R.id.password_four);

		setButtonListener(R.id.key_one, new OnKeyClickListener(1));
		setButtonListener(R.id.key_two, new OnKeyClickListener(2));
		setButtonListener(R.id.key_three, new OnKeyClickListener(3));
		setButtonListener(R.id.key_four, new OnKeyClickListener(4));
		setButtonListener(R.id.key_five, new OnKeyClickListener(5));
		setButtonListener(R.id.key_six, new OnKeyClickListener(6));
		setButtonListener(R.id.key_seven, new OnKeyClickListener(7));
		setButtonListener(R.id.key_eight, new OnKeyClickListener(8));
		setButtonListener(R.id.key_nine, new OnKeyClickListener(9));
		setButtonListener(R.id.key_zero, new OnKeyClickListener(0));
		setButtonListener(R.id.key_clear, new OnKeyClickListener(10));

		mInputInfoView = (TextView) findViewById(R.id.text_input_info);
		mInputErrerInfoView = (TextView) findViewById(R.id.text_input_wrong);
		mTitleBackButton = (Button) findViewById(R.id.title_back);
		mTitleBackButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finish();
			}
		});
	}

    private void setButtonListener(int id, OnKeyClickListener listener)
    {
        Button btn = (Button) findViewById(id);
        if (null != btn && null != listener)
            btn.setOnClickListener(listener);
    }
    
    private void updateScreen()
    {
        // update input icon
        for (int i = 0; i < PSW_LENGTH; i++)
        {
            if (i < mAlreadyInputLength)
                mImageviews[i].setImageResource(R.drawable.ic_already_input);
            else
                mImageviews[i].setImageResource(R.drawable.ic_password_input);
        }
        // update text
        switch (this.mInputState)
        {
            case INPUT_NEW_PASSWORD_PRE:
                mInputInfoView.setText(R.string.input_new_password);
                break;
            case INPUT_NEW_PASSWORD_AG:
                mInputInfoView.setText(R.string.input_new_password_again);
                break;
            case INPUT_NORMAL_INPUT:
                mInputInfoView.setText(R.string.input_password);
                break;
            case INPUT_OLD_PASSWORD:
                mInputInfoView.setText(R.string.input_old_password);
                break;
            default:
                break;
        }
        // update error text
        switch (this.mInputState)
        {
            case INPUT_NEW_PASSWORD_PRE:
                if (bEqualOldPassword == true)
                {
                    mInputErrerInfoView.setVisibility(View.VISIBLE);
                    mInputErrerInfoView.setText(R.string.input_other_password);
                }
                else if (bEqualPrePassword == false)
                {
                    mInputErrerInfoView.setVisibility(View.VISIBLE);
                    mInputErrerInfoView.setText(R.string.password_not_equal_pre);
                }
                else
                    mInputErrerInfoView.setVisibility(View.INVISIBLE);
                break;
            case INPUT_NEW_PASSWORD_AG:
                mInputErrerInfoView.setVisibility(View.INVISIBLE);
            case INPUT_NORMAL_INPUT:
            case INPUT_OLD_PASSWORD:
                if (mErrorInputTimes == 0)
                {
                    mInputErrerInfoView.setVisibility(View.INVISIBLE);
                }
                else
                {
                    mInputErrerInfoView.setVisibility(View.VISIBLE);
                    mInputErrerInfoView.setText(mErrorInputTimes + getResources().getString(R.string.wrong_password));
                }
                break;
            default:
                break;
        }
    }
    
    private boolean checkPassword(int[] password)
    {
        int length = password.length;
        boolean result = false;
        if (length > 0)
        {
            String pwd = "";
            for (int i = 0; i < length; i++)
            {
                pwd += password[i];
            }

            L.v("checkPassword: password = " + pwd);
   //         result = FpApplication.getInstance().getFpServiceManager().checkPassword(pwd);
            result=((Integer.toString(length=Preferences.getPassword(PasswordActivity.this))).equals(pwd));
        }
        return result;
    }
    
    private boolean checkPasswordEqual(int[] pre, int[] ag)
    {
        for (int i = 0; i < PSW_LENGTH; i++)
            if (pre[i] != ag[i])
                return false;
        return true;
    }

	private void passwordAssign(int[] tag, int[] src) {
		for (int i = 0; i < PSW_LENGTH; i++) {
			tag[i] = src[i];
		}
	}
    
    private void savePasswod(int[] password) throws RuntimeException
    {
        try
        {
            String old = "1234";
            String PWD = "" + password[0] + password[1] + password[2] + password[3];
       //     FpApplication.getInstance().getFpServiceManager().changePassword(old, PWD);
            Preferences.setPassword(PasswordActivity.this,Integer.parseInt(PWD));
        }
        catch (RuntimeException e)
        {
            throw e;
        }
    }
    
    private void capturePassword()
    {
        switch (this.mInputState)
        {
            case INPUT_OLD_PASSWORD:
                if (checkPassword(this.mTempPassword))
                {
                    passwordAssign(this.mOldPassword, this.mTempPassword);
                    this.mInputState = InputState.INPUT_NEW_PASSWORD_PRE;
                    mErrorInputTimes = 0;
                }
                else
                {
                    mErrorInputTimes++;
                }
                break;
            
            case INPUT_NEW_PASSWORD_PRE:
                if (checkPasswordEqual(this.mOldPassword, mTempPassword))
                {
                    // same as old password
                    this.bEqualOldPassword = true;
                    this.bEqualPrePassword = true;
                    this.mInputState = InputState.INPUT_NEW_PASSWORD_PRE;
                }
                else
                {
                    this.bEqualOldPassword = false;
                    passwordAssign(this.mNewPassword, this.mTempPassword);
                    this.mInputState = InputState.INPUT_NEW_PASSWORD_AG;
                    this.bEqualPrePassword = true;
                }
                break;
            case INPUT_NEW_PASSWORD_AG:
                if (checkPasswordEqual(this.mNewPassword, mTempPassword))
                {
                    this.bEqualOldPassword = false;
                    this.bEqualPrePassword = true;
                    try
                    {
                        savePasswod(mTempPassword);
                    }
                    catch (RuntimeException e)
                    {
                        e.printStackTrace();
                    }
                    mAlreadyInputLength = 4;
                    if (this.START_TYPE == null)
                    { 
        /*                Intent intent = new Intent(PasswordActivity.this,
                                PreferencesActivity.class);
                        startActivity(intent);*/
                    	Intent resultIntent = new Intent();
                    	resultIntent.putExtra(CHECK_RETURN, true);
                    	setResult(Activity.RESULT_OK, resultIntent);
                    	finish();

                    } else if (this.START_TYPE==FIRST_START) {
                    	mErrorInputTimes = 0;
                        Intent intent = new Intent(PasswordActivity.this,
                                PreferencesActivity.class);
                        startActivity(intent);
                    }
                    
                    finish();
                    
                }
                else
                {
                    this.bEqualPrePassword = false;
                    this.bEqualOldPassword = false;
                    this.mInputState = InputState.INPUT_NEW_PASSWORD_PRE;
                }
                break;
            
            case INPUT_NORMAL_INPUT:
                if (checkPassword(this.mTempPassword))
                {
                    if (CHECK_PASSWORD_RESULT.equals(this.START_TYPE))
                    {

                        Intent intent = new Intent(PasswordActivity.this,
                                PreferencesActivity.class);
                        intent.putExtra(CHECK_RETURN, true);
                        setResult(RESULT_OK, intent);
                        finish();
                    }
                    else if (CHECK_PASSWORD.equals(this.START_TYPE))
                    {

                        mErrorInputTimes = 0;
                        Intent intent = new Intent(PasswordActivity.this,
                                PreferencesActivity.class);
                        startActivity(intent);
                        finish();
                    }
                    else
                        finish();
                    mAlreadyInputLength = 4;
                }
                else
                {
                    mErrorInputTimes++;
                    if(mErrorInputTimes>5) {
                    	finish();
                    }
                }
                break;
            default:
                break;
        }
    }
    
    private class OnKeyClickListener implements View.OnClickListener
    {
        private int mKey = 0;
        public OnKeyClickListener(int key)
        {
            mKey = key;
        }
        @Override
        public void onClick(View v)
        {
            if (mAlreadyInputLength < PSW_LENGTH && mKey < 10)
            {
                if (null != mTempPassword)
                {
                    mTempPassword[mAlreadyInputLength] = mKey;
                    mAlreadyInputLength++;
                    if (mAlreadyInputLength == PSW_LENGTH)
                    {
                        mAlreadyInputLength = 0;
                        capturePassword();
                    }
                    PasswordActivity.this.updateScreen();
                }
            }
            else if (mKey == 10)
            {
                if (mAlreadyInputLength != 0)
                {
                    mAlreadyInputLength--;
                    PasswordActivity.this.updateScreen();
                }
            }
        }
    }
}
