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
	
COBJS     = lib/hid-libusb.o
CPPOBJS   = src/main.o
CPPOBJS   += src/mfsConfig.o
OBJS      = $(COBJS) $(CPPOBJS)
LDFLAGS   = -fPIC
LIBS      = -L$HOME/chroot/usr/lib64 -lusb-1.0 -ludev -lrt -lpthread
INCLUDES ?= -I include/ -I $HOME/chroot/usr/include/ -I $HOME/chroot/usr/include/libusb-1.0/

melfas_update_tool: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o melfas_update_tool

$(COBJS): %.o: %.c
	pwd
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJS) melfas_update_tool

.PHONY: clean
