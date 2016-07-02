SERIAL_WIFI_TOPDIR:=$(shell pwd)

SERIAL_WIFI_INC = $(SERIAL_WIFI_TOPDIR)/include
OSAL_INC = $(SERIAL_WIFI_INC)/osal
SOURCES_PATH := $(SERIAL_WIFI_TOPDIR)/ops/common: \
				$(SERIAL_WIFI_TOPDIR)/ops/config: \
				$(SERIAL_WIFI_TOPDIR)/core: \
				$(SERIAL_WIFI_TOPDIR)/osal: \
				$(SERIAL_WIFI_TOPDIR)/ops: \
				$(SERIAL_WIFI_TOPDIR)/serial: \
				$(SERIAL_WIFI_TOPDIR)/network: \
				./: 

REMOVE_PATTERN= \( -name '*.[oas]' \
                        -o -name '*.ko'  \
                        -o -name 'modules.order' \
                        -o -name 'Module.symvers' \
                        -o -name '*.cmd' \
                        -o -name '*.d'   \
                        -o -name '*.tmp' \
                        -o -name '*.scc' \
                        -o -name '*.tmp_versions' \
                        -o -name '*.depend' \
                        -o -name '*.mod.c' \)
					

CFLAGS += -I$(SERIAL_WIFI_INC) -I$(OSAL_INC) -D_GNU_SOURCE
LFLAGS =-lpthread -lrt

TARGETS = serial-wifi

OBJS-DIR = $(SERIAL_WIFI_TOPDIR)/linuxobj
vpath %.c $(SOURCES_PATH)
SOURCES = $(wildcard $(subst :,/*.c,$(SOURCES_PATH)))
SOURCES_NODIR = $(notdir $(SOURCES))
OBJS = $(patsubst %.c,$(OBJS-DIR)/%.o,$(SOURCES_NODIR))

$(TARGETS) : $(OBJS-DIR) $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS) 

$(OBJS-DIR) :
	mkdir -p $@
	
$(OBJS-DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY : clean

clean:
	@echo "CLEAN         $(TARGETS)"
	@find . -follow $(REMOVE_PATTERN) | xargs rm -fr
	@rm -f $(TARGETS)
