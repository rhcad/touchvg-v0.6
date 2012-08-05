%module(directors="1") skiaview
%{
#include "GiSkiaView.h"
#include <mgstorage.h>
%}

%include ../geom/geom.i
%include ../graph/graph.i
%include ../shape/shape.i

%include "GiSkiaView.h"

%include <mgstorage.h>
%feature("director") MgStorageBase;
