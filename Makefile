###############################################
# Makefile for melfas firmware update program
#
# jsj1012@melfas.com
# melfas
# 2019-06-12
###############################################

all: melfas_update_tool

CC       ?= gcc
CFLAGS   ?= -g

CXX      ?= g++ 
CXXFLAGS ?= -g

PKG_CONFIG ?= pkg-config
BASE_VER ?= 462023

LIB_DEPS := libchrome-$(BASE_VER) libusb-1.0 libudev
PKG_CONFIG_INCLUDE := $(shell $(PKG_CONFIG) --cflags $(LIB_DEPS)) 
PKG_CONFIG_LIB := $(shell $(PKG_CONFIG) --libs  $(LIB_DEPS)) 
	
COBJS     = lib/hid.o
CPPOBJS   = src/main.o
CPPOBJS   += src/mfsConfig.o
OBJS      = $(COBJS) $(CPPOBJS)
LDFLAGS   = -fPIC
LIBS      = -lrt -lpthread ${PKG_CONFIG_LIB} 
INCLUDES ?= -I include/ ${PKG_CONFIG_INCLUDE}

melfas_update_tool: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o melfas_update_tool

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJS) melfas_update_tool

.PHONY: clean
