#include <stdio.h> 
#include "TestJNI.h" 

int i=0; 
JNIEXPORT void JNICALL Java_TestJNI_set (JNIEnv * env, jobject obj, jint j) 
{ 
	i=j*888; 
}   

JNIEXPORT jint JNICALL Java_TestJNI_get (JNIEnv * env, jobject obj) 
{
	printf("ok!You have successfully passed the Java call c\n");
	return i; 
}  
