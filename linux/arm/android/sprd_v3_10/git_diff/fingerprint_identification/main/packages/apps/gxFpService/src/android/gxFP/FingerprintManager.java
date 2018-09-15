package android.gxFP;

import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.gxFP.IEnrollCallback;
import android.gxFP.IFingerprintManager;
import android.gxFP.IVerifyCallback;
import android.content.Context;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import android.os.DeadObjectException;

public class FingerprintManager
{
    private static final String TAG = "FpSetting";
  //  private Context mContext;
    
    /*
     * @Fields mService : remote fingerprint manager service .
     */
    private IFingerprintManager mService;
    private static Method getService;
	public static FingerprintManager getFpManager() {
		FingerprintManager FpManager=null;
			try {
			getService = Class.forName("android.os.ServiceManager").getMethod("getService", String.class);
			FpManager=new FingerprintManager(IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService")));
			}catch(ClassNotFoundException ex){
			Log.v("fpservice", "ClassNotFoundException");
			}catch(NoSuchMethodException ex){
			Log.v("fpservice", "NoSuchMethodException");
			}catch(IllegalAccessException ex){
			Log.v("fpservice", "IllegalAccessException");
			}catch(InvocationTargetException ex){
			Log.v("fpservice", "InvocationTargetException");
			}
		return FpManager;
	}

    public FingerprintManager(IFingerprintManager service)
    {
        mService = service;
   //     mContext = context;
    }
    /*
     * @Title: query   
     * @Description: get fingerprint items status 
     * @param @return
     * @return int : fingerprint items status,first 16bit is count,next 16bit is the flag
     * that fingerprint has data or not.
     * @throws 
     */
    public int query()
    {
        try
        {
            return mService.query();
        }
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
        return 0;
    }
    
    public void setFFmod(boolean mod)
    {
        try
        {
            mService.setFFmod(mod);
        }
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
    }
    /*
     * @Title: delete   
     * @Description: delete fingerprint item.
     * @param @param i : index of item
     * @param @return
     * @return int : error code,like bad param , no permission .
     * @throws 
     */
    public int delete(int i)
    {
        try
        {
            return mService.delete(i);
        }
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
        return 0;
    }
    
    /*
     * 
     *  get information about fingerFrint
     *
     */
    public String getInformation() {
        try
        {
            return mService.getInfo();
        }
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
        return null;
    }
	
    public boolean checkPassword(String psw)
    {
        if (null != psw)
        {
            return psw.equals("1234");
        }
        return false;
    }
    
    public boolean changePassword(String oldPassword,String newPassword)
    {
        return true;
    }
    
    /*
     * @Title: ewVerifySession   
     * @Description: verify session
     * @param @param callback
     * @param @return Verify session object
     * @return VerifySession 
     * @throws 
     */
    public VerifySession newVerifySession(IVerifyCallback callback)
    {
        return new VerifySession(callback);
    }
    
    
    /*
     * @Title: newEnrollSession   
     * @Description: get a new Enroll session object
     * @param @param callback 
     * @param @return
     * @return EnrollSession : Enroll session object
     * @throws 
     */
    public EnrollSession newEnrollSession(IEnrollCallback callback)
    {
        return new EnrollSession(callback);
    }
    
    

    /*
     * <p>Title: FingerprintManager.java</p>
     * <p>Description: Verify Session</p>
     */
    public final class VerifySession {
        
        
        /*
         * @Fields mToken : session's ID , difference other session.
         */
        private IBinder mToken;
        /*
         * @Fields mCallback : FingerprintManagerService's message handler.
         */
        private IVerifyCallback mCallback;
        
        public VerifySession(IVerifyCallback callback) 
        {
            Log.v(TAG,"new VerifySession.");
            mCallback = callback;
            mToken = new Binder();
        }
        
        
        /*
         * @Title: enter   
         * @Description: session begin
         * @param @return
         * @return int errcode.
         * @throws 
         */
        public int enter() {
            try 
            {
                Log.v(TAG,"verify session enter.");
                return mService.verify(mToken, mCallback);
            } 
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
            return -1;
        }
        
        
        /*
         * @Title: exit   
         * @Description: session exit
         * @param @return
         * @return int 
         * @throws 
         */
        public int exit() {
            try 
            {
                Log.v(TAG,"verify session exit.");
                return mService.cancelVerify(mToken);
            } 
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
            return -1; 
        }
    }
    

    public final class EnrollSession {
        private IBinder mToken;
        private IEnrollCallback mCallback;
        public EnrollSession(IEnrollCallback callback) 
        {
            mCallback = callback;
            mToken = new Binder();
        }

        public int enter() {
            try 
            {
                Log.v(TAG,"new enroll session.");
                return mService.enroll(mToken, mCallback);
            } 
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
            return -1;
        }

        public int exit() {
            try 
            {
                Log.v(TAG,"enroll session exit.");
                return mService.cancelEnroll(mToken);
            } 
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
            return -1;
        }
        
        public int reset() 
        {
            try 
            {
                Log.v(TAG,"enroll session reset.");
                return mService.resetEnroll(mToken);
            } 
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
            return -1;
        }
        
        public int save(int index)
        {
            try
            {
                Log.v(TAG,"enroll session save.");
                return mService.saveEnroll(mToken,index);
            }
	catch (DeadObjectException ex)
	{
	    Log.v("gxfpservice", "try get new service stub");
	    try 
            {
	        mService=IFingerprintManager.Stub.asInterface((IBinder)getService.invoke(new Object(), "gxFpService"));
	    } 
		catch(IllegalAccessException e){
		Log.v("fpservice", "IllegalAccessException");
		}catch(InvocationTargetException e){
		Log.v("fpservice", "InvocationTargetException");
		}
	}
        catch (RemoteException e)
        {
            e.printStackTrace();
        }
            return -1;
        }
    }
    
}
