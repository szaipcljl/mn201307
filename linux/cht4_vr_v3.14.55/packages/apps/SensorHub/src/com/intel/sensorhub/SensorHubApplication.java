package com.intel.sensorhub;

import java.util.Locale;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Configuration;
import android.util.DisplayMetrics;

public class SensorHubApplication extends Application {
	private static final String LANGUAGE = "zh";
	// private static final String LANGUAGE = "en";

	public static final String LANGUAGE_KEY = "Language";

	@Override
	public void onCreate() {
		super.onCreate();
		updateResourcesConfiguration();
		SaveLanguage();
	}

	private void updateResourcesConfiguration() {
		Locale locale = new Locale(LANGUAGE);
		Locale.setDefault(locale);
		Configuration config = getResources().getConfiguration();
		DisplayMetrics metrics = getResources().getDisplayMetrics();
		if (LANGUAGE.equals("zh")) {
			config.locale = Locale.SIMPLIFIED_CHINESE;
		} else {
			config.locale = Locale.ENGLISH;
		}
		getResources().updateConfiguration(config, metrics);
	}
	
	private void SaveLanguage() {
		SharedPreferences pre = getSharedPreferences(LANGUAGE_KEY,
				Context.MODE_PRIVATE);
		//String mLocalLangage = Locale.getDefault().getLanguage();
		Editor editor = pre.edit();
		editor.putString(LANGUAGE_KEY, LANGUAGE);
		//editor.putString(LANGUAGE_KEY, mLocalLangage);
		editor.commit();
	}

}
