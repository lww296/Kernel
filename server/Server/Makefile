
include ../../config.mk

TARGET := service

SOURCE = $(wildcard ./*.cpp)
OBJS=$(SOURCE:.cpp=.o)

INCLUDE = ../../include/
LUACPP_INC = ../../service_lua
LINUX_INC = ../../include/linux
LUA_INC = ../../include/linux/lua
INC := -I $(INCLUDE) -I $(LUA_INC) -I $(LINUX_INC) -I $(LUACPP_INC) -I ./

CCFLAGS += $(INC) -D_OPEN_LUA_MODULE_

ifeq ($(BITS), 32)
	LIBPATH = ../../lib/$(LIB_VER)
else
	LIBPATH = ../../lib/$(LIB_VER)_x64
endif

SERLIB := -L $(LIBPATH) -lkernel -lluacppmodule -lluabindd -lprotobuf -llua -ljson 

# third lib
ifeq ($(THIRDLIB), yes)
	SERLIB += -L $(LIBPATH) -lz -lcurl 
endif

all:
	make server

server:$(TARGET)
$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(SERLIB)  

	
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
	@rm -f $(OBJS)
	@rm -f ./*.o
