ROOTDIR  =../../..
PKGNAME  =vg
PROJNAME =shape

INCLUDES += -I$(ROOTDIR)/core/include/geom \
            -I$(ROOTDIR)/core/include/graph \
            -I$(ROOTDIR)/core/include/shape

LIBS     += $(LIBFLAG)geom$(LIBEND) $(LIBFLAG)graph$(LIBEND)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
