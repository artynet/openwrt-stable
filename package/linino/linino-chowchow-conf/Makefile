#
# Copyright (c) dog hunter AG - Zug - CH
# General Public License version 2 (GPLv2)
# Author: Mimmo <mimmo@linino.org>
#


include $(TOPDIR)/rules.mk

PKG_NAME:=linino-chowchow-conf
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/linino-chowchow-conf
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Custom Linino ChowChow configurations
  DEPENDS:=+avahi-daemon +base-files +triggerhappy
endef

define Package/linino-chowchow-conf/conffiles
/etc/avahi/services/linino.service
/etc/config/linino
/etc/config/wireless
/etc/httpd.conf
endef

define Build/Compile
	# NOOP
endef

define Package/linino-chowchow-conf/install
	$(INSTALL_DIR) $(1)/etc/avahi/services
	$(INSTALL_DIR) $(1)/etc/arduino
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/linino

	$(INSTALL_CONF) ./files/etc/config/wireless $(1)/etc/config/wireless
endef

define Package/linino-chowchow-conf/postinst
#!/bin/sh -e
if [ -n "$${IPKG_INSTROOT}" ]; then

	#configuring dhcp/dnsmasq
	#rm "$${IPKG_INSTROOT}/etc/config/dhcp"
	#mv "$${IPKG_INSTROOT}/etc/config/dhcp.1" "$${IPKG_INSTROOT}/etc/config/dhcp"

	#setting hostname, ip address, and wifi ssid
	sed -i "s/option ssid     Linino/option ssid     Arduino/" "$${IPKG_INSTROOT}/etc/config/wireless"
fi
endef

$(eval $(call BuildPackage,linino-chowchow-conf))
