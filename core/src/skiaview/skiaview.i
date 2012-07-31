%module(directors="1") skiaview
%{
#include <GiSkiaView.h>
#include <mgstorage.h>
%}

%include <GiSkiaView.h>

%include <mgstorage.h>
%feature("director") MgStorageBase;
