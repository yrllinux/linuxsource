
WIFI_PROBE_TOPDIR:=$(shell pwd)

WIFI_PROBE_INC = $(WIFI_PROBE_TOPDIR)/include
SOURCES_PATH := $(WIFI_PROBE_TOPDIR)/common: \
				$(WIFI_PROBE_TOPDIR)/config: \
				$(WIFI_PROBE_TOPDIR)/osal: \
				$(WIFI_PROBE_TOPDIR)/parse: \
				$(WIFI_PROBE_TOPDIR)/output: \
				$(WIFI_PROBE_TOPDIR)/output/uart: \
				$(WIFI_PROBE_TOPDIR)/output/ether: \
				$(WIFI_PROBE_TOPDIR)/output/file: \
				$(WIFI_PROBE_TOPDIR)/sniffer: \
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
					

CFLAGS += -I$(WIFI_PROBE_INC) -D_GNU_SOURCE
LFLAGS =-lpthread -lpcap -lrt

TARGETS = wifi-probe

OBJS-DIR = $(WIFI_PROBE_TOPDIR)/linuxobj
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
