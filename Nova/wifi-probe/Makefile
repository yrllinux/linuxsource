#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wifi-probe
PKG_VERSION:=1.0
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=Nova moudle
  CATEGORY:=Nova
  TITLE:=wifi probe --Capture and Parse wifi packet.
  DEPENDS:= +libpthread +libpcap +librt
endef

define Build/Configure
	#echo "const char wifi-probe-version[] = \"$(PKG_VERSION)\";" > $(PKG_BUILD_DIR)/version.c
	#rm -f $(PKG_BUILD_DIR)/version.sh
	#touch $(PKG_BUILD_DIR)/version.sh
	#chmod +x $(PKG_BUILD_DIR)/version.sh
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/description
	wifi-probe general program
endef

define Package/wifi-probe/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) ./files/wifiprobe.init $(1)/etc/init.d/wifiprobe
	$(INSTALL_CONF) ./files/probe.config $(1)/etc/config/probe
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifi-probe $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wifi-probe))
