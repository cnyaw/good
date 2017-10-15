#
#  Android.mk
#  Android project makefile.
#
#  Copyright (c) 2014 Waync Cheng.
#  All Rights Reserved.
#
#  2014/7/6 Waync created
#

LOCAL_PATH:= $(call my-dir)

SW2_INC         := ../../../../smallworld2/include
SW2_SRC_DIR     := ../../../../smallworld2/src
SW2_SRC_0       := swArchive.cpp swIni.cpp swTraceTool.cpp swUtil.cpp swZipUtil.cpp
SW2_SRC         := $(addprefix $(SW2_SRC_DIR)/, $(SW2_SRC_0))

GOOD_INC        := ../../../src/good
YARD_INC        := ../../../../../open/yard-1.5

LUA_SRC_DIR     := ../../../../../open/lua-5.1.4/src
LUA_SRCS_0      := lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c \
                   lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c \
                   lundump.c lvm.c lzio.c lauxlib.c lbaselib.c ldblib.c liolib.c \
                   lmathlib.c loslib.c ltablib.c lstrlib.c loadlib.c linit.c
LUA_SRCS        := $(addprefix $(LUA_SRC_DIR)/, $(LUA_SRCS_0))

include $(CLEAR_VARS)

LOCAL_MODULE    := libgood
LOCAL_CFLAGS    := -Wall -Werror -D_linux_ -DLUA_ANSI -D_android_ -fexceptions
LOCAL_SRC_FILES := gl_code.cpp $(SW2_SRC) $(LUA_SRCS)
                   
LOCAL_LDLIBS    := -llog -lGLESv1_CM -lz -landroid

LOCAL_C_INCLUDES := $(SW2_INC) $(GOOD_INC) $(LUA_SRC_DIR) $(YARD_INC)

include $(BUILD_SHARED_LIBRARY)
