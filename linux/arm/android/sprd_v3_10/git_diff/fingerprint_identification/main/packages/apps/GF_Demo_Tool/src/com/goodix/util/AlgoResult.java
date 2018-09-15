package com.goodix.util;

import java.util.StringTokenizer;

import com.goodix.fpsetting.R;

import android.content.Context;
import android.content.BroadcastReceiver;

@SuppressWarnings("unused")
public class AlgoResult {

	static String[] sRecgnizeInfo = new String[25];
	static String[] sRegisterInfo = new String[20];

	public static final int REC_PICTURE_QUALITY = 0;
	public static final int REC_VALID_SIZE = 1;
	public static final int REC_STUDY1 = 2;
	public static final int REC_STUDY2 = 3;
	public static final int REC_STUDY3 = 4;
	public static final int REC_STUDY4 = 5;
	public static final int REC_STUDY5 = 6;
	public static final int REC_MATCH_DEGREE1 = 7;
	public static final int REC_MATCH_DEGREE2 = 8;
	public static final int REC_MATCH_DEGREE3 = 9;
	public static final int REC_MATCH_DEGREE4 = 10;
	public static final int REC_MATCH_DEGREE5 = 11;
	public static final int REC_ALGL_TIME1 = 12;
	public static final int REC_ALGL_TIME2 = 13;
	public static final int REC_ALGL_TIME3 = 14;
	public static final int REC_ALGL_TIME4 = 15;
	public static final int REC_ALGL_TIME5 = 16;

	public static final int REG_PICTURE_QUALITY = 0;
	public static final int REG_VALID_SIZE = 1;
	public static final int REG_STICK_INFO = 2;
	public static final int REG_OVERBACK = 3;
	public static final int REG_PROGRESS = 4;
	public static final int REG_TEMPLATE_COUNT = 5;
	public static final int REG_OVERLAY_RATE = 6;
	public static final int REG_OVERLAY_COUNT = 7;
	public static final int REG_ALGO_TIME = 8;
	public static final int REG_UPOVERLAY_RATE = 9;
  
	public static final int FILTER_IS_BAD_IMAGE = 0x00000001;
	public static final int FILTER_IS_DELETE = 0x00000002;
	public static final int FILTER_IS_STITCH = 0x00000004;
	public static final int FILTER_IS_STUDY = 0x00000008;
	public static final int FILTER_EFFECTIVEAREA = 0x00000010;
	public static final int FILTER_OVERLAY_INDEX = 0x00000020;
	public static final int FILTER_OVERLAY_RATIO = 0x00000040;
	public static final int FILTER_FILENAME = 0x00000080;
	public static final int FILTER_REGISTER_PERCENT = 0x00000100;
	public static final int FILTER_REGISTER_TIME = 0x00000200;
	public static final int FILTER_REGISTER_STATEGY = 0x00000400;
	public static final int FILTER_REGISTER_TEMPCOUNT = 0x00000800;
	public static final int FILTER_RECONGIZE_RATIO = 0x00001000;
	public static final int FILTER_PREOVERLAY_RATIO = 0x00002000;

	public static final int FILTER_SELECT_ALL = 0xFFFFFFFF;

	public static final int FILTER_REGISTER = FILTER_IS_BAD_IMAGE
			| FILTER_IS_DELETE | FILTER_IS_STITCH | FILTER_EFFECTIVEAREA
			| FILTER_OVERLAY_INDEX | FILTER_OVERLAY_RATIO | FILTER_FILENAME
			| FILTER_REGISTER_PERCENT | FILTER_REGISTER_TIME
			| FILTER_REGISTER_STATEGY | FILTER_REGISTER_TEMPCOUNT
			| FILTER_PREOVERLAY_RATIO;

	public static final int FILTER_RECOGNIZE = FILTER_IS_BAD_IMAGE
			| FILTER_IS_STUDY | FILTER_EFFECTIVEAREA | FILTER_FILENAME
			| FILTER_RECONGIZE_RATIO | FILTER_REGISTER_TIME
			| FILTER_REGISTER_PERCENT;

	public AlgoResult() {
	}

	@Override
	public String toString() {
		String log = "";
		return super.toString() + log;
	}

	public static boolean isMatchInfo(String str) {
		if (str == null) {
			return false;
		}
		if (str.startsWith("MATCH")) {
			return true;
		}
		return false;
	}
	public static boolean isFilePath(String str) {
		if (str == null) {
			return false;
		}
		if (str.startsWith("MATCH:") || str.startsWith("REG:")) {
			return true;
		}
		return false;
	}

	public static void paraString(String str, int filter, int index) {
		if (filter == FILTER_RECOGNIZE) {
			int i = 0;
			StringTokenizer nizer = new StringTokenizer(str.substring(index),
					",", false);
			while (nizer.hasMoreElements()) {
				sRecgnizeInfo[i] = nizer.nextToken();
				i++;

			}
		} else if (filter == FILTER_REGISTER) {
			int i = 0;
			StringTokenizer nizer = new StringTokenizer(str.substring(index),
					",", false);
			while (nizer.hasMoreElements()) {
				sRegisterInfo[i] = nizer.nextToken();
				//nizer.nextToken();
				i++;
			}
		}

	}
	
	private static String formatString(Context context,int resId,String str) {
		return String.format(context.getResources().getString(resId), str);
	}
	
	private static String formatString(Context context,int resId,int d) {
		return String.format(context.getResources().getString(resId), d);
	}
	
	private static String formatString(Context context,int resId,float f) {
		return String.format(context.getResources().getString(resId), f);
	}

	public static String filePath = null;

	public static String bulidLog(Context context, String str, int filter, int count) {
		if (str == null) {
			return null;
		}
		int filepathHead = 0;
		if (filter == FILTER_REGISTER) {
			filepathHead = 4;
		} else {
			filepathHead = 7;
		}
		StringBuilder mLogBuilder = new StringBuilder();

		if ((filter & FILTER_FILENAME) != 0) {
			if (isFilePath(str)) {
				mLogBuilder.append(str.substring(filepathHead));
				mLogBuilder.append("\n");
				mLogBuilder.append("--------------------------------\n");
				return mLogBuilder.toString();
			} else {

				paraString(str, filter, filepathHead);
			}
		}
		if ((filter & FILTER_IS_BAD_IMAGE) != 0) {
			if (filter == FILTER_RECOGNIZE) {
				mLogBuilder.append(formatString(context,R.string.algoresult_img_qulity,
						(sRecgnizeInfo[REC_PICTURE_QUALITY] != null && sRecgnizeInfo[REC_PICTURE_QUALITY].indexOf("0") != -1) ? "BAD\n": "GOOD\n"));
			} else if (filter == FILTER_REGISTER) {
				mLogBuilder.append(formatString(context,R.string.algoresult_img_qulity,
						(sRegisterInfo[REG_PICTURE_QUALITY] != null && sRegisterInfo[REC_PICTURE_QUALITY].indexOf("0") != -1) ? "BAD\n": "GOOD\n"));
			}
		}
		if ((filter & FILTER_EFFECTIVEAREA) != 0) {
			if (filter == FILTER_RECOGNIZE) {
				mLogBuilder.append(formatString(context,R.string.algoresult_effective_area,sRecgnizeInfo[REC_VALID_SIZE]) + "\n");
			} else {
				mLogBuilder.append(formatString(context,R.string.algoresult_effective_area,sRegisterInfo[REG_VALID_SIZE]) + "\n");
			}
		}
		if ((filter & FILTER_IS_STUDY) != 0) {
			mLogBuilder.append(context.getResources().getString(R.string.algoresult_is_study));
			for (int i = 0; i < count; i++) {
				mLogBuilder.append(sRecgnizeInfo[REC_STUDY1 + i]
						.startsWith("1") ? "YES" : "NO" + "   ");
			}
			mLogBuilder.append("\n");
		}

		if ((filter & FILTER_IS_DELETE) != 0) {
			mLogBuilder.append(formatString(context, R.string.algoresult_is_delete, sRegisterInfo[REG_STICK_INFO].startsWith("0") ? "FALSE"
							: "TRUE") + "\n");
		}
		if ((filter & FILTER_IS_STITCH) != 0) {
			mLogBuilder.append(formatString(context,R.string.algoresult_is_stitch,sRegisterInfo[REG_OVERBACK].startsWith("0") ? "FALSE"
					  : "TRUE")	 + "\n");
		}
		if ((filter & FILTER_REGISTER_PERCENT) != 0) {
			if (filter == FILTER_RECOGNIZE) {
				mLogBuilder.append(context.getResources().getString(R.string.algoresult_register_percent));
				for (int i = 0; i < count; i++) {
					mLogBuilder.append(sRecgnizeInfo[REC_MATCH_DEGREE1 + i]
							+ "%   ");
				}
			} else {
				mLogBuilder.append(context.getResources().getString(R.string.algoresult_register_percent));
				mLogBuilder.append(sRegisterInfo[REG_PROGRESS] + "%   ");

			}
			mLogBuilder.append("\n");
		}
		if ((filter & FILTER_REGISTER_TEMPCOUNT) != 0) {
			mLogBuilder.append(formatString(context, R.string.algoresult_temp_count, sRegisterInfo[REG_TEMPLATE_COUNT]) + "\n");
		}
		if ((filter & FILTER_OVERLAY_RATIO) != 0) {
			mLogBuilder.append(formatString(context, R.string.algoresult_overlay_ratio, sRegisterInfo[REG_OVERLAY_RATE]) + "%\n");
		}
		if ((filter & FILTER_OVERLAY_INDEX) != 0) {
			mLogBuilder.append(formatString(context, R.string.algoresult_overlay_index, sRegisterInfo[REG_OVERLAY_COUNT]) + "\n");
		}
		if ((filter & FILTER_REGISTER_TIME) != 0) {
			mLogBuilder.append(context.getResources().getString(R.string.algoresult_register_time));
			if (filter == FILTER_RECOGNIZE) {
				for (int i = 0; i < count; i++) {
					mLogBuilder.append(sRecgnizeInfo[REC_ALGL_TIME1 + i]
							+ "ms   ");
				}
			} else {
				mLogBuilder.append(sRegisterInfo[REG_ALGO_TIME] + "ms   ");
			}
			mLogBuilder.append("\n");
		}
		if ((filter & FILTER_PREOVERLAY_RATIO) != 0) {
			mLogBuilder.append(formatString(context, R.string.algoresult_preoverlay_ratio, sRegisterInfo[REG_UPOVERLAY_RATE].replace("\n", "")) + "% ");
		}
		mLogBuilder.append("\n");

		return mLogBuilder.toString();
	}

}
