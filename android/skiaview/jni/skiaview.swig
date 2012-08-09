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

%include "GiCanvasBase.h"
%feature("director") GiCanvasBase;

%include "GiSkiaView.h"

%include <mgstorage.h>
%feature("director") MgStorageBase;
