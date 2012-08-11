package touchvg.view;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import touchvg.skiaview.Floats;
import touchvg.skiaview.GiCanvasBase;
import touchvg.skiaview.GiContext;

public class GiCanvasEx extends GiCanvasBase{
	private Path mPath = new Path();
	private Paint mPen = new Paint();
	private Paint mBrush = new Paint();
	private Canvas mCanvas = null;
	
	public GiCanvasEx()
	{
	}
	
	public Canvas getCanvas() {
		return mCanvas;
	}

	public boolean beginPaint(Canvas canvas) {
		this.mCanvas = canvas;
		super.beginPaint();
		
		mPen.setAntiAlias(true);
		mPen.setDither(true);
		mPen.setStyle(Paint.Style.STROKE);
		mPen.setStrokeJoin(Paint.Join.ROUND);
		mPen.setStrokeCap(Paint.Cap.ROUND);
		
		mBrush.setStyle(Paint.Style.FILL);
        
		return true;
	}
	
	public void endPaint(Canvas canvas) {
		this.mCanvas = null;
		super.endPaint();
	}
	
	@Override
	public void antiAliasModeChanged(boolean alias) {
		mPen.setAntiAlias(alias);
	}

	@Override
	public void penChanged(GiContext context, float penWidth) {
		mPen.setColor(context.getLineARGB());
		mPen.setStrokeWidth(penWidth);
	}
	
	@Override
	public void brushChanged(GiContext context) {
		mBrush.setColor(context.getFillARGB());
	}

	@Override
	public boolean beginPath() {
		mPath.reset();
		return true;
	}
	
	@Override
	public boolean pathMoveTo(float x, float y) {
		mPath.moveTo(x, y);
		return true;
	}
	
	@Override
	public boolean pathLineTo(float x, float y) {
		mPath.lineTo(x, y);
		return true;
	}
	
	@Override
	public boolean pathBezierTo(Floats pxs) {
		for (int i = 0; i+5 < pxs.count(); i += 6) {
			mPath.cubicTo(pxs.get(i), pxs.get(i+1), pxs.get(i+2), 
					pxs.get(i+3), pxs.get(i+4), pxs.get(i+5));
		}
		return true;
	}
	
	@Override
	public boolean closePath() {
		mPath.close();
		return true;
	}
	
	@Override
	public boolean endPath(boolean stroke, boolean fill) {
		if (fill)
			mCanvas.drawPath(mPath, mBrush);
		if (stroke)
			mCanvas.drawPath(mPath, mPen);
		return true;
	}

	@Override
	public boolean drawBeziers(Floats pxs) {
		boolean ret = pxs.count() >= 8;
		Path p = new Path();
		
		if (ret) {
			p.moveTo(pxs.get(0), pxs.get(1));
			for (int i = 2; i+5 < pxs.count(); i += 6) {
				p.cubicTo(pxs.get(i), pxs.get(i+1), pxs.get(i+2), 
						pxs.get(i+3), pxs.get(i+4), pxs.get(i+5));
			}
			mCanvas.drawPath(p, mPen);
		}
		
		return ret;
	}
	
	@Override
	public boolean drawRect(float x, float y, float w, float h, 
			               boolean stroke, boolean fill) {
		if (fill)
			mCanvas.drawRect(x, y, x+w, y+h, mBrush);
		if (stroke)
			mCanvas.drawRect(x, y, x+w, y+h, mPen);
		return true;
	}

	@Override
	public boolean drawEllipse(float x, float y, float w, float h,
			boolean stroke, boolean fill) {
		if (fill)
			mCanvas.drawOval(new RectF(x, y, x+w, y+h), mBrush);
		if (stroke)
			mCanvas.drawOval(new RectF(x, y, x+w, y+h), mPen);
		return true;
	}

	@Override
	public boolean drawLine(float x1, float y1, float x2, float y2) {
		mCanvas.drawLine(x1, y1, x2, y2, mPen);
		return true;
	}

	@Override
	public boolean drawLines(Floats pxs) {
		float []f = new float[pxs.count()];
		for (int i = 0; i < pxs.count(); i++) {
			f[i] = pxs.get(i);
		}
		mCanvas.drawLines(f, mPen);
		return true;
	}

	@Override
	public boolean drawPolygon(Floats pxs, boolean stroke, boolean fill) {
		boolean ret = pxs.count() >= 4;
		Path p = new Path();
		
		if (ret) {
			p.moveTo(pxs.get(0), pxs.get(1));
			for (int i = 2; i + 1 < pxs.count(); i += 2) {
				p.lineTo(pxs.get(i), pxs.get(i+1));
			}
			p.close();
			
			if (fill)
				mCanvas.drawPath(p, mBrush);
			if (stroke)
				mCanvas.drawPath(p, mPen);
		}
		
		return ret;
	}
	
}
