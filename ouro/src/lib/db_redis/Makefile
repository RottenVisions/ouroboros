LIB =	db_redis

SRCS =					\
	common				\
	db_exception			\
	db_transaction			\
	db_interface_redis		\
	entity_table_redis		\
	ouro_table_redis			\
	redis_watcher

USE_PYTHON = 1
USE_G3DMATH = 1

ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/lib/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/ouro/src/build/common.mak
OURO_INCLUDES += -I $(OURO_ROOT)/ouro/src/lib/dependencies/hiredis