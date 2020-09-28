
#ARCH: linux/arm/android/ios/win
ARCH		?= linux
CROSS_PREFIX	?=
OUTPUT		?= /usr/local
BUILD_DIR	:= $(shell pwd)/../../build/
ARCH_INC	:= $(BUILD_DIR)/$(ARCH).inc
COLOR_INC	:= $(BUILD_DIR)/color.inc

ifeq ($(ARCH_INC), $(wildcard $(ARCH_INC)))
include $(ARCH_INC)
endif

CC	= $(CROSS_PREFIX)gcc
CXX	= $(CROSS_PREFIX)g++
LD	= $(CROSS_PREFIX)ld
AR	= $(CROSS_PREFIX)ar
STRIP   = $(CROSS_PREFIX)strip

ifeq ($(COLOR_INC), $(wildcard $(COLOR_INC)))
include $(COLOR_INC)
else
CC_V	= $(CC)
CXX_V	= $(CXX)
LD_V	= $(LD)
AR_V	= $(AR)
CP_V	= $(CP)
RM_V	= $(RM)
endif

###############################################################################
# target and object
###############################################################################
LIBNAME		= $(notdir $(shell  pwd))
VERSION_SH	= $(shell pwd)/version.sh $(LIBNAME)
VER		= $(shell $(VERSION_SH); awk '/define\ $(LIBNAME)_version/{print $$3}' version.h)
TGT_LIB_H	= $(LIBNAME).h
TGT_LIB_A	= $(LIBNAME).a

OBJS_LIB	= $(LIBNAME).o

###############################################################################
# cflags and ldflags
###############################################################################
ifeq ($(MODE), release)
CFLAGS	:= -O2 -Wall 
LTYPE   := release
else
CFLAGS	:= -g -Wall 
LTYPE   := debug
endif
ifeq ($(OUTPUT),/usr/local)
OUTLIBPATH := /usr/local
else
OUTLIBPATH := $(OUTPUT)/$(LTYPE)
endif
ifeq ($(ARCH), win)
else
CFLAGS	+= -fPIC -std=gnu1x 
endif
CFLAGS	+= $($(ARCH)_CFLAGS)
CFLAGS	+= -I$(INC_DIR)
CFLAGS	+= -I$(OUTPUT)/include


###############################################################################
# target
###############################################################################
.PHONY : all clean

TGT	:= $(TGT_LIB_A)
#####TGT	+= $(TGT_UNIT_TEST)

OBJS	:= $(OBJS_LIB)

all: $(TGT)

%.o:%.c
	$(CC_V) -c $(CFLAGS) $< -o $@

$(TGT_LIB_A): $(OBJS_LIB)
	$(AR_V) rcs $@ $^

clean:
	$(RM_V) -f $(OBJS)
	$(RM_V) -f $(TGT)
