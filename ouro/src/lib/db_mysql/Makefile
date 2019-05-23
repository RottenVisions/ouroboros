LIB =	db_mysql

SRCS =					\
	common				\
	db_exception			\
	db_transaction			\
	db_interface_mysql		\
	entity_table_mysql		\
	entity_sqlstatement_mapping	\
	ouro_table_mysql

USE_PYTHON = 1
USE_G3DMATH = 1

ifndef OURO_ROOT
export OURO_ROOT := $(subst /kbe/src/lib/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/kbe/src/build/common.mak
