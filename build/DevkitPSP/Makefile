TARGET = good

LUA_SRC_DIR = ../../../../open/lua-5.1.4/src
LUA_SRCS = lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c \
  lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c \
  lundump.c lvm.c lzio.c lauxlib.c lbaselib.c ldblib.c liolib.c \
  lmathlib.c loslib.c ltablib.c lstrlib.c loadlib.c linit.c
LUA_OBJS = $(addprefix $(LUA_SRC_DIR)/, $(LUA_SRCS:.c=.o))

SW2_SRC_DIR = ../../../smallworld2/src
SW2_SRCS = swArchive.cpp swIni.cpp swTraceTool.cpp swUtil.cpp swZipUtil.cpp
SW2_OBJS = $(addprefix $(SW2_SRC_DIR)/, $(SW2_SRCS:.cpp=.o))

OBJS = $(LUA_OBJS) $(SW2_OBJS) main.o

CFLAGS = -O2 -G0 -Wall -DDEVKIT_PSP
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CXXFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Good devkitPSP 1.6.1
PSP_EBOOT_ICON = icon.png

LIBS = -lpspgum -lpspgu -lm -lstdc++ -lz -lpng -ljpeg
INCDIR = ../../../../open/yard-1.5 ../../../../open/lua-5.1.4/src ../../../smallworld2/include ../../../stge ../../src/good

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
