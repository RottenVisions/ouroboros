BIN  = baseappmgr
SRCS =						\
	baseapp					\
	baseappmgr				\
	baseappmgr_interface			\
	main

ASMS =

MY_LIBS =		\
	server		\
	network		\
	thread
	

BUILD_TIME_FILE = main
USE_PYTHON = 1
USE_G3DMATH = 1
USE_OPENSSL = 1


ifndef NO_USE_LOG4CXX
	NO_USE_LOG4CXX = 0
	CPPFLAGS += -DLOG4CXX_STATIC
endif

#HAS_PCH = 1
CPPFLAGS += -DOURO_BASEAPPMGR

ifndef OURO_ROOT
export OURO_ROOT := $(subst /kbe/src/server/$(BIN),,$(CURDIR))
endif

include $(OURO_ROOT)/kbe/src/build/common.mak

