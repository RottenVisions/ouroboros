LIB =	common

SRCS =				\
	blowfish		\
	common			\
	format			\
	tasks			\
	timer			\
	timestamp		\
	strutil			\
	ouroversion		\
	ourokey			\
	md5				\
	sha1			\
	base64			\
	rsa				\
	memorystream

ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/lib/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/ouro/src/build/common.mak