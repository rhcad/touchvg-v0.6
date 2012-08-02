LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := skiaview

LOCAL_C_INCLUDES    := $(LOCAL_PATH)/include/geom \
                       $(LOCAL_PATH)/include/graph \
                       $(LOCAL_PATH)/include/shape \
                       $(LOCAL_PATH)/include/skiaview \

LOCAL_SRC_FILES := $(LOCAL_PATH)/src/skiaview/skiaview_java_wrap.cxx \
                   $(LOCAL_PATH)/src/skiaview/GiSkiaView.cpp \
                   $(LOCAL_PATH)/src/skiaview/GiSkiaCanvas.cpp \
                   $(LOCAL_PATH)/src/skiaview/GiCmdController.cpp \
                   $(LOCAL_PATH)/src/shape/mgcmddraw.cpp \
                   $(LOCAL_PATH)/src/shape/mgcmds.cpp \
                   $(LOCAL_PATH)/src/shape/mgcmdselect.cpp \
                   $(LOCAL_PATH)/src/shape/mgcmderase.cpp \
                   $(LOCAL_PATH)/src/shape/mgcmdmgr.cpp \
                   $(LOCAL_PATH)/src/shape/mgdrawline.cpp \
                   $(LOCAL_PATH)/src/shape/mgdrawlines.cpp \
                   $(LOCAL_PATH)/src/shape/mgdrawrect.cpp \
                   $(LOCAL_PATH)/src/shape/mgdrawsplines.cpp \
                   $(LOCAL_PATH)/src/shape/mgellipse.cpp \
                   $(LOCAL_PATH)/src/shape/mgline.cpp \
                   $(LOCAL_PATH)/src/shape/mglines.cpp \
                   $(LOCAL_PATH)/src/shape/mgrdrect.cpp \
                   $(LOCAL_PATH)/src/shape/mgrect.cpp \
                   $(LOCAL_PATH)/src/shape/mgshape.cpp \
                   $(LOCAL_PATH)/src/shape/mgsplines.cpp \
                   $(LOCAL_PATH)/src/graph/gipath.cpp \
                   $(LOCAL_PATH)/src/graph/gixform.cpp \
                   $(LOCAL_PATH)/src/graph/gigraph.cpp \
                   $(LOCAL_PATH)/src/geom/mgmat.cpp \
                   $(LOCAL_PATH)/src/geom/mgbase.cpp \
                   $(LOCAL_PATH)/src/geom/mgbnd.cpp \
                   $(LOCAL_PATH)/src/geom/mgbox.cpp \
                   $(LOCAL_PATH)/src/geom/mgcurv.cpp \
                   $(LOCAL_PATH)/src/geom/mglnrel.cpp \
                   $(LOCAL_PATH)/src/geom/mgnear.cpp \
                   $(LOCAL_PATH)/src/geom/mgnearbz.cpp \
                   $(LOCAL_PATH)/src/geom/mgvec.cpp

include $(BUILD_SHARED_LIBRARY)
