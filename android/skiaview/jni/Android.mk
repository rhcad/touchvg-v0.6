LOCAL_PATH := $(call my-dir)
VCORE_PATH := $(LOCAL_PATH)/../../../core
SRC_PATH   := ../../../core/src

include $(CLEAR_VARS)

LOCAL_MODULE := skiaview
LOCAL_SHARED_LIBRARIES := libcutils libdl libstlport
LOCAL_PRELINK_MODULE   := false
LOCAL_CFLAGS           := -frtti -g  -Wall -Wextra

ifeq ($(TARGET_ARCH),arm)
# Ignore "note: the mangling of 'va_list' has changed in GCC 4.4"
LOCAL_CFLAGS += -Wno-psabi
endif

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif

LOCAL_C_INCLUDES := $(VCORE_PATH)/include/geom \
                    $(VCORE_PATH)/include/graph \
                    $(VCORE_PATH)/include/shape \
                    $(VCORE_PATH)/include \
                    $(VCORE_PATH)/src/shape \
                    $(VCORE_PATH)/src/json

LOCAL_SRC_FILES  := GiCoreView.cpp \
                    GiCanvasBase.cpp \
                    skiaview_java_wrap.cpp \
                    $(SRC_PATH)/json/mgjsonstorage.cpp \
                    $(SRC_PATH)/geom/mgmat.cpp \
                    $(SRC_PATH)/geom/mgbase.cpp \
                    $(SRC_PATH)/geom/mgbnd.cpp \
                    $(SRC_PATH)/geom/mgbox.cpp \
                    $(SRC_PATH)/geom/mgcurv.cpp \
                    $(SRC_PATH)/geom/mglnrel.cpp \
                    $(SRC_PATH)/geom/mgnear.cpp \
                    $(SRC_PATH)/geom/mgnearbz.cpp \
                    $(SRC_PATH)/geom/mgvec.cpp \
                    $(SRC_PATH)/graph/gipath.cpp \
                    $(SRC_PATH)/graph/gixform.cpp \
                    $(SRC_PATH)/graph/gigraph.cpp \
                    $(SRC_PATH)/shape/mgcmddraw.cpp \
                    $(SRC_PATH)/shape/mgcmds.cpp \
                    $(SRC_PATH)/shape/mgcmdselect.cpp \
                    $(SRC_PATH)/shape/mgcmderase.cpp \
                    $(SRC_PATH)/shape/mgcmdmgr.cpp \
                    $(SRC_PATH)/shape/mgactions.cpp \
                    $(SRC_PATH)/shape/mgdrawline.cpp \
                    $(SRC_PATH)/shape/mgdrawlines.cpp \
                    $(SRC_PATH)/shape/mgdrawrect.cpp \
                    $(SRC_PATH)/shape/mgdrawsplines.cpp \
                    $(SRC_PATH)/shape/mgdrawtriang.cpp \
                    $(SRC_PATH)/shape/mgellipse.cpp \
                    $(SRC_PATH)/shape/mgline.cpp \
                    $(SRC_PATH)/shape/mglines.cpp \
                    $(SRC_PATH)/shape/mgrdrect.cpp \
                    $(SRC_PATH)/shape/mgrect.cpp \
                    $(SRC_PATH)/shape/mggrid.cpp \
                    $(SRC_PATH)/shape/mgshape.cpp \
                    $(SRC_PATH)/shape/mgsplines.cpp

include $(BUILD_SHARED_LIBRARY)
