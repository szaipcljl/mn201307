package android.gxFP;

oneway interface IEnrollCallback 
{ 
    void handleMessage(int msg,int arg0,int arg1,inout byte[] data);
}
