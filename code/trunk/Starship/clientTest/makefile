CC = gcc
CXX = g++
CXXFLAG = -Wall -Wno-format -g -O2 

ARCH:=$(shell uname -m)

ARCH32:=i686
ARCH64:=x86_64

ifeq ($(ARCH),$(ARCH64))
SYSTEM_LIB_PATH:=/usr/lib64
else
SYSTEM_LIB_PATH:=/usr/lib
endif

LIB3RD_PATH = ../../l3oss
LOSS_PATH = ../../loss
STARSHIP_PATH = ../../Starship
PROTO_PATH = ../../proto
DATA_PROXY_PATH = ../../DataProxyServer

VPATH = . ../src/protocol
SUB_DIRS := $(foreach dir, $(VPATH), $(shell find $(dir) -maxdepth 5 -type d))
DIRS := $(SUB_DIRS)


INC := $(INC) \
       -I $(LIB3RD_PATH)/include 


LDFLAGS := $(LDFLAGS) -D_LINUX_OS_ \
           -L$(LIB3RD_PATH)/lib -lprotobuf

CPP_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
CC_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
C_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS)) $(patsubst %.c,%.o,$(C_SRCS)) $(patsubst %.cc,%.o,$(CC_SRCS))

#TARGET: SimpleMsgTest
TARGET: ImClient 

#SimpleMsgTest: $(OBJS)
	#$(CXX) $(CXXFLAG) -o $@ $^ $(LDFLAGS)
ImClient: $(OBJS)
	$(CXX) $(CXXFLAG) -o $@ $^ $(LDFLAGS)
%.o:%.cpp
	$(CXX) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.cc
	$(CXX) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.c
	$(CC) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
clean:
	rm -f $(OBJS)
	rm -f ImClient
        
        
