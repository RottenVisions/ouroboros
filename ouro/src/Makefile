ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src,,$(CURDIR))
endif


all clean:
	$(MAKE) -C lib $@
	$(MAKE) -C server $@

ARCH=
ifeq ($(shell uname -m),x86_64)
	 ARCH=64
endif

all_configs:
	$(MAKE) OURO_CONFIG=Hybrid$(ARCH) all
	$(MAKE) OURO_CONFIG=Debug$(ARCH) all


all_configs_clean:
	$(MAKE) OURO_CONFIG=Hybrid$(ARCH) cleanall
	$(MAKE) OURO_CONFIG=Debug$(ARCH) cleanall


coverage:
	$(MAKE) OURO_CONFIG=Debug$(ARCH)_GCOV all


coverage_clean:
	$(MAKE) OURO_CONFIG=Debug$(ARCH)_GCOV cleanall


cleanall:
	$(MAKE) clean

