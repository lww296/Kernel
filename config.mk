
#编译版本模式
VER =
ifeq ($(VER),)
    VER := debug
endif

#操作系统位数
#BITS = 32
BITS = 64

GCC := gcc
GXX := g++
LD := ld

CCFLAGS = -c -Wall -rdynamic -fvisibility=hidden

# load third lib (yes or no)
THIRDLIB = no
ifeq ($(THIRDLIB),)
    THIRDLIB := yes
endif

# 模块选择，默认编译服务器模块
MODULE = 
ifeq ($(MODULE),)
	MODULE := server
endif

#debug or release
ifeq ($(VER), debug)
	CCFLAGS += -g -DDEBUG -Werror
else
	CCFLAGS += -static -O3 -DNDEBUG
endif

#lib path
ifeq ($(VER), debug)
	LIB_VER = debug
else
	LIB_VER = release
endif

#32位 or 64位
ifeq ($(BITS), 32)
	CCFLAGS += -m32
else
	CCFLAGS += -m64
endif

ifeq ($(MODULE), server)
	CCFLAGS += -D_OPEN_DB_MODULE_ -D__MYSQL_DB_MODULE__ -D__REDIS_DB_MODULE__ -D__MONGODB_DB_MODULE__
endif

# load thirdlib define
ifeq ($(THIRDLIB), yes)
	CCFLAGS += -D_OPEN_ENCRYPT_MODULE_ -D_OPEN_COMPRESS_MODULE_ -D_OPEN_WEB_CLIENT_MODULE_
endif

