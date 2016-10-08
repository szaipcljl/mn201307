#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <jni.h>
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <log/log.h>
#undef LOG_TAG
#define LOG_TAG "SensorCaliJNI"

using namespace android;

extern int point;
#if 1
extern int SetAGM_STEP_A();
extern int SetAGM_STEP_B();
extern int SetAGM_STEP_C();
extern int SetAGM_STEP_D();
extern int SetAGM_STEP_E();
extern int SetAGM_STEP_F();
extern int GetProcessPoint();
extern int ApkExit();
#endif


static JNINativeMethod gMethods[] = {
	{"nativeSetAGM_STEP_A", "()I",       (void*)SetAGM_STEP_A},
	{"nativeSetAGM_STEP_B", "()I",       (void*)SetAGM_STEP_B},
	{"nativeSetAGM_STEP_C", "()I",       (void*)SetAGM_STEP_C},
	{"nativeSetAGM_STEP_D", "()I",       (void*)SetAGM_STEP_D},
	{"nativeSetAGM_STEP_E", "()I",       (void*)SetAGM_STEP_E},
	{"nativeSetAGM_STEP_F", "()I",       (void*)SetAGM_STEP_F},
	{"nativeGetProcessPoint", "()I",     (void*)GetProcessPoint},
	{"nativeApkExit", "()I",     (void*)ApkExit}
};

jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
	JNIEnv* env = NULL;
	jint result = -1;

	ALOGD("JNI_OnLoad in sensor hub.\n");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		ALOGE("ERROR: GetEnv failed\n");
		return -1;
	}

	assert(env != NULL);
	if (AndroidRuntime::registerNativeMethods(env, "com/intel/sensorhub/SensorHubNativeInterface",gMethods, NELEM(gMethods)) >= 0) {
		result = JNI_VERSION_1_4;
	}

	return result;
}

