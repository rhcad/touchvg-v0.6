/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_touchvg_WRAP_H_
#define SWIG_touchvg_WRAP_H_

class SwigDirector_GiCanvas : public GiCanvas, public Swig::Director {

public:
    SwigDirector_GiCanvas();
    virtual ~SwigDirector_GiCanvas();
    virtual void setPen(int argb, float width, int style, float phase);
    virtual void setBrush(int argb, int style);
    virtual void clearRect(float x, float y, float w, float h);
    virtual void drawRect(float x, float y, float w, float h, bool stroke, bool fill);
    virtual void drawLine(float x1, float y1, float x2, float y2);
    virtual void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill);
    virtual void beginPath();
    virtual void moveTo(float x, float y);
    virtual void lineTo(float x, float y);
    virtual void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    virtual void quadTo(float cpx, float cpy, float x, float y);
    virtual void closePath();
    virtual void drawPath(bool stroke, bool fill);
    virtual void saveClip();
    virtual void restoreClip();
    virtual bool clipRect(float x, float y, float w, float h);
    virtual bool clipPath();
    virtual void drawHandle(float x, float y, int type);
    virtual void drawBitmap(char const *name, float xc, float yc, float w, float h, float angle);
    virtual float drawTextAt(char const *text, float x, float y, float h, int align);

    typedef void (SWIGSTDCALL* SWIG_Callback0_t)(int, float, int, float);
    typedef void (SWIGSTDCALL* SWIG_Callback1_t)(int, int);
    typedef void (SWIGSTDCALL* SWIG_Callback2_t)(float, float, float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback3_t)(float, float, float, float, unsigned int, unsigned int);
    typedef void (SWIGSTDCALL* SWIG_Callback4_t)(float, float, float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback5_t)(float, float, float, float, unsigned int, unsigned int);
    typedef void (SWIGSTDCALL* SWIG_Callback6_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback7_t)(float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback8_t)(float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback9_t)(float, float, float, float, float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback10_t)(float, float, float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback11_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback12_t)(unsigned int, unsigned int);
    typedef void (SWIGSTDCALL* SWIG_Callback13_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback14_t)();
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback15_t)(float, float, float, float);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback16_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback17_t)(float, float, int);
    typedef void (SWIGSTDCALL* SWIG_Callback18_t)(char *, float, float, float, float, float);
    typedef float (SWIGSTDCALL* SWIG_Callback19_t)(char *, float, float, float, int);
    void swig_connect_director(SWIG_Callback0_t callbacksetPen, SWIG_Callback1_t callbacksetBrush, SWIG_Callback2_t callbackclearRect, SWIG_Callback3_t callbackdrawRect, SWIG_Callback4_t callbackdrawLine, SWIG_Callback5_t callbackdrawEllipse, SWIG_Callback6_t callbackbeginPath, SWIG_Callback7_t callbackmoveTo, SWIG_Callback8_t callbacklineTo, SWIG_Callback9_t callbackbezierTo, SWIG_Callback10_t callbackquadTo, SWIG_Callback11_t callbackclosePath, SWIG_Callback12_t callbackdrawPath, SWIG_Callback13_t callbacksaveClip, SWIG_Callback14_t callbackrestoreClip, SWIG_Callback15_t callbackclipRect, SWIG_Callback16_t callbackclipPath, SWIG_Callback17_t callbackdrawHandle, SWIG_Callback18_t callbackdrawBitmap, SWIG_Callback19_t callbackdrawTextAt);

private:
    SWIG_Callback0_t swig_callbacksetPen;
    SWIG_Callback1_t swig_callbacksetBrush;
    SWIG_Callback2_t swig_callbackclearRect;
    SWIG_Callback3_t swig_callbackdrawRect;
    SWIG_Callback4_t swig_callbackdrawLine;
    SWIG_Callback5_t swig_callbackdrawEllipse;
    SWIG_Callback6_t swig_callbackbeginPath;
    SWIG_Callback7_t swig_callbackmoveTo;
    SWIG_Callback8_t swig_callbacklineTo;
    SWIG_Callback9_t swig_callbackbezierTo;
    SWIG_Callback10_t swig_callbackquadTo;
    SWIG_Callback11_t swig_callbackclosePath;
    SWIG_Callback12_t swig_callbackdrawPath;
    SWIG_Callback13_t swig_callbacksaveClip;
    SWIG_Callback14_t swig_callbackrestoreClip;
    SWIG_Callback15_t swig_callbackclipRect;
    SWIG_Callback16_t swig_callbackclipPath;
    SWIG_Callback17_t swig_callbackdrawHandle;
    SWIG_Callback18_t swig_callbackdrawBitmap;
    SWIG_Callback19_t swig_callbackdrawTextAt;
    void swig_init_callbacks();
};

class SwigDirector_GiView : public GiView, public Swig::Director {

public:
    SwigDirector_GiView();
    virtual ~SwigDirector_GiView();
    virtual void regenAll();
    virtual void regenAppend();
    virtual void redraw();
    virtual bool useFinger();
    virtual bool isContextActionsVisible();
    virtual bool showContextActions(mgvector< int > const &actions, mgvector< float > const &buttonXY, float x, float y, float w, float h);
    virtual void commandChanged();
    virtual void selectionChanged();
    virtual void contentChanged();

    typedef void (SWIGSTDCALL* SWIG_Callback0_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback1_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback2_t)();
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback3_t)();
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback4_t)();
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback5_t)(void *, void *, float, float, float, float);
    typedef void (SWIGSTDCALL* SWIG_Callback6_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback7_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback8_t)();
    void swig_connect_director(SWIG_Callback0_t callbackregenAll, SWIG_Callback1_t callbackregenAppend, SWIG_Callback2_t callbackredraw, SWIG_Callback3_t callbackuseFinger, SWIG_Callback4_t callbackisContextActionsVisible, SWIG_Callback5_t callbackshowContextActions, SWIG_Callback6_t callbackcommandChanged, SWIG_Callback7_t callbackselectionChanged, SWIG_Callback8_t callbackcontentChanged);

private:
    SWIG_Callback0_t swig_callbackregenAll;
    SWIG_Callback1_t swig_callbackregenAppend;
    SWIG_Callback2_t swig_callbackredraw;
    SWIG_Callback3_t swig_callbackuseFinger;
    SWIG_Callback4_t swig_callbackisContextActionsVisible;
    SWIG_Callback5_t swig_callbackshowContextActions;
    SWIG_Callback6_t swig_callbackcommandChanged;
    SWIG_Callback7_t swig_callbackselectionChanged;
    SWIG_Callback8_t swig_callbackcontentChanged;
    void swig_init_callbacks();
};


#endif
