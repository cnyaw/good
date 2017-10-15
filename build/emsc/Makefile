SW2_INC         := ../../../smallworld2/include
SW2_SRC_DIR     := ../../../smallworld2/src
SW2_SRC_0       := swArchive.cpp swIni.cpp swTraceTool.cpp swUtil.cpp swZipUtil.cpp
SW2_SRC         := $(addprefix $(SW2_SRC_DIR)/, $(SW2_SRC_0))

GOOD_INC        := ../../src/good
YARD_INC        := d:/open/yard-1.5

LUA_SRC_DIR     := d:/open/lua-5.1.4/src
LUA_SRCS_0      := lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c \
                   lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c \
                   lundump.c lvm.c lzio.c lauxlib.c lbaselib.c ldblib.c liolib.c \
                   lmathlib.c loslib.c ltablib.c lstrlib.c loadlib.c linit.c
LUA_SRCS        := $(addprefix $(LUA_SRC_DIR)/, $(LUA_SRCS_0))

all: lua.bc good.js good.html
	emcc $(LUA_SRCS) -I$(LUA_SRC_DIR) -o lua.bc
	em++ main.cpp lua.bc $(SW2_SRC) -I$(LUA_SRC_DIR) -I$(SW2_INC) -I$(GOOD_INC) -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_SDL=2 -s LEGACY_GL_EMULATION=1 -s ALLOW_MEMORY_GROWTH=1 -s GL_FFP_ONLY=1 -o good.html
