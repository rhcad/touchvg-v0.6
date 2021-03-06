/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package touchvg.jni;

public class RECT_2D {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected RECT_2D(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(RECT_2D obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        graph2dJNI.delete_RECT_2D(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setLeft(float value) {
    graph2dJNI.RECT_2D_left_set(swigCPtr, this, value);
  }

  public float getLeft() {
    return graph2dJNI.RECT_2D_left_get(swigCPtr, this);
  }

  public void setTop(float value) {
    graph2dJNI.RECT_2D_top_set(swigCPtr, this, value);
  }

  public float getTop() {
    return graph2dJNI.RECT_2D_top_get(swigCPtr, this);
  }

  public void setRight(float value) {
    graph2dJNI.RECT_2D_right_set(swigCPtr, this, value);
  }

  public float getRight() {
    return graph2dJNI.RECT_2D_right_get(swigCPtr, this);
  }

  public void setBottom(float value) {
    graph2dJNI.RECT_2D_bottom_set(swigCPtr, this, value);
  }

  public float getBottom() {
    return graph2dJNI.RECT_2D_bottom_get(swigCPtr, this);
  }

  public RECT_2D() {
    this(graph2dJNI.new_RECT_2D(), true);
  }

}
