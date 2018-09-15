package com.goodix.widget;

import java.lang.ref.WeakReference;
import java.nio.IntBuffer;
import java.util.Random;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;

import com.goodix.fpsetting.R;

public class FingerprintProgressBar extends View
{
    private static final int LINE_WIDTH = 5;
    private static final int LINE_LENGTH = 20;
    private static final int PROGRESS_WIDTH = 2;
    private static final int PROGRESS_INTERVAL = 20;
    private static final int PGOGRESS_LOC = 10;
    
    private Bitmap mFingerprintImage;
    private Bitmap mFingerprintMask;
    private int mBitmapWidth = 0;
    private int mBitmapHeight = 0;
    private int mOnePercentWidth = 0;
    private int mOnePercentHeight = 0;
    private int mTextColor;
    private int mTextSize;
    private int mPreProgress;
    private int mProgress;
    private Paint mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private long mTickDuration = 80;
    
    private Runnable mPreAnimRunnable;
    private Handler mPreAnimHandler;
    private int mAnimPercent;
    
    private int[] mRandom = new int[100];
    
    private boolean bShowSlide = false;
    
    public FingerprintProgressBar(Context context)
    {
        super(context);
    }
    
    public FingerprintProgressBar(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        initView();
    }
    
    public FingerprintProgressBar(Context context, AttributeSet attrs,
            int defStyle)
    {
        super(context, attrs, defStyle);
        initView();
    }
    
    public void setSlideVisible(boolean bVisible)
    {
        this.bShowSlide = bVisible;
        invalidate();
    }
    
    public void setTextColor(int color)
    {
        
    }
    
    public int getTextColor()
    {
        return mTextColor;
    }
    
    public void setTextSize(int color)
    {
        
    }
    
    public int getTextSize()
    {
        return mTextSize;
    }
    
    public int getInterval(int progress)
    {
        return progress - mPreProgress;
    }
    
    public void setProgress(int progress)
    {
        if (progress < 0 )
            return;
        else if ( progress > 100)
        {
            progress = 100;
        }
        if (progress != mProgress)
        {
            mPreProgress = mProgress;
            mProgress = progress;
            mPreAnimHandler.postDelayed(mPreAnimRunnable, mTickDuration);
        }
    }
    
    private void getRandom()
    {
        Random random = new Random();
        int a = 0;
        int b = 0;
        int temp = 0;
        int i = 0;
        for (i = 0; i < 100; i++)
        {
            mRandom[i] = i;
        }
        for (i = 0; i < 300; i++)
        {
            a = random.nextInt(99);
            b = random.nextInt(99);
            if (a != b)
            {
                temp = mRandom[b];
                mRandom[b] = mRandom[a];
                mRandom[a] = temp;
            }
        }
    }
    
    private void changeBitmapAlpha(int color, Bitmap image, int x, int y,
            int w, int h)
    {
    	if (image == null || image.isRecycled()) {
    		return;
    	}
        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                int src = image.getPixel(x + i, y + j);
                int dst = (color & 0xFF000000) | (src & 0x00FFFFFF);
                image.setPixel(x + i, y + j, dst);
            }
        }
    }
    
    private void initView()
    {
        Resources res = getResources();
        mFingerprintImage = decodeResource(res,
                R.drawable.fingerprint_progress_bar);
        mBitmapWidth = mFingerprintImage.getWidth();
        mBitmapHeight = mFingerprintImage.getHeight();
        IntBuffer buffer = IntBuffer.allocate(mBitmapWidth * mBitmapHeight);
        mFingerprintMask = Bitmap.createBitmap(mBitmapWidth, mBitmapHeight,
                Bitmap.Config.ARGB_8888);
        Bitmap temp = decodeResource(res, R.drawable.fingerprint_progress_mask);
        
        for (int i = 0; i < mBitmapHeight; i++)
        {
            for (int j = 0; j < mBitmapWidth; j++)
            {
                buffer.array()[i * mBitmapWidth + j] = temp.getPixel(j, i);
            }
        }
        
        mFingerprintMask.copyPixelsFromBuffer(buffer);
        mOnePercentWidth = mBitmapWidth / 10;
        mOnePercentHeight = mBitmapHeight / 10;
        
        if (mBitmapWidth % 10 != 0)
        {
            mOnePercentWidth++;
        }
        if (mBitmapHeight % 10 != 0)
        {
            mOnePercentHeight++;
        }
        
        mPreProgress = 0;
        mProgress = 0;
        mTextColor = 0xFF000000;
        getRandom();
        
        this.mPreAnimRunnable = new PreAnimationRunnable(this);
        this.mPreAnimHandler = new Handler();
    }
    
    
    @Override
    protected void onDetachedFromWindow() {
    	// TODO Auto-generated method stub
    	super.onDetachedFromWindow();
    	if(mFingerprintMask != null) {
    		mFingerprintMask.recycle();
    		mFingerprintMask = null;
    	}
    	if (mFingerprintImage != null) {
    		mFingerprintImage.recycle();
    		mFingerprintImage = null;
    	}
    }
    
    
    private Bitmap decodeResource(Resources resources, int id)
    {
        TypedValue value = new TypedValue();
        resources.openRawResource(id, value);
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inMutable = true;
        opts.inPreferredConfig = Bitmap.Config.ARGB_8888;
        opts.inTargetDensity = value.density;
        opts.inScaled = false;
        return BitmapFactory.decodeResource(resources, id, opts);
    }
    
    @Override
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        
        int widgetWidth = this.getWidth();
        int widgetHeight = this.getHeight();
        mPaint.setStyle(Style.STROKE);
        mPaint.setStrokeWidth(LINE_WIDTH);
        mPaint.setColor(Color.BLACK);
        mPaint.setAntiAlias(true);
        
        if (bShowSlide == true)
        {
            canvas.drawLine(0, 0, LINE_LENGTH, 0, mPaint);
            canvas.drawLine(0, 0, 0, LINE_LENGTH, mPaint);
            canvas.drawLine(widgetWidth - LINE_LENGTH, 0, widgetWidth, 0,
                    mPaint);
            canvas.drawLine(widgetWidth, 0, widgetWidth, LINE_LENGTH, mPaint);
            canvas.drawLine(widgetWidth - LINE_LENGTH, widgetHeight,
                    widgetWidth, widgetHeight, mPaint);
            canvas.drawLine(widgetWidth, widgetHeight - LINE_LENGTH,
                    widgetWidth, widgetHeight, mPaint);
            canvas.drawLine(0, widgetHeight - LINE_LENGTH, 0, widgetHeight,
                    mPaint);
            canvas.drawLine(0, widgetHeight, LINE_LENGTH, widgetHeight, mPaint);
        }
        
        float left = (widgetWidth - mBitmapWidth) / 2;
        float top = (widgetHeight - mBitmapHeight) / 2;
        canvas.drawBitmap(mFingerprintImage, left, top, mPaint);
        canvas.drawBitmap(mFingerprintMask, left, top, mPaint);
        
        mPaint.setColor(Color.BLACK);
        mPaint.setStrokeWidth(40);
        canvas.drawLine(left, top + mBitmapHeight, left + mBitmapWidth, top
                + mBitmapHeight, mPaint);
        
        mPaint.setStrokeWidth(PROGRESS_WIDTH);
        mPaint.setColor(Color.GRAY);
        canvas.drawLine(left + PROGRESS_INTERVAL, top + mBitmapHeight
                + PGOGRESS_LOC, left + mBitmapWidth - PROGRESS_INTERVAL, top
                + mBitmapHeight + PGOGRESS_LOC, mPaint);
        
        mPaint.setColor(Color.WHITE);
        canvas.drawLine(
                left + PROGRESS_INTERVAL,
                top + mBitmapHeight + PGOGRESS_LOC,
                left
                        + PROGRESS_INTERVAL
                        + (((mBitmapWidth - PROGRESS_INTERVAL * 2) * mAnimPercent) / 100),
                top + mBitmapHeight + PGOGRESS_LOC, mPaint);
    }
    
    private class PreAnimationRunnable implements Runnable
    {
        private WeakReference<View> mViewReference;
        
        public PreAnimationRunnable(View view)
        {
            mViewReference = new WeakReference<View>(view);
        }
        
        @Override
        public void run()
        {
            FingerprintProgressBar view = (FingerprintProgressBar) mViewReference
                    .get();
            if (null != view)
            {
                if (view.mAnimPercent < view.mProgress)
                {
                    int left = (mRandom[mAnimPercent] % 10) * mOnePercentWidth;
                    int top = (mRandom[mAnimPercent] / 10) * mOnePercentHeight;
                    int width = mOnePercentWidth;
                    int height = mOnePercentHeight;
                    changeBitmapAlpha(0x00000000, mFingerprintMask, left, top,
                            width, height);
                    
                    view.mAnimPercent++;
                    mPreAnimHandler.postDelayed(this, view.mTickDuration);
                    view.invalidate();
                }
                else if (view.mAnimPercent > view.mProgress)
                {
                    view.mAnimPercent--;
                    int left = (mRandom[mAnimPercent] % 10) * mOnePercentWidth;
                    int top = (mRandom[mAnimPercent] / 10) * mOnePercentHeight;
                    int width = mOnePercentWidth;
                    int height = mOnePercentHeight;
                    changeBitmapAlpha(0xFF000000, mFingerprintMask, left, top,
                            width, height);
                    mPreAnimHandler.postDelayed(this, view.mTickDuration);
                    view.invalidate();
                }
                // else
                // {
                // mProgress = 0;
                // mPreAnimHandler.postDelayed(this, view.mTickDuration);
                // view.invalidate();
                // }
            }
        }
    }
}
