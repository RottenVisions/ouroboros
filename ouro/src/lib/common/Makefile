LIB =	common

SRCS =				\
	blowfish		\
	common			\
	tasks			\
	timer			\
	timestamp		\
	strutil			\
	ouroversion		\
	ourokey			\
	md5				\
	sha1			\
	ssl				\
	base64			\
	rsa				\
	memorystream

ifndef OURO_ROOT
export OURO_ROOT := $(subst /kbe/src/lib/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/kbe/src/build/common.mak