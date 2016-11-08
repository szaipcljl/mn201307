#include "Sample1.h"
#include <string.h>
#include <ctype.h> //toupper()

#define BUILD_TWO_CPP_FILE 1 

char *strupr(char *str);
#if BUILD_TWO_CPP_FILE
extern int myprint();
#endif

char *strupr(char *str)
{
	char *orign=str;
	for (; *str!='\0'; str++)
		*str = toupper(*str);
	return orign;
}

JNIEXPORT jint JNICALL Java_Sample1_intMethod(JNIEnv *env, jobject obj, jint num)
{
#if BUILD_TWO_CPP_FILE
	myprint();
#endif
	return num * num;
}

JNIEXPORT jboolean JNICALL Java_Sample1_booleanMethod(JNIEnv *env, jobject obj, jboolean boolean)
{
	return !boolean;
}

JNIEXPORT jstring JNICALL Java_Sample1_stringMethod(JNIEnv *env, jobject obj, jstring string)
{
	const char* str = env->GetStringUTFChars(string, 0);
	char cap[128];

	strcpy(cap, str);
	env->ReleaseStringUTFChars(string, 0);
	return env->NewStringUTF(strupr(cap));
}

JNIEXPORT jint JNICALL Java_Sample1_intArrayMethod(JNIEnv *env, jobject obj, jintArray array)
{
	int i, sum = 0;
	jsize len = env->GetArrayLength(array);
	jint *body = env->GetIntArrayElements(array, 0);

	for (i = 0; i < len; ++i)
	{
		sum += body[i];
	}

	jint agm_data[9] = {1,2,3,4,5,6,7,8,9};

	if (len < 9)
		return -1;

	for (i = 0; i < len; ++i) { //change the array value
		body[i] = agm_data[i];
	}


	for (i = 0;i < len; i++) 
		printf("body[%d]:%d ", i, body[i]);
	printf("\n");

	env->ReleaseIntArrayElements(array, body, 0);

	return sum;
}
