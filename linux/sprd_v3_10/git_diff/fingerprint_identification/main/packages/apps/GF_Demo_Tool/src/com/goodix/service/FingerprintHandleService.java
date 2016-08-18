/**
 * <p>Title: ${FingerprintHandleService.java}</p>
 * <p>Description: fingerprint device manager service</p>
 * <p>Copyright: </p>
 * <p>Company: Goodix</p>
 * @author peng.hu
 * @date ${2014.3.8}
 * @version 1.0
 * @Function : 1.device control
 * 						 2.fingerprint capturing
 */

package com.goodix.service;

import java.util.ArrayList;
import com.goodix.util.Fingerprint;
import android.app.Service;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.os.Binder;
import android.os.IBinder;

public class FingerprintHandleService extends Service
{
    public static final int DATABASE_MAX_ITEM = 5; // Maximum number of fingerprint storage
    
    @Override
    public void onCreate()
    {
        super.onCreate();
    }
    
    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }
    
    @Override
    public boolean onUnbind(Intent intent)
    {
        return super.onUnbind(intent);
    }
    
    /**
     * <p>
     * Title: getDatabaseSpace
     * </p>
     * <p>
     * Description: get the remaining space size
     * </p>
     * 
     * @return
     */
    public int getDatabaseSpace()
    {
        // return DATABASE_MAX_ITEM - mDatabaseBCount;
        ContentResolver cr = this.getContentResolver();
        Cursor cursor = cr.query(FpDBContentProvider.CONTENT_URI, null, null,
                null, null);
        int count = cursor.getCount();
        cursor.close();
        return DATABASE_MAX_ITEM - count;
    }
    
    @Override
    public IBinder onBind(Intent arg0)
    {
        return new ServiceBinder();
    }
    
    /**
     * <p>
     * Title: query
     * </p>
     * <p>
     * Description:list the fingerprint data in the database
     * </p>
     * 
     * @return
     */
    public ArrayList<Fingerprint> query()
    {
        ArrayList<Fingerprint> list = new ArrayList<Fingerprint>();
        ContentResolver cr = this.getContentResolver();
        Cursor cursor;
        Fingerprint fp = null;
        
        String[] projection = null;
        String selection = null;
        String[] selectionArgs = null;
        String sortOrder = null;
        
        cursor = cr.query(FpDBContentProvider.CONTENT_URI, projection,
                selection, selectionArgs, sortOrder);
        int count = cursor.getCount();
        if (0 != count)
        {
            if (null != cursor)
            {
                cursor.moveToFirst();
                do
                {
                    fp = new Fingerprint(cursor.getInt(0), cursor.getString(1),
                            cursor.getString(2), cursor.getString(3));
                    list.add(fp);
                }
                while (cursor.moveToNext());
            }// very importance.
        }
        cursor.close();
        return list;
    }
    
    /**
     * <p>
     * Title: match
     * </p>
     * <p>
     * Description: From the database to find whether the matching fingerprints
     * </p>
     * 
     * @param path
     *            Matching of fingerprint data storage location
     * @return
     */
    public Fingerprint match(String path)
    {
        return null;
    }
    
    /**
     * <p>
     * Title: match
     * </p>
     * <p>
     * Description: matching fingerprint data
     * </p>
     * 
     * @param srcPath
     *            source finggerprint data path
     * @param dstPath
     *            Target fingerprint data path
     * @return true:match sucessfully false:match fail
     */
    public boolean match(String srcPath, String dstPath)
    {
        return true;
    }
    
    /**
     * <p>
     * Title: insert
     * </p>
     * <p>
     * Description: Insert a new fingerprint data
     * </p>
     * 
     * @param mFp
     * @return
     */
    public boolean insert(Fingerprint mFp)
    {
        ContentResolver cr = this.getContentResolver();
        ContentValues fpItem = new ContentValues();
        fpItem.put(FpDBOpenHelper.KEY, mFp.getKey());
        fpItem.put(FpDBOpenHelper.NAME, mFp.getName());
        fpItem.put(FpDBOpenHelper.DESCRIPTION, mFp.getDescription());
        fpItem.put(FpDBOpenHelper.URI, mFp.getUri());
        if (cr.insert(FpDBContentProvider.CONTENT_URI, fpItem) != null)
        {
            // this.mDatabaseBCount++;
            return true;
        }
        return false;
    }
    
    /**
     * <p>
     * Title: update
     * </p>
     * <p>
     * Description: Update the fingerprint database content
     * </p>
     * 
     * @param mFp
     * @return
     */
    public boolean update(Fingerprint mFp)
    {
        ContentResolver cr = this.getContentResolver();
        String[] selectionArgs = null;
        String where = FpDBOpenHelper.KEY + "=" + mFp.getKey();
        ContentValues values = new ContentValues();
        values.put(FpDBOpenHelper.KEY, mFp.getKey());
        values.put(FpDBOpenHelper.NAME, mFp.getName());
        values.put(FpDBOpenHelper.DESCRIPTION, mFp.getDescription());
        values.put(FpDBOpenHelper.URI, mFp.getUri());
        if (0 != cr.update(FpDBContentProvider.CONTENT_URI, values, where,
                selectionArgs))
            return true; // if return the update number is not 0 ,then data have been changed
        return false;
    }
    
    /**
     * <p>
     * Title: delete
     * </p>
     * <p>
     * Description:
     * </p>
     * 
     * @param mFp
     *            Delete the fingerprint data
     * @return true :Delete sucessfully  false:delete fail
     */
    public boolean delete(int mKey)
    {
        ContentResolver cr = this.getContentResolver();
        String[] selectionArgs = null;
        String where = FpDBOpenHelper.KEY + "=" + mKey;
        if (0 != cr.delete(FpDBContentProvider.CONTENT_URI, where,
                selectionArgs))
        {
            return true;
        }
        return false;
    }
    
    /**
     * <p>
     * Title: ServiceBinder
     * </p>
     * <p>
     * Description: instantiation service
     * </p>
     */
    public class ServiceBinder extends Binder
    {
        public FingerprintHandleService getService()
        {
            return FingerprintHandleService.this;
        }
    }
}
