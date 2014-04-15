#
# Copyright (C) 2013 Arduino LLC. All right reserved
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=alljoyn-linino
PKG_VERSION:=1.0
PKG_RELEASE:=4

include $(INCLUDE_DIR)/package.mk

define Package/alljoyn-linino
	DEPENDS:=+alljoyn
	SECTION:=net
	CATEGORY:=Network
	DEFAULT:=y
	TITLE:=Linino configuration for alljoyn platform
endef

define Package/alljoyn-linino/description
	Linino fix for the alljoyn platform
endef

define Build/Compile
  	#NOOP
endef

define Package/alljoyn-linino/install
	#NOOP
endef

define Package/alljoyn-linino/postinst
#!/bin/sh
if [ -e /etc/config/alljoyn ]
then
	sed -i "s/option all_networks '0'/option all_networks '1'/g" /etc/config/alljoyn
fi
endef

define Package/alljoyn-linino/postrm
#!/bin/sh
if [ -e /etc/config/alljoyn ]
then
	sed -i "s/option all_networks '1'/option all_networks '0'/g" /etc/config/alljoyn
fi
endef

$(eval $(call BuildPackage,alljoyn-linino))
