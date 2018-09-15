/************************************************************************
 * <p>Title: AboutActivity.java</p>
 * <p>Description: </p>
 * <p>Copyright (C), 1997-2014, Shenzhen Goodix Technology Co.,Ltd.</p>
 * <p>Company: Goodix</p>
 * @author  peng.hu
 * @date    2014-7-7
 * @version  1.0
 ************************************************************************/

package com.goodix.fpsetting;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;

import com.goodix.application.FpApplication;

/**
 * <p>
 * Title: AboutActivity.java
 * </p>
 * <p>
 * Description:
 * </p>
 */
public class AboutActivity extends Activity {
	private Button mOkButton;
	private TextView mAboutTxt;
	private String mAlgorithmName;
	private String mApplicationName;
	private String mHardwareName;


	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_about);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.title);
		loadView();

	}

	private void loadView() {
		TextView titleText = (TextView) findViewById(R.id.fp_description);
		titleText.setText(R.string.about);
		mHardwareName = getResources().getString(R.string.mode_version);
		mApplicationName = getResources().getString(R.string.application_version);
		mAlgorithmName = getResources().getString(R.string.Algorithm_version);
		Button titleBackBtn = (Button) findViewById(R.id.title_back);
		titleBackBtn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finish();
			}
		});

		mOkButton = (Button) findViewById(R.id.about_ok);
		mOkButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				finish();
			}
		});
		mAboutTxt = (TextView) findViewById(R.id.abouttext);
		mAboutTxt.setText(aboutInfo());
	}

	private CharSequence aboutInfo() {
		StringBuffer s = new StringBuffer();
		s.append(mAlgorithmName + getAlgorithmVersion() + "\n"
				+ mApplicationName + getAppVersion() + "\n" + mHardwareName
				+ getHardwareVersion());
		return s;

	}

	private String getAlgorithmVersion() {
		String algorithm = "unkown";
		try {
			if (FpApplication.getInstance().getFpServiceManager() != null) {
				int index = FpApplication.getInstance().getFpServiceManager().getInformation().indexOf(",");
				algorithm = FpApplication.getInstance().getFpServiceManager().getInformation().substring(index + 1);

			}
		} catch (Exception e)  {
		      return algorithm;
		}
		return algorithm;
	}

	private String getAppVersion() {
		try {
			PackageInfo verCode = this.getPackageManager().getPackageInfo(
					this.getPackageName(), 0);
			return verCode.versionName;
		} catch (Exception e) {
		
			return "unknown";
		}
	}

	private String getHardwareVersion() {
		String sHardwareVersion = "unkown";
		try {
			if (FpApplication.getInstance().getFpServiceManager() != null) {
				int index = FpApplication.getInstance().getFpServiceManager().getInformation().indexOf(",");
				sHardwareVersion = FpApplication.getInstance().getFpServiceManager().getInformation().substring(0, index);
			}
		} catch (Exception e) {
		      return sHardwareVersion;
		}
		return sHardwareVersion;
	}
}
