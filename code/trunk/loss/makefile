CC = gcc
CXX = g++
CFLAG = -Wall -Wno-format -g -O2 -fPIC
CXXFLAG = -Wall -Wno-format -g -O2 -fPIC

ARCH:=$(shell uname -m)

ARCH32:=i686
ARCH64:=x86_64

ifeq ($(ARCH),$(ARCH64))
SYSTEM_LIB_PATH:=/usr/lib64
else
SYSTEM_LIB_PATH:=/usr/lib
endif

LIB3RD_PATH = ../l3oss
LOSS_PATH = .


VPATH = $(LOSS_PATH)/src
SUB_DIRS := $(foreach dir, $(VPATH), $(shell find $(dir) -maxdepth 5 -type d))
DIRS := $(SUB_DIRS)

INC := $(INC) \
       -I /usr/include \
       -I $(LIB3RD_PATH)/include \
       -I $(LIB3RD_PATH)/include/mysql \
       -I $(LIB3RD_PATH)/include/jemalloc \
       -I $(LOSS_PATH)/src

LDFLAGS := $(LDFLAGS) -D_LINUX_OS_ \
           -L$(LIB3RD_PATH)/lib -lprotobuf \
           -L$(LIB3RD_PATH)/lib -llog4cplus \
           -L$(LIB3RD_PATH)/lib -lev \
           -L$(LIB3RD_PATH)/lib -ljemalloc \
           -L$(SYSTEM_LIB_PATH) -lpthread \
           -L$(SYSTEM_LIB_PATH) -ldl -lrt

CPP_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
CC_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
C_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
S_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.s))
OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS)) $(patsubst %.cc,%.o,$(CC_SRCS)) $(patsubst %.c,%.o,$(C_SRCS)) $(patsubst %.s,%.o,$(S_SRCS)) 

LIBRARYS: $(OBJS)
	ar -crv libloss.a $(OBJS)
	ranlib libloss.a
	$(CXX) -shared -Wl,-soname,libloss.so -o libloss.so $(OBJS) -lc -lrt -ldl $(LDFLAGS)
	mv libloss.a $(LOSS_PATH)/lib/
	mv libloss.so $(LOSS_PATH)/lib/

%.o:%.cpp
	$(CXX) $(INC) $(CXXFLAG) -c -o $@ $< 
%.o:%.c
	$(CXX) $(INC) $(CXXFLAG) -c -o $@ $< 
#%.o:%.c
	#$(CC) $(INC) $(CFLAG) -c -o $@ $< 
#%.o:%.s
	#$(CC) $(INC) $(CFLAG) -c -o $@ $< 
clean:
	rm -f $(OBJS)
	rm -f $(LIBRARYS) 
        
        
