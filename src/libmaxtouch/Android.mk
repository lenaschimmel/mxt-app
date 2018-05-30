LOCAL_PATH := $(call my-dir)
PATH_TO_LIBUSB_SRC := /home/ameno/src/libusb
include $(PATH_TO_LIBUSB_SRC)/android/jni/libusb.mk
include $(CLEAR_VARS)
LOCAL_PATH := /home/ameno/src/mxt-app/src/libmaxtouch

LOCAL_C_INCLUDES := $(TOP_DIR)/src $(TOP_DIR)/lib/libusbdroid/code/src 
LOCAL_CFLAGS += -DHAVE_LIBUSB -DMXT_VERSION=\"$(GIT_VERSION)\"
LOCAL_SRC_FILES := \
  libmaxtouch.c \
  log.c \
  msg.c \
  config.c \
  utilfuncs.c \
  info_block.c \
  sysfs/sysfs_device.c \
  sysfs/dmesg.c \
  i2c_dev/i2c_dev_device.c \
  hidraw/hidraw_device.c \
  usb/usb_device.c
LOCAL_MODULE := maxtouch
#LOCAL_STATIC_LIBRARIES := libusbdroid

LOCAL_SHARED_LIBRARIES += libusb1.0

include $(BUILD_STATIC_LIBRARY)
