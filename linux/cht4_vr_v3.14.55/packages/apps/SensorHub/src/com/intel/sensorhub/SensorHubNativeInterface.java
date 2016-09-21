package com.intel.sensorhub;

public class SensorHubNativeInterface {
	public native int nativeSetAGM_STEP_A();

	public native int nativeSetAGM_STEP_B();

	public native int nativeSetAGM_STEP_C();

	public native int nativeSetAGM_STEP_D();

	public native int nativeSetAGM_STEP_E();

	public native int nativeSetAGM_STEP_F();

	static {
		//System.load("system/lib/sensorcalitool.so");
		System.loadLibrary("sensorcalitool");
	}

	public int SetAGM_STEP_A() {
		return nativeSetAGM_STEP_A();
	}

	public int SetAGM_STEP_B() {
		return nativeSetAGM_STEP_B();
	}

	public int SetAGM_STEP_C() {
		return nativeSetAGM_STEP_C();
	}

	public int SetAGM_STEP_D() {
		return nativeSetAGM_STEP_D();
	}

	public int SetAGM_STEP_E() {
		return nativeSetAGM_STEP_E();
	}

	private int SetAGM_STEP_F() {
		return nativeSetAGM_STEP_F();
	}

}
