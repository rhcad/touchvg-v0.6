LOCAL_PATH := $(call my-dir)
VCORE_PATH := $(LOCAL_PATH)../../../core

include $(CLEAR_VARS)

LOCAL_MODULE        := skiaview

LOCAL_C_INCLUDES    := $(VCORE_PATH)/include/geom \
                       $(VCORE_PATH)/include/graph \
                       $(VCORE_PATH)/include/shape \
                       $(VCORE_PATH)/include/skiaview

LOCAL_DEFAULT_CPP_EXTENSION:= .cc

LOCAL_SRC_FILES := $(VCORE_PATH)/src/skiaview/skiaview_java_wrap.cc \
				   $(VCORE_PATH)/src/skiaview/GiSkiaView.cc \
				   $(VCORE_PATH)/src/skiaview/GiSkiaCanvas.cc \
				   $(VCORE_PATH)/src/skiaview/GiCmdController.cc \
				   $(VCORE_PATH)/src/shape/mgcmddraw.cc \
				   $(VCORE_PATH)/src/shape/mgcmds.cc \
				   $(VCORE_PATH)/src/shape/mgcmdselect.cc \
				   $(VCORE_PATH)/src/shape/mgcmderase.cc \
				   $(VCORE_PATH)/src/shape/mgcmdmgr.cc \
				   $(VCORE_PATH)/src/shape/mgdrawline.cc \
				   $(VCORE_PATH)/src/shape/mgdrawlines.cc \
				   $(VCORE_PATH)/src/shape/mgdrawrect.cc \
				   $(VCORE_PATH)/src/shape/mgdrawsplines.cc \
				   $(VCORE_PATH)/src/shape/mgellipse.cc \
				   $(VCORE_PATH)/src/shape/mgline.cc \
				   $(VCORE_PATH)/src/shape/mglines.cc \
				   $(VCORE_PATH)/src/shape/mgrdrect.cc \
				   $(VCORE_PATH)/src/shape/mgrect.cc \
				   $(VCORE_PATH)/src/shape/mgshape.cc \
				   $(VCORE_PATH)/src/shape/mgsplines.cc \
				   $(VCORE_PATH)/src/graph/gipath.cc \
				   $(VCORE_PATH)/src/graph/gixform.cc \
				   $(VCORE_PATH)/src/graph/gigraph.cc \
				   $(VCORE_PATH)/src/geom/mgmat.cc \
				   $(VCORE_PATH)/src/geom/mgbase.cc \
				   $(VCORE_PATH)/src/geom/mgbnd.cc \
				   $(VCORE_PATH)/src/geom/mgbox.cc \
				   $(VCORE_PATH)/src/geom/mgcurv.cc \
				   $(VCORE_PATH)/src/geom/mglnrel.cc \
				   $(VCORE_PATH)/src/geom/mgnear.cc \
				   $(VCORE_PATH)/src/geom/mgnearbz.cc \
				   $(VCORE_PATH)/src/geom/mgvec.cc

LOCAL_LDLIBS    := -L$(SYSROOT)/usr/lib -llog
LOCAL_CFLAGS    := -frtti

include $(BUILD_SHARED_LIBRARY)
