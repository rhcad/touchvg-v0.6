ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =json

INCLUDES += -I$(ROOTDIR)/core/include/shape

SOURCES   =$(wildcard *.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
