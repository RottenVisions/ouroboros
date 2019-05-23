
all:: config.status

CONFIG_ARGS = --with-openssl=$(OURO_ROOT)/kbe/src/lib/dependencies/openssl

ifeq ($(pythonbuild), release)
	CONFIG_ARGS += --enable-optimizations 
else
ifneq (,$(findstring 64,$(OURO_CONFIG)))
	CONFIG_ARGS += --enable-optimizations 
endif
endif

config.status:
	@chmod -R 755 .
	export OURO_ROOT=$(OURO_ROOT)
	$(SHELL) configure $(CONFIG_ARGS)
	@make

.PHONY: clean

clean::
	echo "clean"

# KBE modifys:
# 1: PCbuild vcxprojs to KBE-vcxproj

# 2: configure
# OPENSSL_LDFLAGS="-L${OURO_ROOT}/kbe/src/libs"

# 3:Modules/Setup.dist

# 4: setup.py
#       system_lib_dirs = ['/lib64', '/usr/lib64', '/lib', '/usr/lib', '$(OURO_ROOT)/kbe/src/libs']
#       system_include_dirs = ['/usr/include', '$(OURO_ROOT)/kbe/src/lib/dependencies/zlib']

