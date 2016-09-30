#include <stdio.h> 
#include "TestJNI.h" 

jobject  gJavaObj;
JNIEnv *gEnv;
jmethodID javaCallback;

static void native_callback(int count)
{
	//回调Java层的函数
	(*gEnv)->CallVoidMethod(gEnv, gJavaObj, javaCallback, count);

}

JNIEXPORT int JNICALL Java_TestJNI_nativeCallbackInitilize(JNIEnv *env, jobject obj)
{

	//直接通过定义全局的JNIEnv和jobject变量，在此保存env和obj的值是不可以在线程中使用的
	//线程不允许共用env环境变量，但是JavaVM指针是整个jvm共用的，所以可以通过下面的方法保存JavaVM指针，在线程中使用
	JavaVM * gJavaVM;

	(*env)->GetJavaVM(env,&gJavaVM);

	//同理，jobject变量也不允许在线程中共用，因此需要创建全局的jobject对象在线程中访问该对象
	gJavaObj = (*env)->NewGlobalRef(env,obj);


	//从全局的JavaVM中获取到环境变量
	(*gJavaVM)->AttachCurrentThread(gJavaVM,&gEnv, NULL);

	//获取Java层对应的类
	jclass javaClass = (*gEnv)->GetObjectClass(gEnv,gJavaObj);
	if( javaClass == NULL ) {
		printf("Fail to find javaClass");
		return -1;
	}

	//获取Java层被回调的函数
	javaCallback = (*gEnv)->GetMethodID(gEnv,javaClass,"onNativeCallback","(I)V");
	if( javaCallback == NULL) {
		printf("Fail to find method onNativeCallback\n");
		return -2;
	}

	return 0;
}

int i=0; 
JNIEXPORT void JNICALL Java_TestJNI_set (JNIEnv * env, jobject obj, jint j) 
{ 
	i=j*888; 
	native_callback(2*i - 1000);
}   

JNIEXPORT jint JNICALL Java_TestJNI_get (JNIEnv * env, jobject obj) 
{
	printf("ok!You have successfully passed the Java call c\n");
	return i; 
}  
