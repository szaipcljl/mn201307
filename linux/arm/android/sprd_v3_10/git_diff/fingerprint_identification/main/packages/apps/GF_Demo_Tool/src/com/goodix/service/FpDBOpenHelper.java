package com.goodix.service;

import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.content.Context;

/**
 * <p>
 * Title: FpDBOpenHelper
 * </p>
 * <p>
 * Description:
 * </p>
 */
public class FpDBOpenHelper extends SQLiteOpenHelper
{
    
    public static final String DATABASE_NAME = "FingerprintDatabase.db";
    public static final String DATABASE_TABLE = "FingerprintInfo";
    public static final int DATABASE_VERSION = 1;
    public static final String KEY = "_id";
    public static final String NAME = "Name";
    public static final String DESCRIPTION = "Description";
    public static final String URI = "Uri";
    
    private static final String DATABASE_CREATE = "create table "
            + DATABASE_TABLE + "(" + KEY + " INTEGER," + NAME + " TEXT,"
            + DESCRIPTION + " TEXT," + URI + " TEXT)";
    
    public FpDBOpenHelper(Context context, String name,
            SQLiteDatabase.CursorFactory factory, int version)
    {
        super(context, name, factory, version);
    }
    
    @Override
    public void onCreate(SQLiteDatabase db)
    {
        db.execSQL(DATABASE_CREATE);
        
    }
    
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldvision, int newvision)
    {
        // TODO Auto-generated method stub
        db.execSQL("DROP TABLE IF EXISTS" + DATABASE_TABLE);
    }
    
}
