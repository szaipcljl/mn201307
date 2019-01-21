package com.goodix.service;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.content.UriMatcher;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteQueryBuilder;

public class FpDBContentProvider extends ContentProvider
{
    public static final Uri CONTENT_URI = Uri
            .parse("content://com.goodix.service.fpdbcontentprovider/elements");
    private static FpDBOpenHelper fpDBHelper = null;
    private static final int ALLROWS = 1;
    private static final int SINGLEROW = 2;
    private static final UriMatcher uriMatcher;
    
    static
    {
        uriMatcher = new UriMatcher(UriMatcher.NO_MATCH);
        uriMatcher.addURI("com.goodix.service.fpdbcontentprovider",
                "elements", ALLROWS);
        uriMatcher.addURI("com.goodix.service.fpdbcontentprovider",
                "elements/#", SINGLEROW);
    }
    
    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db;
        try
        {
            db = fpDBHelper.getWritableDatabase();
        }
        catch (SQLiteException ex)
        {
            db = fpDBHelper.getReadableDatabase();
        }
        
        switch (uriMatcher.match(uri))
        {
            case SINGLEROW:
            {
                /*
                 * String rowID = uri.getPathSegments().get(1);
                 * 
                 * selection = FpDBOpenHelper.KEY + "=" + rowID +
                 * (!TextUtils.isEmpty(selection)?"AND (" + selection + ')':"");
                 */
            }
                break;
            
            default:
                break;
        }
        
        if (selection == null)
        {
            selection = "1"; // while(1) delete all items
        }
        
        int deleteCount = db.delete(FpDBOpenHelper.DATABASE_TABLE, selection,
                selectionArgs);
        super.getContext().getContentResolver().notifyChange(uri, null);
        return deleteCount;
    }
    
    @Override
    public String getType(Uri uri)
    {
        switch (uriMatcher.match(uri))
        {
            case ALLROWS:
                return "vnd.android.cursor.dir/vnd.goodix.elemental";
                
            case SINGLEROW:
                return "vnd.android.cursor.item/vnd.goodix.elemental";
                
            default:
                throw new IllegalArgumentException("Unsupported URI : " + uri);
        }
    }
    
    @Override
    public Uri insert(Uri uri, ContentValues values)
    {
        SQLiteDatabase db = fpDBHelper.getWritableDatabase();
        
        String nullColumnHack = null;
        long id = db.insert(FpDBOpenHelper.DATABASE_TABLE, nullColumnHack,
                values);
        
        if (id > -1) // success
        {
            Uri insertedID = ContentUris.withAppendedId(CONTENT_URI, id);
            super.getContext().getContentResolver()
                    .notifyChange(insertedID, null);
            return insertedID;
        }
        else
        // failed
        {
            return null;
        }
    }
    
    @Override
    public boolean onCreate()
    {
        fpDBHelper = new FpDBOpenHelper(getContext(),
                FpDBOpenHelper.DATABASE_NAME, null,
                FpDBOpenHelper.DATABASE_VERSION);
        return true;
    }
    
    @Override
    public Cursor query(Uri uri, String[] projection, String selection,
            String[] selectionArgs, String sortOrder)
    {
        
        SQLiteDatabase db;
        try
        {
            db = fpDBHelper.getWritableDatabase();
        }
        catch (SQLiteException ex)
        {
            db = fpDBHelper.getReadableDatabase();
        }
        
        String groupBy = null;
        String having = null;
        
        SQLiteQueryBuilder queryBuilder = new SQLiteQueryBuilder();
        
        switch (uriMatcher.match(uri))
        {
            case ALLROWS:
                
                break;
            
            case SINGLEROW:
                // String rowID = uri.getPathSegments().get(1);
                // queryBuilder.appendWhere(FpDBOpenHelper.KEY + " = " + rowID);
                break;
            
            default:
                break;
        
        }
        
        queryBuilder.setTables(FpDBOpenHelper.DATABASE_TABLE);
        Cursor cursor = queryBuilder.query(db, projection, selection,
                selectionArgs, groupBy, having, sortOrder);
        
        return cursor;
    }
    
    @Override
    public int update(Uri uri, ContentValues values, String selection,
            String[] selectionArgs)
    {
        SQLiteDatabase db = fpDBHelper.getWritableDatabase();
        switch (uriMatcher.match(uri))
        {
            case SINGLEROW:
            {
                // String rowID = uri.getPathSegments().get(1);
                
                // selection = FpDBOpenHelper.KEY + "=" + rowID +
                // (!TextUtils.isEmpty(selection)?"AND (" + selection + ')':"");
            }
                break;
            
            default:
                break;
        }
        
        int updateCount = db.update(FpDBOpenHelper.DATABASE_TABLE, values,
                selection, selectionArgs);
        super.getContext().getContentResolver().notifyChange(uri, null);
        return updateCount;
    }
    
}
