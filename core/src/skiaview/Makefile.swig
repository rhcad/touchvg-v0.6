ROOTDIR  =../../..
PKGNAME  =core
PROJNAME =skiaview

INCLUDES += -I$(ROOTDIR)/core/include/geom \
            -I$(ROOTDIR)/core/include/graph \
            -I$(ROOTDIR)/core/include/shape \
            -I$(ROOTDIR)/core/include/skiaview

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../geom/*.cpp) \
           $(wildcard ../graph/*.cpp) \
           $(wildcard ../shape/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
