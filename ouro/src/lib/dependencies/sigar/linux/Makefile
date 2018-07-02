LIB =	sigar

CSRCS =				\
	sigar			\
	sigar_cache		\
	sigar_fileinfo		\
	sigar_format		\
	sigar_getline		\
	sigar_ptql		\
	sigar_signal		\
	sigar_util		\
	linux_sigar

ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/lib/dependencies/$(LIB)/linux,,$(CURDIR))
endif

include $(OURO_ROOT)/ouro/src/build/common.mak