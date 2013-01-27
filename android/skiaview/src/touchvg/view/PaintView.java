//! \file PaintView.java
//! \brief 定义Android绘图视图类 PaintView
// License: LGPL, https://github.com/rhcad/touchvg

package touchvg.view;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.HashSet;
import java.util.Set;

import touchvg.jni.GiColor;
import touchvg.jni.GiCoreView;
import touchvg.jni.GiGestureState;
import touchvg.jni.GiGestureType;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

//! Android绘图视图类
/** \ingroup GRAPH_SKIA
 *  \see MyPaintView, GiCoreView, GiCanvasEx
 */
public class PaintView extends View {
    private GiCoreView mCore;
    private GiCanvasEx mCanvas;
    private Context mContext;
    private GestureDetector mDetector;
    private GiGestureState gestureState;
    private GiGestureType gestureType;
    private int fingerCount;
    private float lastX = 1;
    private float lastY = 1;
    private float lastX1 = 1;
    private float lastY1 = 1;
    private boolean isMoving = false;
    private boolean mGestureEnable = true;
    private int mBkColor = Color.TRANSPARENT;
    private int mBmpIds[] = { 0, 0, 0, 0, 0 };
    private Bitmap mBmps[] = { null, null, null, null, null };
    private final PaintView mView = this;
    public String mFilePath = "mnt/sdcard/touchVG";
    private Set<String> nameCache = new HashSet<String>();

    static {
        System.loadLibrary("skiaview");
    }

    public PaintView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.mContext = context;
        init();
    }

    public PaintView(Context context) {
        super(context);
        this.mContext = context;
        init();
    }

    private void init() {
        DisplayMetrics dm = mContext.getApplicationContext().getResources()
                .getDisplayMetrics();
        GiCanvasEx.setScreenDpi(dm.densityDpi);

        this.mCanvas = new GiCanvasEx(this);
        mCore = new GiCoreView(getCanvas());
        mDetector = new GestureDetector(mContext, new PaintGestureDetector());

        setDrawingCacheEnabled(true); // for getCanvasBitmap
        buildDrawingCache();

        this.setOnTouchListener(new OnTouchListener() {

            public boolean onTouch(View v, MotionEvent event) {
                if (!mGestureEnable) {
                    return false;
                }
                mView.getParent().requestDisallowInterceptTouchEvent(true);

                switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    fingerCount = event.getPointerCount();
                    lastX = event.getX(0);
                    lastY = event.getY(0);
                    if (fingerCount > 1) {
                        lastX1 = event.getX(1);
                        lastY1 = event.getY(1);
                    } // call mCore.onGesture in onTouch() after later.
                    break;
                case MotionEvent.ACTION_MOVE:

                    if (event.getPointerCount() == 2 && fingerCount == 1) {
                        fingerCount = event.getPointerCount();
                        lastX = event.getX(0);
                        lastY = event.getY(0);
                        lastX1 = event.getX(1);
                        lastY1 = event.getY(1);
                        gestureState = GiGestureState.kGestureBegan;
                    }
                    if (gestureState == GiGestureState.kGestureBegan) {
                        gestureType = fingerCount > 1 ? GiGestureType.kZoomRotatePan
                                : GiGestureType.kSinglePan;
                        mCore.onGesture(gestureType, gestureState, fingerCount,
                                lastX, lastY, lastX1, lastY1);
                    }
                    gestureState = GiGestureState.kGestureMoved;
                    isMoving = true;
                    lastX = event.getX(0);
                    lastY = event.getY(0);
                    if (event.getPointerCount() > 1) {
                        lastX1 = event.getX(1);
                        lastY1 = event.getY(1);
                    }
                    if (event.getPointerCount() >= fingerCount) {
                        mCore.onGesture(gestureType, gestureState, fingerCount,
                                lastX, lastY, lastX1, lastY1);
                    }
                    break;

                case MotionEvent.ACTION_UP:
                    if (isMoving) {
                        isMoving = false;
                        gestureState = GiGestureState.kGestureEnded;
                        mCore.onGesture(gestureType, gestureState, fingerCount,
                                lastX, lastY, lastX1, lastY1);
                        gestureType = GiGestureType.kGestureUnknown;
                        fingerCount = 0;
                    }
                    break;
                }
                return mDetector.onTouchEvent(event);
            }
        });
    }

    public GiCoreView getCoreView() {
        return mCore;
    }

    public void setBkColor(int argb) {
        mBkColor = argb;
        this.setBackgroundColor(argb);

        GiColor color = new GiColor();
        color.setARGB(mBkColor);
        getCanvas().setBkColor(color);
    }

    protected void onDraw(Canvas canvas) {
        mCore.onSize(this.getWidth(), this.getHeight());
        if (getCanvas().beginPaint(canvas)) {
            canvas.drawColor(mBkColor);
            mCore.onDraw(getCanvas());
            mCore.onDynDraw(getCanvas());
            getCanvas().endPaint();
        }
    }

    public void setGestureEnable(boolean enable) {
        mGestureEnable = enable;
        if (!enable && gestureState == GiGestureState.kGestureMoved) {
            gestureState = GiGestureState.kGestureCancel;
            mCore.onGesture(gestureType, gestureState, 0, 0, 0, 0, 0);
        }
    }

    //! 手势识别器
    private class PaintGestureDetector extends
            GestureDetector.SimpleOnGestureListener {
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            mCore.onGesture(GiGestureType.kSingleTap,
                    GiGestureState.kGestureBegan, 1, e.getX(), e.getY(), 0, 0);
            return false;
        }

        @Override
        public void onLongPress(MotionEvent e) {
            mCore.onGesture(GiGestureType.kLongPress,
                    GiGestureState.kGestureBegan, 1, e.getX(), e.getY(), 0, 0);
        }

        @Override
        public boolean onDown(MotionEvent e) {
            gestureState = GiGestureState.kGestureBegan;

//          fingerCount = e.getPointerCount();
//          lastX = e.getX(0);
//          lastY = e.getY(0);
//          if (fingerCount > 1) {
//              lastX1 = e.getX(1);
//              lastY1 = e.getY(1);
//          } // call mCore.onGesture in onTouch() after later.
            return true;
        }

        @Override
        public boolean onDoubleTap(MotionEvent e) {
            mCore.onGesture(GiGestureType.kDoubleTap,
                    GiGestureState.kGestureBegan, 1, e.getX(), e.getY(), 0, 0);
            return false;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float dx,
                float dy) {
            if (e2.getAction() == MotionEvent.ACTION_CANCEL) {
                gestureState = GiGestureState.kGestureCancel;
                mCore.onGesture(gestureType, gestureState, 0, 0, 0, 0, 0);
            }
            return false;
        }
    }

    public byte[] getCanvasBitmap() {
        Bitmap bitmap = getDrawingCache();
        if (bitmap == null) {
            return null;
        }
        final ByteArrayOutputStream os = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, os);
        return os.toByteArray();
    }

    public void setBitmapIDs(int vgdot1, int vgdot2) {
        mBmpIds[0] = vgdot1;
        mBmpIds[1] = vgdot2;
    }

    public Bitmap getHandleBitmap(int index) {
        int id = index < mBmpIds.length ? mBmpIds[index] : 0;
        if (id != 0) {
            if (mBmps[index] == null) {
                mBmps[index] = ((BitmapDrawable) mView.getResources()
                        .getDrawable(id)).getBitmap();
            }
            return mBmps[index];
        }
        return null;
    }

    public Bitmap getImageBitmap(String name) {
        String filename = this.mFilePath + File.separatorChar + name;
        if (new File(filename).exists()) {
            Bitmap bitmap = BitmapFactory.decodeFile(filename);
            if (bitmap != null) {
                nameCache.add(name);
                return bitmap;
            }
        }
        Integer picID = Integer.valueOf(name);
        if (picID != 0) {
            Bitmap bitmap = ((BitmapDrawable) mView.getResources()
                .getDrawable(picID)).getBitmap();
            if (bitmap != null) {
                nameCache.add(name);
                return bitmap;
            }
        }

        return null;
    }

    public void clearAllUnusedFiles()
    {
        for (String name : nameCache) {
            if (!this.getCoreView().findShapeByImageId(name)) {
                String filename = this.mFilePath + File.separatorChar + name;
                if (new File(filename).exists()) {
                    new File(filename).delete();
                }
            }
        }
        nameCache.clear();
    }

    public void clearAllImageFiles()
    {
        for (String name : nameCache) {
            String filename = this.mFilePath + File.separatorChar + name;
            if (new File(filename).exists()) {
                new File(filename).delete();
            }
        }
        nameCache.clear();
    }

    public GiCanvasEx getCanvas() {
        return mCanvas;
    }

    public String getFilePath() {
        return mFilePath;
    }

    public void setFilePath(String filePath) {
        this.mFilePath = filePath;
    }
}
