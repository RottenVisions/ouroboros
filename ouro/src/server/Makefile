ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/server,,$(CURDIR))
endif

all clean realclean all_config install::

# Check with and without the trailing slash
ifneq ($(CURDIR),$(OURO_ROOT)/ouro/src/server)
ifneq ($(CURDIR),$(OURO_ROOT)ouro/src/server)
	@echo 'Error: OURO_ROOT=$(OURO_ROOT)'
	@echo '       is not the root of $(CURDIR)'
	@echo 'Remove the OURO_ROOT environment variable.'
	@false
endif
endif

	$(MAKE) -C ../lib $@
	$(MAKE) -C baseapp $@
	$(MAKE) -C baseappmgr $@
	$(MAKE) -C cellapp $@
	$(MAKE) -C cellappmgr $@
	$(MAKE) -C dbmgr $@
	$(MAKE) -C loginapp $@
	$(MAKE) -C machine $@
	$(MAKE) -C tools $@

ifdef OURO_CONFIG
	@echo completed $@ \(OURO_CONFIG = $(OURO_CONFIG)\)
else
	@echo completed $@
endif
	$(MAKE) done

done:
ifdef DO_NOT_BELL
else
	echo -n 
endif

server:
	$(MAKE) all
