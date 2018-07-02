LIB = zip

CSRCS =					\
	adler32				\
	compress			\
	crc32				\
	deflate				\
	gzio				\
	infback				\
	inffast				\
	inflate				\
	inftrees			\
	trees				\
	uncompr				\
	zutil

ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/lib/dependencies/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/ouro/src/build/common.mak