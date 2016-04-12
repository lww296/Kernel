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

CCFLAGS = -c -Wall -rdynamic

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

TARGET := libluacppmodule

SOURCE = $(wildcard ./*.cpp)
OBJS = $(SOURCE:.cpp=.o)

INCLUDE = ../include/
LIN_INC = ../include/linux
LUA_INC = ../include/linux/lua
LUABIND_INC = ../include/linux/luabind
INC := -I $(INCLUDE) -I $(LIN_INC) -I $(LUA_INC) -I $(LUABIND_INC) -I ./ 

CCFLAGS += $(INC) -D_OPEN_LUA_MODULE_

ifeq ($(BITS), 32)
        LIBPATH = ../lib/$(LIB_VER)
else
        LIBPATH = ../lib/$(LIB_VER)_x64
endif

ifeq ($(LIB_VER), debug)
	LIBS := -L $(LIBPATH) -lluabindd
else
	LIBS := -L $(LIBPATH) -lluabind
endif

all:$(TARGET)
$(TARGET) : $(OBJS)
	@$(CXX) -fPIC --shared $(OBJS) $(LIBS) -o $(LIBPATH)/$(TARGET:=.so)
	@echo " Build    $(TARGET:=.so)"

%.o : %.cpp
	@$(CXX) $< -o $@ $(CCFLAGS) -fPIC 
	@echo "  XX		$<"
	@echo "  LD		$@"

%.o : %.cc
	@$(CXX) $< -o $@ $(CCFLAGS) -fPIC 
	@echo "  XX		$<"
	@echo "  LD		$@"


clean:
	@rm -f $(TARGET)
	@rm -f ./*.o
