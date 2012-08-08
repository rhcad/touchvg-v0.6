%module(directors="1") skiaview
%{
#include "GiSkiaView.h"
#include "GiCanvasBase.h"
#include <mgstorage.h>
%}

%include <gicolor.h>
%include <gicontxt.h>

%include stl.i
namespace std {
    %template(FloatVector)    vector<float>;
}

%include various.i
%include "GiCanvasBase.h"
%feature("director") GiCanvasBase;

%include "GiSkiaView.h"

%include <mgstorage.h>
%feature("director") MgStorageBase;
