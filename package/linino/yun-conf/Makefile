#
# Copyright (c) 2013 Arduino LLC. All right reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=yun-conf
PKG_VERSION:=1.0.0
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/yun-conf
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Custom Yún configurations
  DEPENDS:=+avahi-daemon +base-files +triggerhappy @BROKEN
endef

define Package/yun-conf/conffiles
/etc/avahi/services/arduino.service
/etc/config/arduino
/etc/httpd.conf
endef

define Build/Compile
	# NOOP
endef

define Package/yun-conf/install
	$(INSTALL_DIR) $(1)/etc/avahi/services
	$(INSTALL_DIR) $(1)/etc/arduino
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/linino

	$(INSTALL_CONF) ./files/etc/avahi/services/arduino.service $(1)/etc/avahi/services/
	#$(INSTALL_CONF) ./files/etc/config/dhcp $(1)/etc/config/dhcp.1
endef

define Package/yun-conf/postinst
#!/bin/sh -e
if [ -n "$${IPKG_INSTROOT}" ]; then

	#configuring dhcp/dnsmasq
	#rm "$${IPKG_INSTROOT}/etc/config/dhcp"
	#mv "$${IPKG_INSTROOT}/etc/config/dhcp.1" "$${IPKG_INSTROOT}/etc/config/dhcp"

	#setting hostname, ip address, and wifi ssid
	sed -i "s/option ssid     Linino/option ssid     Arduino/" "$${IPKG_INSTROOT}/etc/config/wireless"
fi
endef

$(eval $(call BuildPackage,yun-conf))
