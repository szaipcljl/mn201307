package com.intel.sensorhub;

import java.io.File;

import android.content.Context;
import android.os.Environment;
import android.os.storage.StorageManager;
import android.util.Log;

public class DataFileSave {
	private static final String TAG = "DataFileSave";

	public static final String EXTERNAL_STORAGE_PATH = "/storage/sdcard1";
	public static final String INTERNAL_STORAGE_PATH = "/storage/emulated/0";

	private Context mContext;
	private StorageManager mStorageManager;
	private File mFilePath;

	DataFileSave(Context context) {
		mContext = context;
		mStorageManager = (StorageManager) mContext
				.getSystemService(Context.STORAGE_SERVICE);

		mFilePath = Environment.getExternalStorageDirectory();
		Log.d(TAG,
				mFilePath.getPath() + "    state:"
						+ Environment.getStorageState(mFilePath));
		Log.d(TAG, Environment.getExternalStorageState());
	}

	private int SaveDataToSDCard() {

		return 0;
	}
}
