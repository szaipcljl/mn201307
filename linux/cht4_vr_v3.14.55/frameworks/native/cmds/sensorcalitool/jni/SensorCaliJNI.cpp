
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <jni.h>
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include "SensorCaliJNI.h"
#include <log/log.h>
using namespace android;
#undef LOG_TAG
#define LOG_TAG "SensorCaliJNI"

#if 1
extern int SetAGM_STEP_A();
extern int SetAGM_STEP_B();
extern int SetAGM_STEP_C();
extern int SetAGM_STEP_D();
extern int SetAGM_STEP_E();
extern int SetAGM_STEP_F();
#endif

#if 1
 int SetAGM_STEP_A_test()
 {
	ALOGE("ERROR: SetAGM_STEP_A_test.\n");
	return 0;
 }
 int SetAGM_STEP_B_test()
 {
	ALOGE("ERROR: SetAGM_STEP_B_test.\n");
	return 0;
 }
 int SetAGM_STEP_C_test()
 {
	ALOGE("ERROR: SetAGM_STEP_C_test.\n");
	return 0;
 }
 int SetAGM_STEP_D_test()
 {
	ALOGE("ERROR: SetAGM_STEP_D_test.\n");
	return 0;
 }
 int SetAGM_STEP_E_test()
 {
	ALOGE("ERROR: SetAGM_STEP_E_test.\n");
	return 0;
 }
 int SetAGM_STEP_F_test()
 {
	ALOGE("ERROR: SetAGM_STEP_F_test.\n");
	return 0;
 }
#endif


JNIEXPORT jint JNICALL Java_SensorCaliJNI_setAGMStepA(JNIEnv *env, jobject obj, jint num)
{
	SetAGM_STEP_A();
	return 0;
}

JNIEXPORT jint JNICALL Java_SensorCaliJNI_setAGMStepB(JNIEnv *env, jobject obj, jint num)
{
	SetAGM_STEP_B();
	return 0;
}


JNIEXPORT jint JNICALL Java_SensorCaliJNI_setAGMStepC(JNIEnv *env, jobject obj, jint num)
{
	SetAGM_STEP_C();
	return 0;
}


JNIEXPORT jint JNICALL Java_SensorCaliJNI_setAGMStepD(JNIEnv *env, jobject obj, jint num)
{
	SetAGM_STEP_D();
	return 0;
}

JNIEXPORT jint JNICALL Java_SensorCaliJNI_setAGMStepE(JNIEnv *env, jobject obj, jint num)
{
	SetAGM_STEP_E();
	return 0;
}

JNIEXPORT jint JNICALL Java_SensorCaliJNI_setAGMStepF(JNIEnv *env, jobject obj, jint num)
{
	SetAGM_STEP_F();
	return 0;
}

static JNINativeMethod gMethods[] = {
    {"nativeSetAGM_STEP_A", "()I",       (void*)SetAGM_STEP_A_test},
	{"nativeSetAGM_STEP_B", "()I",       (void*)SetAGM_STEP_B_test},
	{"nativeSetAGM_STEP_C", "()I",       (void*)SetAGM_STEP_C_test},
	{"nativeSetAGM_STEP_D", "()I",       (void*)SetAGM_STEP_D_test},
	{"nativeSetAGM_STEP_E", "()I",       (void*)SetAGM_STEP_E_test},
	{"nativeSetAGM_STEP_F", "()I",       (void*)SetAGM_STEP_F_test}
};

jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env = NULL;
    jint result = -1;

    ALOGE("JNI_OnLoad in sensor hub.\n");
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


