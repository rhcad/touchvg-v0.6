//! \file MyPaintView.java
//! \brief 定义Android绘图视图辅助类 MyPaintView
// License: LGPL, https://github.com/rhcad/touchvg

package touchvg.view;

import java.io.File;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.view.View;
import touchvg.jni.GiContext;
import touchvg.jni.GiCoreView;

//! Android绘图视图辅助类
/** \ingroup GRAPH_SKIA
 *  \see PaintView, GiCoreView
 */
public class MyPaintView extends Object {
    private PaintView mView = null;
    private GiCoreView mCoreView = null;
    private String mShapeCommand = "splines";

    public MyPaintView(View view, Context context) {
        this.mView = (PaintView)view;
        this.mCoreView = this.mView.getCoreView();
        setRedPen();
        setCommandName(mShapeCommand);
        penWidthChanged(3);
    }

    public MyPaintView(Context context) {
        this.mView = new PaintView(context);
        this.mCoreView = this.mView.getCoreView();
        setYellowPen();
        penWidthChanged(3);
    }

    public void addImage(String name)
    {
        Bitmap bitmap = mView.getImageBitmap(name);
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        mCoreView.addImageShapes(name, width, height);
    }

    public void addImage(int picID)
    {
        Bitmap bitmap = ((BitmapDrawable) mView.getResources()
                .getDrawable(picID)).getBitmap();
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        mCoreView.addImageShapes(String.valueOf(picID), width, height);
    }

    public void setFilePath(String filePath) {
        this.mView.setFilePath(filePath);
    }

    public String getFilePath() {
        return this.mView.getFilePath();
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
        return mCoreView.setCommandName(cmdname.isEmpty() ? "@draw" : cmdname);
    }

    public void onSelect() {
        setCommandName("select");
    }

    public void onEraser() {
        setCommandName("erase");
    }

    public void setColorPen(int rgb) {
        GiContext ctx = mCoreView.getCurrentContext(false);
        ctx.setLineARGB(rgb);
        mCoreView.applyContext(ctx, 0x01, 1);   // 0x01: only RGB
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
        mCoreView.applyContext(ctx, 0x04, 1);   // 0x01: only LineWidth

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
        ctx.setLineWidth(ctx.getLineWidth() < 0 ? -width : -width, true);
        mCoreView.applyContext(ctx, 0x04, 1);   // 0x01: only LineWidth

        float w = ctx.getLineWidth();
        ctx.delete();
        return w;
    }

    public float setPenThick() {
        GiContext ctx = mCoreView.getCurrentContext(true);
        ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() + 1 : -1, true);
        mCoreView.applyContext(ctx, 0x04, 1);   // 0x01: only LineWidth

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
        String[] names = { "splines", "line", "rect", "square", "ellipse", "circle", "triangle",
            "polygon", "quadrangle", "lines", "grid", "arc3p", "arc-cse", "arc-tan",
            null };
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

    public String setShapeType(String shapeName)
    {
        if (setCommandName(shapeName.isEmpty() ? "@draw" : shapeName)) {
            mShapeCommand = mCoreView.getCommandName();
        }
        return mShapeCommand;
    }

    public int getShapeCount() {
        return mCoreView.getShapeCount();
    }

    public int getChangeCount()
    {
        return mCoreView.getChangeCount();
    }

    public boolean loadFromFile(String filename) {
        MgStorageJson s = new MgStorageJson();
        boolean ret = s.readFile(filename);
        ret = mCoreView.loadShapes(ret ? s.storageForRead() : null) && ret;
        return ret;
    }

    public boolean loadFromString(String content)
    {
        MgStorageJson s = new MgStorageJson();
        s.setContent(content);
        return mCoreView.loadShapes(s.storageForRead());
    }

    public String getSaveContent()
    {
        MgStorageJson s = new MgStorageJson();
        mCoreView.saveShapes(s.storageForWrite());
        return s.getContent();
    }

    public boolean saveAsFile(String filename) {
        boolean ret;
        if (getShapeCount() == 0) {
            ret = new File(filename).delete();
        } else {
            MgStorageJson s = new MgStorageJson();
            ret = mCoreView.saveShapes(s.storageForWrite()) && s.writeFile(filename);
        }
        return ret;
    }

    public byte[] getCanvasBitmap() {
        return mView.getCanvasBitmap();
    }

    public void setBitmapIDs(int vgdot1, int vgdot2) {
        mView.setBitmapIDs(vgdot1, vgdot2);
    }

    public void clearAllImageFiles() {
        this.mView.clearAllImageFiles();
    }

    public void clearAllUnusedFiles() {
        this.mView.clearAllUnusedFiles();
    }

    public void setZoomFeature(int mask) {
        this.mCoreView.setZoomFeature(mask);
    }
}
