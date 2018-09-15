package android.gxFP;

import android.gxFP.IEnrollCallback;
import android.gxFP.IVerifyCallback;

interface IFingerprintManager 
{ 
void setFFmod(boolean mod);
	/*recognize*/
    int verify(IBinder token,IVerifyCallback callback);
    int cancelVerify(IBinder token);
    
    /*register*/
    int enroll(IBinder token,IEnrollCallback callback);
    int resetEnroll(IBinder token);
    int cancelEnroll(IBinder token);
    int saveEnroll(IBinder token,int index);
    
    /* fingerprint data*/
    int query();
    int delete(int index);
    
    /*password*/
    // int checkPassword(String password);
    // int changePassword(String oldPassword,String newPassword);
    
    /*engtest such as mode set*/
   // int EngTest(int cmd);
    
    /*get informaton about fingerFrint*/  
    String getInfo();
}
