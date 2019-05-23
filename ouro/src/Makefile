ifndef OURO_ROOT
export OURO_ROOT := $(subst /kbe/src,,$(CURDIR))
endif


all clean:
	$(MAKE) -C lib $@
	$(MAKE) -C server $@

cleanall:
	$(MAKE) clean

