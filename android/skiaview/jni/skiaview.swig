#ifdef SWIGJAVA
%insert("runtime") %{
#define SWIG_JAVA_NO_DETACH_CURRENT_THREAD
#define SWIG_JAVA_ATTACH_CURRENT_THREAD_AS_DAEMON
%}
#endif

%module(directors="1") skiaview
%{
#include "GiSkiaView.h"
#include "GiCanvasBase.h"
#include <mgstoragebs.h>
%}

%include <mgtype.h>
%include <mgtol.h>
%include <mgvec.h>
%include <mgpnt.h>
%include <mgmat.h>
%include <mgbox.h>
%include <gidef.h>
%include <gicolor.h>
%include <gicontxt.h>
%include <gixform.h>
%include <gicanvdr.h>
%include <gigraph.h>

%include "mgvector.h"
%template(Floats) mgvector<float>;
%template(Chars) mgvector<short>;
%template(Bytes) mgvector<char>;

%feature("director") GiCanvasBase;
%include "GiCanvasBase.h"

%include "GiSkiaView.h"

%feature("director") MgStorageBase;
%include <mgstoragebs.h>

#ifdef SWIGJAVA
%{
#include <jni.h>
#ifdef ANDROID
#include <android/log.h>
#endif

struct TmpJOBJ {
    JNIEnv* env;
    jobject* jo;
    template <class T>
    TmpJOBJ(JNIEnv* e, T* v) : env(e), jo((jobject*)v) { }
    ~TmpJOBJ() { if (*jo) { env->DeleteLocalRef(*jo); *jo = 0; } }
};
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *ajvm, void *reserved) {
    return JNI_VERSION_1_6;
}
%}
#endif