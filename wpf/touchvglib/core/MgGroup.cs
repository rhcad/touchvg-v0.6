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

public class MgGroup : MgComposite {
  private HandleRef swigCPtr;

  internal MgGroup(IntPtr cPtr, bool cMemoryOwn) : base(touchvgPINVOKE.MgGroup_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(MgGroup obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~MgGroup() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          touchvgPINVOKE.delete_MgGroup(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public MgGroup() : this(touchvgPINVOKE.new_MgGroup(), true) {
  }

  public static MgGroup create() {
    IntPtr cPtr = touchvgPINVOKE.MgGroup_create();
    MgGroup ret = (cPtr == IntPtr.Zero) ? null : new MgGroup(cPtr, false);
    return ret;
  }

  public new static int Type() {
    int ret = touchvgPINVOKE.MgGroup_Type();
    return ret;
  }

  public override string getTypeName() {
    string ret = touchvgPINVOKE.MgGroup_getTypeName(swigCPtr);
    return ret;
  }

  public override MgObject clone() {
    IntPtr cPtr = touchvgPINVOKE.MgGroup_clone(swigCPtr);
    MgObject ret = (cPtr == IntPtr.Zero) ? null : new MgObject(cPtr, false);
    return ret;
  }

  public override void copy(MgObject src) {
    touchvgPINVOKE.MgGroup_copy(swigCPtr, MgObject.getCPtr(src));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public override void release() {
    touchvgPINVOKE.MgGroup_release(swigCPtr);
  }

  public override bool equals(MgObject src) {
    bool ret = touchvgPINVOKE.MgGroup_equals(swigCPtr, MgObject.getCPtr(src));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override int getType() {
    int ret = touchvgPINVOKE.MgGroup_getType(swigCPtr);
    return ret;
  }

  public override bool isKindOf(int type) {
    bool ret = touchvgPINVOKE.MgGroup_isKindOf(swigCPtr, type);
    return ret;
  }

  public override Box2d getExtent() {
    Box2d ret = new Box2d(touchvgPINVOKE.MgGroup_getExtent(swigCPtr), true);
    return ret;
  }

  public override void update() {
    touchvgPINVOKE.MgGroup_update(swigCPtr);
  }

  public override void transform(Matrix2d mat) {
    touchvgPINVOKE.MgGroup_transform(swigCPtr, Matrix2d.getCPtr(mat));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public override void clear() {
    touchvgPINVOKE.MgGroup_clear(swigCPtr);
  }

  public override void clearCachedData() {
    touchvgPINVOKE.MgGroup_clearCachedData(swigCPtr);
  }

  public override int getPointCount() {
    int ret = touchvgPINVOKE.MgGroup_getPointCount(swigCPtr);
    return ret;
  }

  public override Point2d getPoint(int index) {
    Point2d ret = new Point2d(touchvgPINVOKE.MgGroup_getPoint(swigCPtr, index), true);
    return ret;
  }

  public override void setPoint(int index, Point2d pt) {
    touchvgPINVOKE.MgGroup_setPoint(swigCPtr, index, Point2d.getCPtr(pt));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public override bool isClosed() {
    bool ret = touchvgPINVOKE.MgGroup_isClosed(swigCPtr);
    return ret;
  }

  public override bool hitTestBox(Box2d rect) {
    bool ret = touchvgPINVOKE.MgGroup_hitTestBox(swigCPtr, Box2d.getCPtr(rect));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override bool draw(int mode, GiGraphics gs, GiContext ctx, int segment) {
    bool ret = touchvgPINVOKE.MgGroup_draw(swigCPtr, mode, GiGraphics.getCPtr(gs), GiContext.getCPtr(ctx), segment);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override bool save(MgStorage s) {
    bool ret = touchvgPINVOKE.MgGroup_save(swigCPtr, MgStorage.getCPtr(s));
    return ret;
  }

  public override bool load(MgShapeFactory factory, MgStorage s) {
    bool ret = touchvgPINVOKE.MgGroup_load(swigCPtr, MgShapeFactory.getCPtr(factory), MgStorage.getCPtr(s));
    return ret;
  }

  public override int getHandleCount() {
    int ret = touchvgPINVOKE.MgGroup_getHandleCount(swigCPtr);
    return ret;
  }

  public override Point2d getHandlePoint(int index) {
    Point2d ret = new Point2d(touchvgPINVOKE.MgGroup_getHandlePoint(swigCPtr, index), true);
    return ret;
  }

  public override bool setHandlePoint(int index, Point2d pt, float tol) {
    bool ret = touchvgPINVOKE.MgGroup_setHandlePoint(swigCPtr, index, Point2d.getCPtr(pt), tol);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override bool isHandleFixed(int index) {
    bool ret = touchvgPINVOKE.MgGroup_isHandleFixed(swigCPtr, index);
    return ret;
  }

  public override int getHandleType(int index) {
    int ret = touchvgPINVOKE.MgGroup_getHandleType(swigCPtr, index);
    return ret;
  }

  public override bool offset(Vector2d vec, int segment) {
    bool ret = touchvgPINVOKE.MgGroup_offset(swigCPtr, Vector2d.getCPtr(vec), segment);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override float hitTest(Point2d pt, float tol, MgHitResult res) {
    float ret = touchvgPINVOKE.MgGroup_hitTest(swigCPtr, Point2d.getCPtr(pt), tol, MgHitResult.getCPtr(res));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool addShape(MgShape shape) {
    bool ret = touchvgPINVOKE.MgGroup_addShape(swigCPtr, MgShape.getCPtr(shape));
    return ret;
  }

}

}
