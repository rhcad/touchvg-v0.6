ROOTDIR  =../../..
PKGNAME  =vg
PROJNAME =graph

INCLUDES += -I$(ROOTDIR)/core/include/geom \
            -I$(ROOTDIR)/core/include/graph

LIBS     += $(LIBFLAG)geom$(LIBEND)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
