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
import touchvg.jni.Ints;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;

//! Android绘图视图类
/*! \ingroup GRAPH_SKIA
    \see MyPaintView, GiCoreView, GiCanvasEx
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
    private Set < String> nameCache = new HashSet < String>();
    private RelativeLayout mButtonLayout = null;
    private int mImageIDs[] = new int[]{};

    static {
        System.loadLibrary("graph2d");
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

                    if (mButtonLayout != null) {
                        removeButtonLayout(mButtonLayout);
                        mButtonLayout = null;
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

    private void removeButtonLayout(RelativeLayout buttonLayout)
    {
        ViewGroup f = (ViewGroup) PaintView.this.getParent();
        if (buttonLayout != null) {
        	f.removeView(buttonLayout);
        }
    }

    private void addButtonLayout(RelativeLayout buttonLayout)
    {
        ViewGroup f = (ViewGroup) PaintView.this.getParent();
        f.addView(buttonLayout);
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

    public void setBitmapIDs(int vgdot1, int vgdot2, int placeholder) {
        mBmpIds[0] = vgdot1;
        mBmpIds[1] = vgdot2;
        mBmpIds[2] = placeholder;
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
        Bitmap bitmap = null;
        
        if (new File(filename).exists()) {
        	bitmap = BitmapFactory.decodeFile(filename);
            if (bitmap != null) {
                nameCache.add(name);
                return bitmap;
            }
        }

    	bitmap = ((BitmapDrawable) mView.getResources()
            .getDrawable(mBmpIds[2])).getBitmap();
      	return bitmap;
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

    public void doContextAction(int action) {
        mCore.doContextAction(action);
    }

    public boolean isContextActionsVisible() {
        return mButtonLayout != null;
    }

    public boolean showContextActions(Ints actions, float x, float y, float w, float h) {
        removeButtonLayout(mButtonLayout);
        mButtonLayout = null;
        
        int n = actions.count();
        if (n == 0)
            return false;

        Rect selbox = new Rect((int) x, (int) y, (int) (x + w), (int) (y + h));
        mButtonLayout = new RelativeLayout(mContext);

        if (selbox.height() < (n < 7 ? 40 : 80)) {
            selbox.inset(0, (selbox.height() - (n < 7 ? 40 : 80)) / 2);
        }
        if (selbox.width() < (n == 3 || n > 4 ? 120 : 40)) {
            selbox.inset(selbox.width() - (n == 3 || n > 4 ? 120 : 40) / 2, 0);
        }
        selbox.inset(-12, -15);

        for (int i = 0; i < n; i++) {
            addContextAction(n, i, actions.get(i), selbox, mButtonLayout);
        }

        addButtonLayout(mButtonLayout);
        return isContextActionsVisible();
    }

    private void addContextAction(int n, int index, final int action, Rect selbox,
            RelativeLayout layout) {
        int xc, yc;

        switch (index) {
        case 0:
            if (n == 1) {
                xc = selbox.centerX();
                yc = selbox.top;    // MT
            } else {
                xc = selbox.left;
                yc = selbox.top;    // LT
            }
            break;
        case 1:
            if (n == 3) {
                xc = selbox.centerX();
                yc = selbox.top;    // MT
            } else {
                xc = selbox.right;
                yc = selbox.top;    // RT
            }
            break;
        case 2:
            if (n == 3) {
                xc = selbox.right;
                yc = selbox.top;    // RT
            } else {
                xc = selbox.right;
                yc = selbox.bottom; // RB
            }
            break;
        case 3:
            xc = selbox.left;
            yc = selbox.bottom;     // LB
            break;
        case 4:
            xc = selbox.centerX();
            yc = selbox.top;        // MT
            break;
        case 5:
            xc = selbox.centerX();
            yc = selbox.bottom;     // MB
            break;
        case 6:
            xc = selbox.right;
            yc = selbox.centerY();  // RM
            break;
        case 7:
            xc = selbox.left;
            yc = selbox.centerY();  // LM
            break;
        default:
            return;
        }

        Button btn = new Button(mContext);
        btn.setId(100 + action);
        boolean hasImage = setButtonBackground(btn, action);

        btn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                PaintView.this.doContextAction(v.getId() - 100);
                removeButtonLayout(mButtonLayout);
                mButtonLayout = null;
            }
        });

        RelativeLayout.LayoutParams btparams = new RelativeLayout.LayoutParams(
        		hasImage ? 32 : 64, hasImage ? 32 : 36); // w, h
        btparams.leftMargin = xc - btparams.width / 2;
        btparams.topMargin = yc - btparams.height / 2;
        layout.addView(btn, btparams);

    }

    public int[] getContextButtonImages() {
        return mImageIDs;
    }

    public void setContextButtonImages(int[] mImageIDs) {
        this.mImageIDs = mImageIDs;
    }
    
    private static final String[] buttonCaptions = { null, "全选", "重选", "绘图", "取消",
    	    "删除", "克隆", "剪开", "角标", "定长", "不定长", "锁定", "解锁",
    	    "编辑", "返回", "闭合", "不闭合", "加点", "删点", "成组", "解组",
    	    "翻转", "三视图" };

    private boolean setButtonBackground(Button button, int index) {
    	if (index >= 0 && index < mImageIDs.length && mImageIDs[index] != 0) {
    		button.setBackgroundResource(mImageIDs[index]);
    		return true;
    	}
    	if (index >= 0 && index < buttonCaptions.length) {
    		button.setText(buttonCaptions[index]);
    	}
    	return false;
    }

    public String getFilePath() {
        return mFilePath;
    }

    public void setFilePath(String filePath) {
        this.mFilePath = filePath;
    }
}
