package touchvg.view;

import java.io.File;

import android.graphics.Color;
import touchvg.skiaview.GiContext;
import touchvg.skiaview.GiSkiaView;

public class MyPaintView extends Object {
	private PaintView mView = null;
	private GiSkiaView mCoreView = null;
	private String mShapeCommand = "splines";
	
	public MyPaintView(Object view) {
		this.mView = (PaintView)view;
		this.mCoreView = this.mView.getCoreView();
		setYellowPen();
    }
	
	public PaintView getView() {
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
	
	public void clearShapes() {
		mCoreView.loadShapes(null);
		setCommandName(mShapeCommand);
	}
	
	public float setPenBold() {
		GiContext ctx = mCoreView.getCurrentContext(true);
		ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() - 1 : -2);
        mCoreView.applyContext(ctx, 0x04, 1);	// 0x01: only LineWidth
        
        float w = ctx.getLineWidth();
        ctx.delete();
        return w;
	}
	
	public float setPenThick() {
		GiContext ctx = mCoreView.getCurrentContext(true);
		ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() + 1 : -1);
        mCoreView.applyContext(ctx, 0x04, 1);	// 0x01: only LineWidth
        
        float w = ctx.getLineWidth();
        ctx.delete();
        return w;
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
	
	public boolean saveAsFile(String filename) {
		boolean ret = false;
		if (getShapeCount() == 0) {
			ret = new File(filename).delete();
		} else {
		}
		return ret;
	}
	
	public void setBitmapIDs(int vgdot1, int vgdot2) {
		mView.setBitmapIDs(vgdot1, vgdot2);
	}
}
