%module(directors="1") skiaview
%{
#include "GiSkiaView.h"
#include "GiCanvasBase.h"
#include <mgstorage.h>
%}

%include <mgtype.h>
%include <mgvec.h>
%include <mgpnt.h>
%include <mgmat.h>
%include <mgbox.h>
%include <gicolor.h>
%include <gicontxt.h>
%include <gixform.h>
%include <gicanvdr.h>
%include <gicanvas.h>
%include <gigraph.h>

%include "mgvector.h"
%template(Floats) mgvector<float>;
%template(Chars) mgvector<char>;

%feature("director") GiCanvasBase;
%include "GiCanvasBase.h"

%include "GiSkiaView.h"

%feature("director") MgStorageBase;
%include <mgstorage.h>
