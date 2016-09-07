ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#define PINFO
#PINFO DESCRIPTION=ethernet driver
#endef

INSTALLDIR = sbin
include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk
#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../install
   USE_INSTALL_ROOT=1
##############################################################

USEFILE=$(PROJECT_ROOT)/$(SECTION)/$(NAME).use
EXTRA_SRCVPATH = $(EXTRA_SRCVPATH_$(SECTION))
EXTRA_INCVPATH+=$(INSTALL_ROOT_nto)/usr/include/xilinx
EXTRA_LIBVPATH+=$(INSTALL_ROOT_nto)/usr/lib/xilinx
-include $(PROJECT_ROOT)/$(SECTION)/extra.mk

LIBS += drvrS

PRE_SRCVPATH = $(foreach var,$(filter a, $(VARIANTS)),$(CPU_ROOT)/$(subst $(space),.,$(patsubst a,dll,$(filter-out g, $(VARIANTS)))))



include $(MKFILES_ROOT)/qtargets.mk
