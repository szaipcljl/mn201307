package android.gxFP;

oneway interface IVerifyCallback 
{ 
	void handleMessage(int msg,int arg0,int arg1,inout byte[] data);
}
