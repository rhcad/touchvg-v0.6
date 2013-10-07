// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.shape;

import java.util.HashSet;
import java.util.Set;

import touchvg.core.CmdObserverDefault;
import touchvg.core.GiContext;
import touchvg.core.GiGraphics;
import touchvg.core.Matrix2d;
import touchvg.core.MgBaseShape;
import touchvg.core.MgCommand;
import touchvg.core.MgCommandDraw;
import touchvg.core.MgHitResult;
import touchvg.core.MgMotion;
import touchvg.core.MgObject;
import touchvg.core.MgShape;
import touchvg.core.MgShapeFactory;
import touchvg.core.MgSplines;
import touchvg.core.MgStorage;
import touchvg.core.Point2d;
import touchvg.view.GraphView;
import touchvg.view.ViewHelper;
import vgtest.app.R;
import android.content.Context;

//! 测试自定义图形类的测试视图类
public class ViewSinShape extends GraphView {
    private MyCmdObserver mObserver = new MyCmdObserver();
    private static final int[] HANDLEIDS = { R.drawable.vgdot1,
            R.drawable.vgdot2, R.drawable.vgdot3 };

    public ViewSinShape(Context context) {
        super(context);

        final ViewHelper helper = new ViewHelper(this);

        ViewHelper.setContextButtonImages(null, R.array.vg_action_captions,
                null, HANDLEIDS);

        helper.cmdView().getCmdSubject().registerObserver(mObserver);
        helper.setCommand(DrawSinShape.NAME); // may be called by a button
    }
    
    @Override
    protected void onDetachedFromWindow() {
        if (mObserver != null) {
            final ViewHelper helper = new ViewHelper(this);
            helper.cmdView().getCmdSubject().unregisterObserver(mObserver);
            mObserver.delete();
            mObserver = null;
        }
        super.onDetachedFromWindow();
    }
    
    private class MyCmdObserver extends CmdObserverDefault {
        
        @Override
        public MgBaseShape createShape(MgMotion sender, int type) {
            if (type == SinShape.TYPE)
                return new SinShape();
            return super.createShape(sender, type);
        }
        
        @Override
        public MgCommand createCommand(MgMotion sender, String name) {
            if (name.equals(DrawSinShape.NAME))
                return new DrawSinShape();
            return super.createCommand(sender, name);
        }
    }
    
    private Set<MgCommand> mCmdCache = new HashSet<MgCommand>();
    
    private class DrawSinShape extends MgCommandDraw {
        public static final String NAME = "sin";
        
        public DrawSinShape() {
        	mCmdCache.add(this);
        }
        
        @Override
        public String getName() {
            return NAME;
        }
        
        @Override
        public void release() {
            mCmdCache.remove(this);
            delete();
        }
        
        @Override
        public MgShape createShape(MgShapeFactory factory) {
            return factory.createShape(SinShape.TYPE);
        }
        
        @Override
        public boolean click(MgMotion sender) {
            super.click(sender);
            if (sender.getView().getCommandName().equals(NAME)) {
                dynshape().shape().setHandlePoint(0, snapPoint(sender), 0);
                addShape(sender);
            }
            return true;
        }
    }

    private static Set<SinShape> mShapeCache;
    
    private class SinShape extends MgBaseShape {
        private MgSplines mCurve = new MgSplines();
        public static final int TYPE = 100;
        
        public SinShape() {
        	if (mShapeCache == null)
        		mShapeCache = new HashSet<SinShape>();
        	mShapeCache.add(this);
        	
            for (int i = 0; i < 100; i++) {
                double y = 10 * Math.sin(i * Math.PI / 20);
                mCurve.addPoint(new Point2d(i * 0.2f, (float)y));
            }
        }
        
        private SinShape castShape(MgObject obj) {
        	for (SinShape sp: mShapeCache) {
        		if (MgObject.getCPtr(obj) == getCPtr(sp)) {
        			return sp;
        		}
        	}
        	return null;
        }
        
        @Override
        public int getType() {
            return TYPE;
        }
        
        @Override
        public boolean isKindOf(int type) {
            return type == TYPE || super.isKindOf(type);
        }

        @Override
        public String getTypeName() {
            return this.getClass().getSimpleName();
        }
        
        @Override
        public MgObject clone() {
            SinShape obj = new SinShape();
            obj.copy(this);
            return obj;
        }
        
        @Override
        public void copy(MgObject src) {
            super.copy(src);
            SinShape s = castShape(src);
            if (s != null) {
                mCurve.copy(s.mCurve);
            }
        }
        
        @Override
        public void release() {
            mShapeCache.remove(this);
            delete();
        }
        
        @Override
        public boolean equals(MgObject src) {
        	SinShape s = castShape(src);
            if (s == null || !mCurve.equals(s.mCurve)) {
            	return false;
            }
            return super.equals(src);
        }

        @Override
        public void update() {
            mCurve.update();
            super.setExtent(mCurve.getExtent());
            super.update();
        }

        @Override
        public void transform(Matrix2d mat) {
            mCurve.transform(mat);
            super.transform(mat);
        }

        @Override
        public void clear() {
            //mCurve.clear();
            super.clear();
        }

        @Override
        public int getPointCount() {
            return 1;
        }

        @Override
        public Point2d getPoint(int index) {
            return mCurve.getExtent().center();
        }

        @Override
        public void setPoint(int index, Point2d pt) {
            mCurve.offset(pt.subtract(getPoint(0)), -1);
        }

        @Override
        public boolean isCurve() {
            return true;
        }

        @Override
        public float hitTest(Point2d pt, float tol, MgHitResult res) {
            float dist = mCurve.hitTest(pt, tol, res);
            return dist;
        }

        @Override
        public boolean draw(int mode, GiGraphics gs, GiContext ctx, int segment) {
            boolean ret = mCurve.draw(mode, gs, ctx, segment);
            return super.draw(mode, gs, ctx, segment) || ret;
        }

        @Override
        public boolean save(MgStorage s) {
            return super.save(s) && mCurve.save(s);
        }

        @Override
        public boolean load(MgShapeFactory factory, MgStorage s) {
            return super.load(factory, s) && mCurve.load(factory, s);
        }

        @Override
        public int getHandleCount() {
            return super.getHandleCount();
        }

        @Override
        public Point2d getHandlePoint(int index) {
            return super.getHandlePoint(index);
        }

        @Override
        public boolean setHandlePoint(int index, Point2d pt, float tol) {
            return super.setHandlePoint(index, pt, tol);
        }
        
        @Override
        public void setOwner(MgShape owner) {
            super.setOwner(owner);
        }
    }
}
