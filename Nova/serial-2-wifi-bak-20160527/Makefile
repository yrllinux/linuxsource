#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=serial-2-wifi
PKG_VERSION:=1.0
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=Nova moudle
  CATEGORY:=Nova
  TITLE:=serial change to wifi.
  DEPENDS:= +libpthread +librt
endef

define Build/Configure
	#echo "const char serial-2-wifi-version[] = \"$(PKG_VERSION)\";" > $(PKG_BUILD_DIR)/version.c
	#rm -f $(PKG_BUILD_DIR)/version.sh
	#touch $(PKG_BUILD_DIR)/version.sh
	#chmod +x $(PKG_BUILD_DIR)/version.sh
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/description
	serial-2-wifi general program
endef

define Package/serial-2-wifi/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) ./files/serial2wifi.init $(1)/etc/init.d/serial2wifi
	$(INSTALL_CONF) ./files/serial2wifi.config $(1)/etc/config/serial2wificfg
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/serial-wifi $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,serial-2-wifi))
