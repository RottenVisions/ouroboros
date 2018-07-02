ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/server/tools,,$(CURDIR))
endif

all clean realclean all_config install unit_tests unit_tests_build unit_tests_run unit_tests_clean::

# Check with and without the trailing slash
ifneq ($(CURDIR),$(OURO_ROOT)/ouro/src/server/tools)
ifneq ($(CURDIR),$(OURO_ROOT)ouro/src/server/tools)
	@echo 'Error: OURO_ROOT=$(OURO_ROOT)'
	@echo '       is not the root of $(CURDIR)'
	@echo 'Remove the OURO_ROOT environment variable.'
	@false
endif
endif

	$(MAKE) -C ../../lib $@
	$(MAKE) -C bots $@
	$(MAKE) -C interfaces $@
	$(MAKE) -C logger $@
	$(MAKE) -C obcmd $@

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
