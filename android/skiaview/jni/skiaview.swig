%module(directors="1") skiaview
%{
#include "GiSkiaView.h"
#include "GiCanvasBase.h"
#include <mgstorage.h>
%}

%include <gicolor.h>
%include <gicontxt.h>

%include "mgvector.h"
%template(floats) mgvector<float>;
%template(chars) mgvector<char>;

%include "GiCanvasBase.h"
%feature("director") GiCanvasBase;

%include "GiSkiaView.h"

%include <mgstorage.h>
%feature("director") MgStorageBase;
