%module shape
%{
#include <gigraph.h>
#include <mgshapes.h>
#include <mgbasicsp.h>
#include <mgstorage.h>
#include <mgstoragebs.h>
%}

%include <mgtol.h>
%include <mgvec.h>
%include <mgpnt.h>
%include <mgbox.h>
%include <mgmat.h>

%include <gidef.h>
%include <gicolor.h>
%include <gicontxt.h>
%include <gixform.h>
%include <gigraph.h>

%include <mgvector.h>
%template(Floats) mgvector<float>;
%template(Chars) mgvector<char>;
%template(Ints) mgvector<int>;

%include <mgobject.h>
%include <mgshape.h>
%include <mgshapes.h>
%include <mgbasicsp.h>
%include <mgstorage.h>
%include <mgstoragebs.h>
