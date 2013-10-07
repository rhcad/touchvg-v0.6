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

public class GiCoreView : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal GiCoreView(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(GiCoreView obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~GiCoreView() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          touchvgPINVOKE.delete_GiCoreView(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public GiCoreView(GiCoreView mainView) : this(touchvgPINVOKE.new_GiCoreView__SWIG_0(GiCoreView.getCPtr(mainView)), true) {
  }

  public GiCoreView() : this(touchvgPINVOKE.new_GiCoreView__SWIG_1(), true) {
  }

  public void createView(GiView view, int type) {
    touchvgPINVOKE.GiCoreView_createView__SWIG_0(swigCPtr, GiView.getCPtr(view), type);
  }

  public void createView(GiView view) {
    touchvgPINVOKE.GiCoreView_createView__SWIG_1(swigCPtr, GiView.getCPtr(view));
  }

  public void createMagnifierView(GiView newview, GiView mainView) {
    touchvgPINVOKE.GiCoreView_createMagnifierView(swigCPtr, GiView.getCPtr(newview), GiView.getCPtr(mainView));
  }

  public void destoryView(GiView view) {
    touchvgPINVOKE.GiCoreView_destoryView(swigCPtr, GiView.getCPtr(view));
  }

  public int drawAll(GiView view, GiCanvas canvas) {
    int ret = touchvgPINVOKE.GiCoreView_drawAll(swigCPtr, GiView.getCPtr(view), GiCanvas.getCPtr(canvas));
    return ret;
  }

  public bool drawAppend(GiView view, GiCanvas canvas) {
    bool ret = touchvgPINVOKE.GiCoreView_drawAppend(swigCPtr, GiView.getCPtr(view), GiCanvas.getCPtr(canvas));
    return ret;
  }

  public void dynDraw(GiView view, GiCanvas canvas) {
    touchvgPINVOKE.GiCoreView_dynDraw(swigCPtr, GiView.getCPtr(view), GiCanvas.getCPtr(canvas));
  }

  public int setBkColor(GiView view, int argb) {
    int ret = touchvgPINVOKE.GiCoreView_setBkColor(swigCPtr, GiView.getCPtr(view), argb);
    return ret;
  }

  public static void setScreenDpi(int dpi) {
    touchvgPINVOKE.GiCoreView_setScreenDpi(dpi);
  }

  public void onSize(GiView view, int w, int h) {
    touchvgPINVOKE.GiCoreView_onSize(swigCPtr, GiView.getCPtr(view), w, h);
  }

  public bool onGesture(GiView view, GiGestureType type, GiGestureState state, float x, float y, bool switchGesture) {
    bool ret = touchvgPINVOKE.GiCoreView_onGesture__SWIG_0(swigCPtr, GiView.getCPtr(view), (int)type, (int)state, x, y, switchGesture);
    return ret;
  }

  public bool onGesture(GiView view, GiGestureType type, GiGestureState state, float x, float y) {
    bool ret = touchvgPINVOKE.GiCoreView_onGesture__SWIG_1(swigCPtr, GiView.getCPtr(view), (int)type, (int)state, x, y);
    return ret;
  }

  public bool twoFingersMove(GiView view, GiGestureState state, float x1, float y1, float x2, float y2, bool switchGesture) {
    bool ret = touchvgPINVOKE.GiCoreView_twoFingersMove__SWIG_0(swigCPtr, GiView.getCPtr(view), (int)state, x1, y1, x2, y2, switchGesture);
    return ret;
  }

  public bool twoFingersMove(GiView view, GiGestureState state, float x1, float y1, float x2, float y2) {
    bool ret = touchvgPINVOKE.GiCoreView_twoFingersMove__SWIG_1(swigCPtr, GiView.getCPtr(view), (int)state, x1, y1, x2, y2);
    return ret;
  }

  public bool isPressDragging() {
    bool ret = touchvgPINVOKE.GiCoreView_isPressDragging(swigCPtr);
    return ret;
  }

  public GiGestureType getGestureType() {
    GiGestureType ret = (GiGestureType)touchvgPINVOKE.GiCoreView_getGestureType(swigCPtr);
    return ret;
  }

  public GiGestureState getGestureState() {
    GiGestureState ret = (GiGestureState)touchvgPINVOKE.GiCoreView_getGestureState(swigCPtr);
    return ret;
  }

  public string getCommand() {
    string ret = touchvgPINVOKE.GiCoreView_getCommand(swigCPtr);
    return ret;
  }

  public bool setCommand(GiView view, string name, string arg2) {
    bool ret = touchvgPINVOKE.GiCoreView_setCommand__SWIG_0(swigCPtr, GiView.getCPtr(view), name, arg2);
    return ret;
  }

  public bool setCommand(GiView view, string name) {
    bool ret = touchvgPINVOKE.GiCoreView_setCommand__SWIG_1(swigCPtr, GiView.getCPtr(view), name);
    return ret;
  }

  public bool doContextAction(int action) {
    bool ret = touchvgPINVOKE.GiCoreView_doContextAction(swigCPtr, action);
    return ret;
  }

  public void clearCachedData() {
    touchvgPINVOKE.GiCoreView_clearCachedData(swigCPtr);
  }

  public int addShapesForTest() {
    int ret = touchvgPINVOKE.GiCoreView_addShapesForTest(swigCPtr);
    return ret;
  }

  public int getShapeCount() {
    int ret = touchvgPINVOKE.GiCoreView_getShapeCount(swigCPtr);
    return ret;
  }

  public int getChangeCount() {
    int ret = touchvgPINVOKE.GiCoreView_getChangeCount(swigCPtr);
    return ret;
  }

  public int getSelectedShapeCount() {
    int ret = touchvgPINVOKE.GiCoreView_getSelectedShapeCount(swigCPtr);
    return ret;
  }

  public int getSelectedShapeType() {
    int ret = touchvgPINVOKE.GiCoreView_getSelectedShapeType(swigCPtr);
    return ret;
  }

  public void clear() {
    touchvgPINVOKE.GiCoreView_clear(swigCPtr);
  }

  public bool loadFromFile(string vgfile) {
    bool ret = touchvgPINVOKE.GiCoreView_loadFromFile(swigCPtr, vgfile);
    return ret;
  }

  public bool saveToFile(string vgfile, bool pretty) {
    bool ret = touchvgPINVOKE.GiCoreView_saveToFile__SWIG_0(swigCPtr, vgfile, pretty);
    return ret;
  }

  public bool saveToFile(string vgfile) {
    bool ret = touchvgPINVOKE.GiCoreView_saveToFile__SWIG_1(swigCPtr, vgfile);
    return ret;
  }

  public string getContent() {
    string ret = touchvgPINVOKE.GiCoreView_getContent(swigCPtr);
    return ret;
  }

  public void freeContent() {
    touchvgPINVOKE.GiCoreView_freeContent(swigCPtr);
  }

  public bool setContent(string content) {
    bool ret = touchvgPINVOKE.GiCoreView_setContent(swigCPtr, content);
    return ret;
  }

  public bool zoomToExtent() {
    bool ret = touchvgPINVOKE.GiCoreView_zoomToExtent(swigCPtr);
    return ret;
  }

  public bool zoomToModel(float x, float y, float w, float h) {
    bool ret = touchvgPINVOKE.GiCoreView_zoomToModel(swigCPtr, x, y, w, h);
    return ret;
  }

  public float calcPenWidth(float lineWidth) {
    float ret = touchvgPINVOKE.GiCoreView_calcPenWidth(swigCPtr, lineWidth);
    return ret;
  }

  public GiContext getContext(bool forChange) {
    GiContext ret = new GiContext(touchvgPINVOKE.GiCoreView_getContext(swigCPtr, forChange), false);
    return ret;
  }

  public void setContext(GiContext ctx, int mask, int apply) {
    touchvgPINVOKE.GiCoreView_setContext__SWIG_0(swigCPtr, GiContext.getCPtr(ctx), mask, apply);
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
  }

  public void setContext(int mask) {
    touchvgPINVOKE.GiCoreView_setContext__SWIG_1(swigCPtr, mask);
  }

  public void setContextEditing(bool editing) {
    touchvgPINVOKE.GiCoreView_setContextEditing(swigCPtr, editing);
  }

  public bool addImageShape(string name, float width, float height) {
    bool ret = touchvgPINVOKE.GiCoreView_addImageShape(swigCPtr, name, width, height);
    return ret;
  }

  public bool getBoundingBox(Floats box) {
    bool ret = touchvgPINVOKE.GiCoreView_getBoundingBox(swigCPtr, Floats.getCPtr(box));
    if (touchvgPINVOKE.SWIGPendingException.Pending) throw touchvgPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int viewAdapterHandle() {
    int ret = touchvgPINVOKE.GiCoreView_viewAdapterHandle(swigCPtr);
    return ret;
  }

  public MgView viewAdapter() {
    IntPtr cPtr = touchvgPINVOKE.GiCoreView_viewAdapter(swigCPtr);
    MgView ret = (cPtr == IntPtr.Zero) ? null : new MgView(cPtr, false);
    return ret;
  }

}

}
