LIB =	db_interface

SRCS =				\
	db_interface		\
	db_tasks		\
	db_threadpool		\
	entity_table		\
	ouro_tables

USE_PYTHON = 1
USE_G3DMATH=1
USE_OPENSSL = 1

ifndef OURO_ROOT
export OURO_ROOT := $(subst /kbe/src/lib/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/kbe/src/build/common.mak
OURO_INCLUDES += -I $(OURO_ROOT)/kbe/src/lib/dependencies/hiredis