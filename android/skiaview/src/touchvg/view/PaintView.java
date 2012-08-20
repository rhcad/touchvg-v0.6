package touchvg.view;

import touchvg.skiaview.GiSkiaView;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

public class PaintView extends View {
    private GiSkiaView mView;
    private GiCanvasEx mCanvas;
    private Context context;
    private GestureDetector detector;
    private int gestureState ;
    private int gestureType ;
    private int fingerCount;
    private float lastX = 1;
    private float lastY = 1;
    private float lastX1 = 1;
    private float lastY1 = 1;
    private boolean isMoving = false;
    private int mBkColor = Color.TRANSPARENT;
    
    static {
        System.loadLibrary("skiaview");
    }
    
    public PaintView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
        init();
    }
    
    public PaintView(Context context) {
        super(context);
        this.context = context;
        init();
    }
    
    private void init()
    {
        DisplayMetrics dm = context.getApplicationContext().getResources().getDisplayMetrics(); 
        GiCanvasEx.setScreenDpi(dm.densityDpi);
        
        mCanvas = new GiCanvasEx(this);
        mView = new GiSkiaView(mCanvas);
        detector = new GestureDetector(context, new PaintGestureDetector());
        
        this.setOnTouchListener(new OnTouchListener() {
            
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction())
                {
                case MotionEvent.ACTION_MOVE:
                    fingerCount = event.getPointerCount();
                    isMoving = true;
                    if (fingerCount == 2)
                    {
                        gestureType = 5;
                        gestureState = 2;
                        mView.onGesture(gestureType, gestureState, fingerCount, event.getX(0), event.getY(0), event.getX(1), event.getY(1));
                        lastX = event.getX(0);
                        lastY = event.getY(0);
                        lastX1 = event.getX(1);
                        lastY1 = event.getY(1);
                    }
                    break;
                    
                case MotionEvent.ACTION_UP:
                    if (isMoving)
                    {
                        isMoving = false;
                        gestureState = 3;
                        if (fingerCount == 1)
                        {
                            if (gestureType == 1)
                            {
                                mView.onGesture(gestureType, gestureState, fingerCount, event.getX(), event.getY(), 0, 0);
                                gestureType = 0;
                            }
                        }
                        else if (fingerCount == 2)
                        {
                            if (gestureType == 5)
                            {
                                mView.onGesture(gestureType, gestureState, fingerCount, lastX, lastY, lastX1, lastY1);
                                gestureType = 0;
                            }
                        }
                        lastX = 1;
                        lastY = 1;
                        lastX1 = 1;
                        lastY1 = 1;
                    }
                    break;
                }
                return detector.onTouchEvent(event);
            }
        });
    }
    
    public GiSkiaView getCoreView() {
        return mView;
    }
    
    public void setBkColor(int argb) {
        mBkColor = argb;
    }
    
    protected void onDraw(Canvas canvas) {
        mView.onSize(canvas.getWidth(), canvas.getHeight());
        
        if (mCanvas.beginPaint(canvas)) {
            canvas.drawColor(mBkColor);
            mView.onDraw(mCanvas);
            mView.onDynDraw(mCanvas);
            mCanvas.endPaint();
        }
    }
    
    private class PaintGestureDetector extends GestureDetector.SimpleOnGestureListener
    {
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            gestureState = 0;
            gestureType = 2;
            mView.onGesture(gestureType, gestureState, 1, e.getX(), e.getY(), 0, 0);
            gestureType = 0;
            return false;
        }
        
        @Override
        public void onLongPress(MotionEvent e) {
            gestureType = 4;
            fingerCount = e.getPointerCount();
            gestureState = 0;
            mView.onGesture(gestureType, gestureState, 1, e.getX(), e.getY(), 0, 0);
            gestureType = 0;
        }

        @Override
        public boolean onDown(MotionEvent e) {
            gestureState = 1;
            fingerCount = e.getPointerCount();
            lastX = e.getX(0);
            lastY = e.getY(0);
            if (fingerCount > 1) {
                lastX1 = e.getX(1);
                lastY1 = e.getY(1);
            }   // call mView.onGesture after later.
            return true;
        }
        
        @Override
        public boolean onDoubleTap(MotionEvent e) {
            gestureState = 1;
            gestureType = 3;
            mView.onGesture(gestureType, gestureState, 1, e.getX(), e.getY(), 0, 0);
            gestureType = 0;
            return false;
        }
        
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2,
                float distanceX, float distanceY) {
            fingerCount = e2.getPointerCount();
            if (fingerCount == 1)
            {
                gestureType = 1;
                mView.onGesture(gestureType, gestureState, fingerCount, e2.getX(0), e2.getY(0), 0, 0);
                switch (e2.getAction())
                {
                case MotionEvent.ACTION_MOVE:
                    isMoving = true;
                    gestureState = 2;
                    
                    break;
                case MotionEvent.ACTION_CANCEL:
                    gestureState = 0;
                    break;
                }
            }
            
            return false;
        }
    }
    
}
