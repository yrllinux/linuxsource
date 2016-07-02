###
### Copyright (c) 2004,2006 KEDACOM, Inc.
###
### ReVision History:
###
### 2007/08/23 GuZhenhua£º add macros for different hardware archit

#########################################################################
###
###  DESCRIPTION:
###    Common definitions for all Makefiles.
###
###  MAKEFILE OPTIONS:
###    These are options that should be specified in the Makefile.
###    At a minimum, one or more of ARC_TARGET, SO_TARGET,
###    APP_TARGET, and/or DIRS must be defined.
###
###    ARC_TARGET - target name for archive file, excluding file
###      extension.
###    SO_TARGET - target name for shared object file, excluding file
###      extension.
###    APP_TARGET - target name of application.
###    DIRS - list of subdirectories to execute "make" in.
###    OBJS - list of object files (without .o extension) that compose
###      the target.
###    LIBS - optional list of libraries that the target uses--
###      don't include the suffix (.a). Libraries that other libraries
###      depend on (e.g. romcon) should be put later in the list.
###    INSTALL_INC - list of header files to install. (Libs will always
###      be installed.)
###    INSTALL_INC_LOC - Use "os" to specify that headers should be
###      installed into the os-specific directory. Otherwise the
###      headers will be installed to "common" (default=common).
###    INSTALL_LIB_LOC - Use "os" to specify that libraries should be
###      installed into the os-specific directory. Otherwise the
###      libraries will be installed to "common" (default=common).
###
###  TOOLKIT ENVIRONMENT VARIABLES:
###    These environment variables should be set before compiling.
###
###    ETI_TOOLKIT - must point to the base directory of the
###      Equator toolkit.
###    ETI_TOOLKIT_INSTALL - must point to a location to put
###      the built driver binaries.
###    ETI_TOOLKIT_LOCAL - optional variable that can point to a
###      location to pick up headers and libraries before the
###      Equator toolkit. It should point to the same location
###      as ETI_TOOLKIT_INSTALL.
###
###  OPTIONAL ENVIRONMENT VARIABLES:
###    These are options that are normally set in environment varibles.
###    They can also be set in the Makefile or on the make command line
###    (e.g. "make DEBUG=1"). Settings in the Makefile takes precendence
###    over the command line, which takes precendence over environment
###    variables. Unless otherwise stated, options should be set to 1
###    or 0 or left unset to use the default. Settings on the make
###    command line will propagate down to subdirectories when building
###    a tree.
###
###    DEBUG - include symbols and define "DEBUG" symbol during
###      compile (default=0).
###    INC_PATH - Additional directories to be searched for headers,
###      separated by spaces. Default is to use the compiler's default
###      path, adding RTOS_INCLUDES for VxWorks builds.
###    LIB_PATH - Additional directories to be searched for libraries,
###      separated by spaces. Default is to use the linker's default
###      path.
###    CFLAGS - Additional compile options.
###    LDFLAGS - Additional link options.
###
#########################################################################

## Add appropriate suffixes and extensions

ifneq ($(ARC_TARGET),)
  ARC_TARGET := lib$(ARC_TARGET)$(LIB_SUFFIX).a
endif

## Special variables to help with clean targets

DIRSC := $(foreach dir,$(DIRS),$(dir)(clean))

ASMS := $(foreach obj,$(OBJS),$(basename $(obj)).s)

# add for TI DM81xx app
ifneq ($(APP_TARGET),)
	ifeq ($(LINUX_COMPILER),_ARM_DM81_)
		COMMOM_DIR = $(shell pwd)/../../../10-common/include
		KERNEL_DIR = $(PSP_INSTALL_DIR)/linux-2.6.37-psp04.00.00.09
		SYSLINK_ROOT = $(SYSLINK_INSTALL_DIR)
		DEVKITLIB_INC = $(LINUX_DEVKIT_DIR)/arm-none-linux-gnueabi/usr/include
		
		#release path define					 
		ifeq ($(DEBUG),0) 		
			LIB_PATH += $(DEVKITLIB_INC) $(WORKSPACE)/../../../10-common/lib/release/linux_arm_dm81xx $(WORKSPACE)/../../../10-common/lib/release/linux_arm_dm81xx/nipdebug 
			include $(shell pwd)/../../../10-common/lib/release/devm_ti816x/make/Rules.make
			DEVM_DIR = $(shell pwd)/../../../10-common/lib/release/devm_ti816x
			SERVER_DIR = $(shell pwd)/../../../10-common/lib/release/devm_ti816x/server_package
		else #debug path define
			LIB_PATH += $(DEVKITLIB_INC) $(WORKSPACE)/../../../10-common/lib/debug/linux_arm_dm81xx $(WORKSPACE)/../../../10-common/lib/debug/linux_arm_dm81xx/nipdebug 
			include $(shell pwd)/../../../10-common/lib/debug/devm_ti816x/make/Rules.make
			DEVM_DIR = $(shell pwd)/../../../10-common/lib/debug/devm_ti816x
			SERVER_DIR = $(shell pwd)/../../../10-common/lib/debug/devm_ti816x/server_package
		endif
	
	## [CE] Name the xdc target and platform
	XDCTARGET = gnu.targets.arm.GCArmv7A
	XDCPLATFORM = ti.platforms.evmTI816X
	VERBOSE = @
	  XDCPATH :=$(XDCPATH);$(DEVM_DIR);$(SERVER_DIR)
	  CROSS_COMPILE := $(CSTOOL_PREFIX)
	  export CROSS_COMPILE
		export CODEGEN_INSTALL_DIR
		export XDCPATH
		export PLATFORM_XDC
		## [CE] configuro-generated files
		XDC_CFG = $(APP_TARGET)_config
		XDC_CFGFILE = $(COMMMK_DIR)/81xxApp.cfg
		COMPILER_FILE = $(XDC_CFG)/compiler.opt
		LINKER_FILE = $(XDC_CFG)/linker.cmd
		CONFIG_BLD = $(COMMMK_DIR)/config.bld
		## [CE] The path to the configuro utility
		CONFIGURO = $(XDC_INSTALL_DIR)/xs xdc.tools.configuro
		
		INC_PATH += $(SYSLINK_ROOT) $(SYSLINK_ROOT)/ti/syslink/inc $(SYSLINK_ROOT)/ti/syslink/inc/usr $(SYSLINK_ROOT)/ti/syslink/inc/usr/Linux \
							 $(WORKSPACE)/../../../codeccomponent/include  $(COMMOM_DIR)/platform  $(COMMOM_DIR)/platform/netra/ipc $(COMMOM_DIR)/platform/netra/vpss \
							 $(COMMOM_DIR)/platform/netra/audio $(COMMOM_DIR)/system1 $(COMMOM_DIR)/protocol $(WORKSPACE)/../../../../common_hd3 $(WORKSPACE)/../../../common \
							 $(WORKSPACE)/../../../../common_hd3/mediacommon/include $(WORKSPACE)/../../../codeclib/include $(WORKSPACE)/../../../codecdriver/include \
							 $(KERNEL_DIR)/include $(KERNEL_DIR)/arch/arm/plat-omap/include $(XDC_INSTALL_DIR)/packages $(XDAIS_INSTALL_DIR)/packages $(COMMOM_DIR)/video1 \
							 $(COMMOM_DIR)/audio $(COMMOM_DIR)/mcu $(COMMOM_DIR)/system1 $(COMMOM_DIR)/algorithm  $(COMMOM_DIR)/pwlib/ptlib \
							 $(COMMOM_DIR)/protocol $(WORKSPACE)/../../common/include 
	
		CFLAGS += -D_REENTRANT -D_LINUX_ -rdynamic 
		ifeq ($(PLATFORM),TI816X)
		    CFLAGS	+= -DPLATFORM=8168 -DDEVICE=DEVICE_FLOAT -D_TMS320C6400
		endif
		
		ifeq ($(PLATFORM),TI814X)
		    CFLAGS	+= -DPLATFORM=8148 -DDEVICE=DEVICE_FLOAT -D_TMS320C6400
		endif
		
	endif
endif


## Put the extension on all objs

OBJS := $(foreach obj,$(OBJS),$(obj).o)

## OS Definition
CFLAGS += -D_LINUX_

## Hardware arch definition
ifeq ($(LINUX_COMPILER),_PPC_8377_)
   CFLAGS += -D_PPC_
else
ifeq ($(LINUX_COMPILER),_HHPPC_)
   CFLAGS += -D_PPC_
else
ifeq ($(LINUX_COMPILER),_ARM_DM6446_)
   CFLAGS += -D_ARM_
else   
ifeq ($(LINUX_COMPILER),_EQUATOR_)
   CFLAGS += -D_EQUATOR_
else
   CFLAGS += -D_X86_
endif
endif
endif
endif

# add for TI DM81xx
ifeq ($(LINUX_COMPILER),_ARM_DM81_)
   
   CFLAGS += -D_ARM_ -D_ARM_DM81_
endif

## Turn on Exception call stack
CFLAGS += -fno-omit-frame-pointer

ifeq ($(PWLIB_SUPPORT),1)
   CFLAGS += -DPWLIB_SUPPORT -DPTRACING=1 -D_REENTRANT -DPHAS_TEMPLATES -DPMEMORY_CHECK=1 -DPASN_LEANANDMEAN -pipe -fPIC
endif

## Turn on debug flag and define DEBUG symbol for debug builds
ifeq ($(DEBUG),0)
	ifeq ($(LINUX_COMPILER),_ARM_DM81_)
		ifeq ($(ARM_ISA),armv7-a)
		    CFLAGS	+=  -O2 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp
		endif
		ifeq ($(ARM_ISA),armv5t)
		    CFLAGS	+=  -mlittle-endian -march=armv5t -mtune=arm9tdmi -mabi=aapcs-linux -O
		endif
	else
  CFLAGS += -O2
  CFLAGS += -DNDEBUG
  endif
endif

ifeq ($(DEBUG),1)
  CFLAGS +=  -O0
  CFLAGS += -DDEBUG=$(DEBUG)
endif

ifeq ($(DEBUG),2)
  CFLAGS +=  -g
  CFLAGS += -DDEBUG=1
endif

## Add include path and constant definitions to
## compile options

CFLAGS += $(foreach dir,$(INC_PATH),-I$(dir))

## Add library path and libraries to link options
LDFLAGS += $(foreach lib,$(LIB_PATH),-L$(lib))

ifeq ($(LINUX_COMPILER),_HHPPC_)
   LDFLAGS += --static
endif
ifeq ($(LINUX_COMPILER),_PPC_8377_)
   LDFLAGS += --static
endif
ifeq ($(LINUX_COMPILER),_ARM_DM6446_)
   LDFLAGS += --static
endif
ifeq ($(LINUX_COMPILER),_ARM_DM81_)
	LDFLAGS += -lm -lpthread -ldl -lrt -pthread -Wl,-T $(LINKER_FILE)
endif


## When using a shared object library and not building
## the shared object library itself, don't link with the
## libraries. Don't know how to do "or" in make, so use
## an intermediate variable.
LDFLAGS += $(foreach lib,$(LIBS),-l$(lib)$(LIB_SUFFIX))

## Set up library install location
ifndef INSTALL_LIB_PATH
  ifeq ($(INSTALL_LIB_LOC),os)
    INSTALL_LIB_PATH = $(ETI_TOOLKIT_INSTALL)/$(RTOS_DIR)/$(MAP_ARCH)_lib
  else
    INSTALL_LIB_PATH = $(ETI_TOOLKIT_INSTALL)/common/$(MAP_ARCH)_lib
  endif
endif

## Set up application install location
ifndef INSTALL_APP_PATH
  APP_DIR ?= unknown
  INSTALL_APP_PATH = $(ETI_TOOLKIT_INSTALL)/app/$(APP_DIR)
endif


## Set up the tools to use
ifeq ($(LINUX_COMPILER),_HHPPC_)
    CROSS = ppc_82xx-
    ifeq ($(PPC_LINUX_VERSION),_85xx_)
        CROSS = ppc_85xx-
    endif
endif

## Set up the tools to use
ifeq ($(LINUX_COMPILER),_PPC_8377_)
    CROSS = /opt/freescale/usr/local/gcc-4.2.171-eglibc-2.5.171/powerpc-linux-gnu/bin/powerpc-linux-gnu-
endif

## Set up the tools to use
ifeq ($(LINUX_COMPILER),_ARM_DM81_) 
CROSS = /opt/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-
endif


ifeq ($(LINUX_COMPILER),_ARM_DM6446_)
   CROSS = /opt/montavista/pro/devkit/arm/v5t_le/bin/arm_v5t_le-
endif

CC      = $(CROSS)g++
CPP     = $(CROSS)g++
LD      = $(CROSS)g++
AR      = $(CROSS)ar
INSTALL = install -D -m 644
OBJDUMP = objdump
RM      = rm -f


ifeq ($(LINUX_COMPILER),_EQUATOR_)
CC      = /usr/local/Equator/v6.0/tools/i686_Linux/bin/ecc
STRIP   = /usr/local/Equator/v6.0/tools/i686_Linux/bin/estrip
CPP     = /usr/local/Equator/v6.0/tools/i686_Linux/bin/ecc
LD      = /usr/local/Equator/v6.0/tools/i686_Linux/bin/ecc
endif

##------------------------------------------------------------------------
## Rules

## Suffix rules

$(SRC_DIR)/%.o: $(SRC_DIR)/%.s
	$(CC) -c -o $@ $(CFLAGS) $<
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $(CFLAGS) $<


## Rules for making archives

ifneq ($(strip $(ARC_TARGET)),)
  
  ifneq ($(LINUX_COMPILER),_HHPPC_)
      CFLAGS += -DFD_SETSIZE=512
  endif
  ifneq ($(LINUX_COMPILER),_ARM_DM81_)
      CFLAGS += -DFD_SETSIZE=512
  endif
  
  all: install cleanobjs_arc
  
  install: install_inc install_arc
  
  install_arc: $(ARC_TARGET)
	$(INSTALL) $(ARC_TARGET) $(INSTALL_LIB_PATH)/$(ARC_TARGET)
  
  $(ARC_TARGET) : $(OBJS)
	$(AR) crus $(ARC_TARGET) $(OBJS)

  uninstall: uninstallarc
  
  uninstallarc:
	$(foreach file, $(INSTALL_INC), $(RM) $(INSTALL_INC_PATH)/$(file) ;)
	$(RM) $(INSTALL_LIB_PATH)/$(ARC_TARGET)

  clean: cleanarc
  
  cleanarc:
	$(RM) $(ARC_TARGET)
	
  cleanobjs_arc:
	$(RM) $(OBJS) *.pdb *.map

endif


## Rules for making shared object

ifneq ($(strip $(SO_TARGET)),)
  
  all: install
  
  install: install_inc install_so
  
  install_so: $(SO_TARGET)
	$(INSTALL) $(SO_TARGET) $(INSTALL_LIB_PATH)/$(SO_TARGET)
  
  $(SO_TARGET) : $(OBJS)
	$(LD) $(OBJS) -o $(SO_TARGET) $(LDFLAGS)
	$(OBJDUMP) --syms $(SO_TARGET) | sort | grep " g" > $(SO_TARGET).map

  uninstall: uninstallso
  
  uninstallso:
	$(foreach file, $(INSTALL_INC), $(RM) $(INSTALL_INC_PATH)/$(file) ;)
	$(RM) $(INSTALL_LIB_PATH)/$(ARC_TARGET)

  clean: cleanso
  
  cleanso:
	$(RM) $(SO_TARGET)

endif


## Rules for making applications

ifneq ($(strip $(APP_TARGET)),)

  all:$(COMPILER_FILE) $(APP_TARGET) cleanobjs_app 
  
  install: install_inc install_app
  
  install_app: $(APP_TARGET)
	$(INSTALL) $(APP_TARGET) $(INSTALL_APP_PATH)/$(APP_TARGET)
  
  $(APP_TARGET): $(OBJS)
	$(LD) $(OBJS) -o $(APP_TARGET) $(LDFLAGS)

ifneq ($(strip $(STRIP)),)
		$(STRIP) $(APP_TARGET)
endif	

#	$(OBJDUMP) --syms $(APP_TARGET) | sort | grep " g" > $(APP_TARGET).map

  clean: cleanapp
  
  cleanapp:
	$(RM) $(APP_TARGET)
	
  cleanobjs_app:
	$(RM) $(OBJS_CLIENT) *.pdb *.map

endif

ifeq ($(LINUX_COMPILER),_ARM_DM81_)
$(LINKER_FILE) $(COMPILER_FILE):	$(XDC_CFGFILE)
	@mkdir -p release
	@echo
	@echo ======== Building $(APP_TARGET) ========
	@echo Configuring application using $<
	@echo
	$(VERBOSE) $(CONFIGURO) -o $(XDC_CFG) -t $(XDCTARGET) -p $(XDCPLATFORM) -b $(CONFIG_BLD) $(XDC_CFGFILE)
	@echo ======================================
endif


## Rules for making subdirectories

ifneq ($(strip $(DIRS)),)

  all: $(DIRS)
  $(DIRS): FORCE
	$(MAKE) -C $@
  $(DIRSC): FORCE
	$(MAKE) -C $@ clean
  FORCE:
  clean: $(DIRSC)

endif


## Shared rules

install: install_inc

install_inc:
	$(foreach file, $(INSTALL_INC), $(INSTALL) $(file) $(INSTALL_INC_PATH)/$(notdir $(file)) ;)

clean: cleanobjs

cleanobjs:
	$(RM) $(ASMS)  $(OBJS)  *.pdb *.map


## Rule to pre-install all headers

setup:
	(cd $(TOP);    \
	make install_inc;   \
	echo )
