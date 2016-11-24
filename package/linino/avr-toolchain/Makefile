#
# Copyright (C) 2014 Dog Hunter AG
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=avr-toolchain
PKG_VERSION:=3.2.4
PKG_RELEASE:=6

PKG_SOURCE_URL:=http://download.linino.org/pkg-src/
PKG_SOURCE:=$(PKG_NAME)-mips32-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=4dc1abbd6c233de391ee9820a19003ac

include $(INCLUDE_DIR)/package.mk

define Package/avr-toolchain
	SECTION:=devel
	CATEGORY:=Development
	TITLE:=Tools for AVR
	DEPENDS:=+zlib
endef

define Package/avr-toolchain/description
	The AVR toolchain package contains a linker, an assembler, and other tools for handling object files
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/temp
	/bin/tar xvf $(DL_DIR)/$(PKG_SOURCE) -C $(PKG_BUILD_DIR)/temp
endef

define Build/Configure
	# NOOP
endef

define Build/Compile
	# NOOP
endef

define Package/avr-toolchain/install
	$(INSTALL_DIR) $(1)/usr/avr
	
	$(CP) -av $(PKG_BUILD_DIR)/temp/* $(1)/usr/avr
endef

define Package/avr-toolchain/preinst
#!/bin/sh
# check if we are on a real system
if `grep -qs '/opt' /proc/mounts`
then	
	sed -i '/export PATH/ s|$$|:\/opt\/usr\/avr\/bin|' /etc/profile
else
	sed -i '/export PATH/ s|$$|:\/usr\/avr\/bin|' /etc/profile
fi
endef

define Package/avr-toolchain/postrm
#!/bin/sh
# check if we are on a real system
if `grep -qs '/opt' /proc/mounts`
then
	rm -rf /opt/usr/avr
	sed -i 's/:\/opt\/usr\/avr\/bin//g' /etc/profile
else
	rm -rf /usr/avr
	sed -i 's/:\/usr\/avr\/bin//g' /etc/profile
fi
endef

$(eval $(call BuildPackage,avr-toolchain))
