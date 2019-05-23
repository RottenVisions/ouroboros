BIN  = obcmd
SRCS =					\
	client_sdk			\
	client_sdk_unity	\
	client_sdk_ue4		\
	server_assets		\
	entity_component	\
	obcmd				\
	obcmd_interface		\
	profile				\
	main

ASMS =

MY_LIBS =		\
	entitydef	\
	server		\
	network		\
	pyscript	\
	thread		
	

BUILD_TIME_FILE = main
USE_G3DMATH = 1
USE_OPENSSL = 1
USE_PYTHON = 1


ifndef NO_USE_LOG4CXX
	NO_USE_LOG4CXX = 0
	CPPFLAGS += -DLOG4CXX_STATIC
endif

#HAS_PCH = 1
CPPFLAGS += -DOURO_CMD

ifndef OURO_ROOT
export OURO_ROOT := $(subst /kbe/src/server/tools/$(BIN),,$(CURDIR))
endif

include $(OURO_ROOT)/kbe/src/build/common.mak

