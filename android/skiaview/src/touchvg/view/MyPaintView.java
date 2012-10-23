package touchvg.view;

import java.io.File;

import android.content.Context;
import android.graphics.Color;
import android.view.View;
import touchvg.skiaview.GiContext;
import touchvg.skiaview.GiSkiaView;

public class MyPaintView extends Object {
	private PaintView mView = null;
	private GiSkiaView mCoreView = null;
	private String mShapeCommand = "splines";
	
	public MyPaintView(View view) {
		this.mView = (PaintView)view;
		this.mCoreView = this.mView.getCoreView();
		setYellowPen();
    }
	
	public MyPaintView(Context context) {
		this.mView = new PaintView(context);
		this.mCoreView = this.mView.getCoreView();
		setYellowPen();
    }
	
	public View getView() {
		return mView;
	}
	
	public void setBkColor(int argb) {
		mView.setBkColor(argb);
	}
	
	public String getCommandName() {
		return mCoreView.getCommandName();
	}
	
	public boolean setCommandName(String cmdname) {
		return mCoreView.setCommandName(cmdname);
	}
	
	public void onSelect() {
		setCommandName("select");
	}
	
	public void onEraser() {
		setCommandName("erase");
	}
	
	public void setColorPen(int rgb) {
		setCommandName(mShapeCommand);
        GiContext ctx = mCoreView.getCurrentContext(false);
        ctx.setLineARGB(rgb);
        mCoreView.applyContext(ctx, 0x01, 1);	// 0x01: only RGB
        ctx.delete();
	}
	
	public void setRedPen() {
		setColorPen(Color.RED);
	}
	
	public void setBluePen() {
		setColorPen(Color.BLUE);
	}
	
	public void setYellowPen() {
		setColorPen(Color.YELLOW);
	}
	
	public void setGestureEnable(boolean enable) {
	    mView.setGestureEnable(enable);
	}
	
	public void clearShapes() {
		mCoreView.loadShapes(null);
		setCommandName(mShapeCommand);
	}
	
	public float setPenBold() {
		GiContext ctx = mCoreView.getCurrentContext(true);
		ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() - 1 : -2, true);
        mCoreView.applyContext(ctx, 0x04, 1);	// 0x01: only LineWidth
        
        float w = ctx.getLineWidth();
        ctx.delete();
        return w;
	}
	
    public void penAlphaChanged(int alpha)
    {
    	GiContext ctx = mCoreView.getCurrentContext(true);
    	ctx.setLineAlpha((char)alpha);
    	mCoreView.applyContext(ctx, 0x02, 1);
    	ctx.delete();
    }
	
	public float penWidthChanged(int width)
	{
		GiContext ctx = mCoreView.getCurrentContext(true);
		ctx.setLineWidth(ctx.getLineWidth() < 0 ? -width : -1, true);
        mCoreView.applyContext(ctx, 0x04, 1);	// 0x01: only LineWidth
        
        float w = ctx.getLineWidth();
        ctx.delete();
        return w;
	}
	
	public float setPenThick() {
		GiContext ctx = mCoreView.getCurrentContext(true);
		ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() + 1 : -1, true);
        mCoreView.applyContext(ctx, 0x04, 1);	// 0x01: only LineWidth
        
        float w = ctx.getLineWidth();
        ctx.delete();
        return w;
	}
	
	public void penStyleChanged(int style) {
		GiContext ctx = mCoreView.getCurrentContext(true);
        ctx.setLineStyle(style);
        mCoreView.applyContext(ctx, 8, 1);
	}
	
	public int switchPenStyle() {
        GiContext ctx = mCoreView.getCurrentContext(true);
        if (ctx.getLineStyle() >= 4)
            ctx.setLineStyle(0);
        else
            ctx.setLineStyle(ctx.getLineStyle() + 1);
        mCoreView.applyContext(ctx, 8, 1);
        
        int style = ctx.getLineStyle();
        ctx.delete();
        return style;
    }
	
	public String switchShapeType() {
    	String[] names = { "line", "fixedline", "rect", "square", "ellipse", "circle", 
    			"triangle", "diamond", "polygon", "quadrangle", "parallelogram", 
    			"lines", "splines", "grid", null };
    	String cmd = mCoreView.getCommandName();
    	
    	int i = 0;
    	for (; names[i] != null && !cmd.equals(names[i]); i++) ;
    	if (names[i] != null) i++;
    	if (names[i] == null) i = 0;
        
        if (setCommandName(names[i])) {
        	mShapeCommand = names[i];
        }
        
        return mShapeCommand;
    }
	
	public int getShapeCount() {
		return mCoreView.getShapeCount();
	}
	
	public boolean readFromFile(String filename) {
		//boolean ret = s.readFile(filename);
		//ret = mCoreView.loadShapes(ret ? s : null) && ret;
		return false;
	}
	
	public boolean loadFromString(String content)
	{
		//s.setContent(content);
		//return mCoreView.loadShapes(s);
		return false;
	}
	
	public String getSaveContent()
	{
		//mCoreView.saveShapes(s);
		//return s.getContent();
		return "";
	}
	
	public boolean saveAsFile(String filename) {
		boolean ret = false;
		if (getShapeCount() == 0) {
			ret = new File(filename).delete();
		} else {
			//ret = mCoreView.saveShapes(s) && s.writeFile(filename);
		}
		return ret;
	}
	
	public byte[] getCanvasBitmap() {
		return mView.getCanvasBitmap();
	}
	
	public void setBitmapIDs(int vgdot1, int vgdot2) {
		mView.setBitmapIDs(vgdot1, vgdot2);
	}
}
