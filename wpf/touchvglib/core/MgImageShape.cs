/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace touchvg.core {

using System;
using System.Runtime.InteropServices;

public class MgImageShape : MgBaseRect {
  private HandleRef swigCPtr;

  internal MgImageShape(IntPtr cPtr, bool cMemoryOwn) : base(touchvgPINVOKE.MgImageShape_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(MgImageShape obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~MgImageShape() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          touchvgPINVOKE.delete_MgImageShape(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public MgImageShape() : this(touchvgPINVOKE.new_MgImageShape(), true) {
  }

  public static MgImageShape create() {
    IntPtr cPtr = touchvgPINVOKE.MgImageShape_create();
    MgImageShape ret = (cPtr == IntPtr.Zero) ? null : new MgImageShape(cPtr, false);
    return ret;
  }

  public new static int Type() {
    int ret = touchvgPINVOKE.MgImageShape_Type();
    return ret;
  }

  public override string getTypeName() {
    string ret = touchvgPINVOKE.MgImageShape_getTypeName(swigCPtr);
    return ret;
  }

  public override MgObject clone() {
    IntPtr cPtr = touchvgPINVOKE.MgImageShape_clone(swigCPtr);
    MgObject ret = (cPtr == IntPtr.Zero) ? null : new MgObject(cPtr, false);
    return ret;
  }

  public override void copy(MgObject src) {
    touchvgPINVOKE.MgImageShape_copy(swigCPtr, MgObject.getCPtr(src));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public override void release() {
    touchvgPINVOKE.MgImageShape_release(swigCPtr);
  }

  public override bool equals(MgObject src) {
    bool ret = touchvgPINVOKE.MgImageShape_equals(swigCPtr, MgObject.getCPtr(src));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override int getType() {
    int ret = touchvgPINVOKE.MgImageShape_getType(swigCPtr);
    return ret;
  }

  public override bool isKindOf(int type) {
    bool ret = touchvgPINVOKE.MgImageShape_isKindOf(swigCPtr, type);
    return ret;
  }

  public override Box2d getExtent() {
    Box2d ret = new Box2d(touchvgPINVOKE.MgImageShape_getExtent(swigCPtr), true);
    return ret;
  }

  public override void update() {
    touchvgPINVOKE.MgImageShape_update(swigCPtr);
  }

  public override void transform(Matrix2d mat) {
    touchvgPINVOKE.MgImageShape_transform(swigCPtr, Matrix2d.getCPtr(mat));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public override void clear() {
    touchvgPINVOKE.MgImageShape_clear(swigCPtr);
  }

  public override void clearCachedData() {
    touchvgPINVOKE.MgImageShape_clearCachedData(swigCPtr);
  }

  public override int getPointCount() {
    int ret = touchvgPINVOKE.MgImageShape_getPointCount(swigCPtr);
    return ret;
  }

  public override Point2d getPoint(int index) {
    Point2d ret = new Point2d(touchvgPINVOKE.MgImageShape_getPoint(swigCPtr, index), true);
    return ret;
  }

  public override void setPoint(int index, Point2d pt) {
    touchvgPINVOKE.MgImageShape_setPoint(swigCPtr, index, Point2d.getCPtr(pt));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public override bool isClosed() {
    bool ret = touchvgPINVOKE.MgImageShape_isClosed(swigCPtr);
    return ret;
  }

  public override bool hitTestBox(Box2d rect) {
    bool ret = touchvgPINVOKE.MgImageShape_hitTestBox(swigCPtr, Box2d.getCPtr(rect));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override bool draw(int mode, GiGraphics gs, GiContext ctx, int segment) {
    bool ret = touchvgPINVOKE.MgImageShape_draw(swigCPtr, mode, GiGraphics.getCPtr(gs), GiContext.getCPtr(ctx), segment);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override bool save(MgStorage s) {
    bool ret = touchvgPINVOKE.MgImageShape_save(swigCPtr, MgStorage.getCPtr(s));
    return ret;
  }

  public override bool load(MgShapeFactory factory, MgStorage s) {
    bool ret = touchvgPINVOKE.MgImageShape_load(swigCPtr, MgShapeFactory.getCPtr(factory), MgStorage.getCPtr(s));
    return ret;
  }

  public override int getHandleCount() {
    int ret = touchvgPINVOKE.MgImageShape_getHandleCount(swigCPtr);
    return ret;
  }

  public override Point2d getHandlePoint(int index) {
    Point2d ret = new Point2d(touchvgPINVOKE.MgImageShape_getHandlePoint(swigCPtr, index), true);
    return ret;
  }

  public override bool setHandlePoint(int index, Point2d pt, float tol) {
    bool ret = touchvgPINVOKE.MgImageShape_setHandlePoint(swigCPtr, index, Point2d.getCPtr(pt), tol);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override bool isHandleFixed(int index) {
    bool ret = touchvgPINVOKE.MgImageShape_isHandleFixed(swigCPtr, index);
    return ret;
  }

  public override int getHandleType(int index) {
    int ret = touchvgPINVOKE.MgImageShape_getHandleType(swigCPtr, index);
    return ret;
  }

  public override bool offset(Vector2d vec, int segment) {
    bool ret = touchvgPINVOKE.MgImageShape_offset(swigCPtr, Vector2d.getCPtr(vec), segment);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override float hitTest(Point2d pt, float tol, MgHitResult res) {
    float ret = touchvgPINVOKE.MgImageShape_hitTest(swigCPtr, Point2d.getCPtr(pt), tol, MgHitResult.getCPtr(res));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getName() {
    string ret = touchvgPINVOKE.MgImageShape_getName(swigCPtr);
    return ret;
  }

  public void setName(string name) {
    touchvgPINVOKE.MgImageShape_setName(swigCPtr, name);
  }

  public static MgShape findShapeByImageID(MgShapes shapes, string name) {
    IntPtr cPtr = touchvgPINVOKE.MgImageShape_findShapeByImageID(MgShapes.getCPtr(shapes), name);
    MgShape ret = (cPtr == IntPtr.Zero) ? null : new MgShape(cPtr, false);
    return ret;
  }

}

}
