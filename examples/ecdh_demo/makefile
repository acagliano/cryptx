# ----------------------------
# Makefile Options
# ----------------------------

NAME ?= DEMO
ICON ?= icon.png
DESCRIPTION ?= "CE C Toolchain Demo"
COMPRESSED ?= NO
ARCHIVED ?= NO

CFLAGS ?= -Wall -Wextra -Oz
CXXFLAGS ?= -Wall -Wextra -Oz
DEBUGMODE = DEBUG

HAS_PRINTF = no

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
