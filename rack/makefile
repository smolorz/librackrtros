KCONFIG_DIR=./config/kconfig
CC := gcc

ifneq ($(MAKECMDGOALS),help)
ifeq ($(srctree),)
ifneq ($(MAKEFILE_LIST),)
# Since 3.80, we can find out which Makefile is currently processed,
# and infere the location of the source tree using MAKEFILE_LIST.
srctree := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
else
ifeq ($(srctree),)
srctree := $(shell test -d config && pwd)
ifeq ($(srctree),)
$(error Please specify the location of your source tree: make srctree=...)
endif
endif
endif
endif
build_alias := $(shell $(srctree)/config/config.guess)
host_alias := $(shell  $(srctree)/config/arch2host.sh $(ARCH) $(build_alias))
override ARCH := $(shell echo $(host_alias)|cut -f1 -d-|sed -e s/^i.86/i386/ -e s/^powerpc/ppc/)
endif

ifeq ($(MAKECMDGOALS),)
confgui = $(shell test $(TERM) = emacs && echo xconfig || echo menuconfig)
forceconf = $(shell test -r .rackrtros_config -a -r .cfok || echo $(confgui))
endif

override srctree := $(shell cd $(srctree) && pwd)

all:: $(forceconf)
	@/bin/true

xconfig: qconf config-script

gconfig: gconf config-script

mconfig menuconfig: mconf config-script

winconfig: winconf config-script

config: conf config-script

oldconfig: oldconf config-script

config-script: autogen config.status

reconfig::
	@touch .rackrtros_config

reconfig:: config-script

autogen:
	@current_build_dir=`pwd` ; \
	cd $(srctree) ; \
	if test ! -r configure; then \
	   echo "configure not found, running autogen.sh" ; \
	   ./scripts/autogen.sh ; \
	fi ; \
	cd $$current_build_dir

config.status: .rackrtros_config
	@test -r config.status && recf=yes || recf=no ; \
	eval `grep ^CONFIG_RACKRTROS_INSTALLDIR $<`; \
	eval `grep ^CONFIG_RACKRTROS_TARGET_TUPLE $<`; \
	eval `grep ^CONFIG_RACKRTROS_TOOLCHAIN_PARENT_DIR $<`; \
	eval `grep ^CONFIG_RACKRTROS_CROSS_COMPILE $<`; \
	if test x$$CONFIG_RACKRTROS_CROSS_COMPILE = xy ; then \
	   host_alias=$$CONFIG_RACKRTROS_TARGET_TUPLE ; \
	   CROSS_COMPILE="$$CONFIG_RACKRTROS_TOOLCHAIN_PARENT_DIR/$$host_alias/bin/$$host_alias-" ; \
	else \
	   host_alias=$(host_alias) ; \
	   CROSS_COMPILE=$(CROSS_COMPILE) ; \
	fi ; \
	$(srctree)/configure \
	CROSS_COMPILE=$$CROSS_COMPILE \
	CC=$${CROSS_COMPILE}gcc \
	CXX=$${CROSS_COMPILE}g++ \
	LD=$${CROSS_COMPILE}ld \
	AR=$${CROSS_COMPILE}ar \
	RANLIB=$${CROSS_COMPILE}ranlib \
	STRIP=$${CROSS_COMPILE}strip \
	NM=$${CROSS_COMPILE}nm \
	--build=$(build_alias) \
	--host=$$host_alias \
	--with-kconfig-file=$< \
	--prefix=$$CONFIG_RACKRTROS_INSTALLDIR \
	$$confopts ; \
	if test $$? = 0; then \
	   touch .cfok ; \
	   if test x$$recf = xyes ; then \
	      touch .cfchanged ; \
	   fi ; \
	else \
	   rm -f .cfok ; false; \
	fi

#eval `grep ^CONFIG_RACK_XENOMAIDIR $<`;
#--with-xeno-dir=$$CONFIG_RACK_XENOMAIDIR

qconf: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig xconfig \
	srctree=$(srctree) ARCH=$(ARCH)

gconf: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig gconfig \
	srctree=$(srctree) ARCH=$(ARCH)

mconf: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig menuconfig \
	srctree=$(srctree) ARCH=$(ARCH)

winconf: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig winconfig \
	srctree=$(srctree) ARCH=$(ARCH)

conf: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig config \
	srctree=$(srctree) ARCH=$(ARCH)

oldconf: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig oldconfig \
	srctree=$(srctree) ARCH=$(ARCH)

defconfig: $(KCONFIG_DIR)
	@$(MAKE) -C $(KCONFIG_DIR) \
	-f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig defconfig \
	srctree=$(srctree) ARCH=$(ARCH)
	cp .rackrtros_config defconfig

$(KCONFIG_DIR):
	@test -d $@ || mkdir -p $@

help:
	@echo ; echo "This is RACK's build bootstrapping Makefile. In order to build RACK," ; \
	echo 'you first need to configure it. Proceed as follows:' ; \
	echo ; echo '$$ cd $$your_builddir' ; echo ; \
	echo '# Configuration using a KDE-based, GTK-based or Dialog-based GUI' ; \
	echo '$$ make -f $$rack_srcdir/makefile srctree=$$rack_srcdir {xconfig,gconfig,menuconfig}' ; \
	echo '                            OR,' ; \
	echo '# Configuration using a text-based interface' ; \
	echo '$$ make -f $$rack_srcdir/makefile srctree=$$rack_srcdir {config,oldconfig}' ; \
	echo ; echo 'In case a configuration error occurs, re-run the command above to fix the' ; \
	echo 'faulty parameter. Once the configuration is successful, type:' ; echo ; \
	echo '$$ make [all]' ; \
	echo ; echo "To change the configuration from now on, simply run:"; echo ; \
	echo '$$ make {xconfig,gconfig,menuconfig,config}' ; echo

clean distclean:
	if test -r $(KCONFIG_DIR)/Makefile.kconfig ; then \
	$(MAKE) -C $(KCONFIG_DIR) -f Makefile.kconfig clean ; fi
	if test -r GNUmakefile ; then \
	$(MAKE) -f GNUmakefile $@ ; else \
	$(MAKE) -C $(KCONFIG_DIR) -f $(srctree)/$(KCONFIG_DIR)/Makefile.kconfig clean ; \
	fi
	@find . -name autom4te.cache | xargs rm -fr

all %::
	@if test -r GNUmakefile ; then \
	$(MAKE) -f GNUmakefile $@ ; else \
	echo "*** Please configure RACK first (running 'make help' in RACK's toplevel dir might help)." ; \
	exit 1; fi

.PHONY: config-script reconfig xconfig gconfig mconfig menuconfig winconfig config oldconfig qconf mconf winconf conf clean distclean help
