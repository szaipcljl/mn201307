package com.intel.sensorhub;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;

import android.content.Context;
import android.os.Environment;
import android.os.storage.StorageManager;
import android.util.Log;

public class DataFileSave {
	private static final String TAG = "DataFileSave";

	public static final String EXTERNAL_STORAGE_PATH = "/storage/sdcard1";
	public static final String INTERNAL_STORAGE_PATH = "/storage/emulated/0";

	public static final String CALIBRATION_DATA_FILE_NAME = "/storage/emulated/0/sensor_calibration_AGM.bin";
	public static final String OUTPORT_DATA_FILE_NAME = "/storage/sdcard1/sensor_calibration_AGM.bin";
	// public static final String OUTPORT_DATA_FILE_NAME =
	// "/storage/emulated/0/testee.txt";

	private Context mContext;
	private StorageManager mStorageManager;
	private File mFilePath;

	DataFileSave(Context context) {
		mContext = context;
		mStorageManager = (StorageManager) mContext
				.getSystemService(Context.STORAGE_SERVICE);

		mFilePath = Environment.getExternalStorageDirectory();
		Log.d(TAG, mFilePath.getPath());

	}

	public static void getAllFiles(File dir, int level) {
		Log.d(TAG, "dir:" + dir.getName());
		level++;
		File[] files = dir.listFiles();
		for (int i = 0; i < files.length; i++) {
			if (files[i].isDirectory()) {
				// 这里面用了递归的算法
				// getAllFiles(files[i], level);
				Log.d(TAG, "file:" + files[i].getName());
			} else {
				Log.d(TAG, "file:" + files[i]);

				if (files[i].exists()) {
					Log.d(TAG, "=================");
				}
			}
		}
	}

	public static String getExternalStoragePath() {
		String path = null;
		File extStorage = new File(EXTERNAL_STORAGE_PATH);
		String[] files = extStorage.list();
		if (files == null || files.length == 0) {
			path = INTERNAL_STORAGE_PATH;
		} else {
			path = EXTERNAL_STORAGE_PATH;
		}
		return path;
	}

	private boolean IsSDCardInsert() {
		File extStorage = new File(EXTERNAL_STORAGE_PATH);
		String[] files = extStorage.list();
		if (files == null || files.length == 0) {
			Log.d(TAG, "Have no SD card.");
			return false;
		} else {
			return true;
		}
	}

	public int SaveDataToSDCard() {

		if (IsSDCardInsert()) {
			String sSourceFilePath = CALIBRATION_DATA_FILE_NAME;
			String sDirectFilePath = OUTPORT_DATA_FILE_NAME;

			String sdCardRoot = Environment.getExternalStorageDirectory()
					.getAbsolutePath();
			Log.d(TAG, "sdCardRoot:" + sdCardRoot);

			try {
				int byteread = 0;
				File mCalibrateDataFile = new File(sSourceFilePath);
				File mOutPortFile = new File(sDirectFilePath);

				Log.d(TAG, "file:" + sSourceFilePath);
				if (mCalibrateDataFile.exists()) { // 文件存在时
					if (!mOutPortFile.exists()) {
						if (mOutPortFile.createNewFile()) {
							Log.d(TAG, "create file " + sDirectFilePath
									+ " success");
						} else {
							Log.d(TAG, "create file " + sDirectFilePath
									+ " fail");
						}
					} else {
						Log.d(TAG, "file " + sDirectFilePath + " exists");
					}
					InputStream inStream = new FileInputStream(
							mCalibrateDataFile); // 读入原文件
					FileOutputStream fs = new FileOutputStream(mOutPortFile);
					byte[] buffer = new byte[2048];
					while ((byteread = inStream.read(buffer)) != -1) {
						fs.write(buffer, 0, byteread);
					}
					inStream.close();
					fs.close();
					return 0;
				} else {
					Log.d(TAG, "file no exit...");
				}
			} catch (Exception e) {
				Log.d(TAG, "Exception e");
				e.printStackTrace();

			}

			Log.d(TAG, "copy end");
		}
		return 1;
	}
}
