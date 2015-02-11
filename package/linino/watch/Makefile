# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=watch
PKG_VERSION:=3.2.8
PKG_RELEASE:=1

PKG_SOURCE:=procps-3.2.8.tar.gz
PKG_SOURCE_URL:=http://procps.sourceforge.net
PKG_MD5SUM:=9532714b6846013ca9898984ba4cd7e0

PKG_BUILD_DIR:=$(BUILD_DIR)/watch-3.2.8

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/watch
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses
  TITLE:=proc utilities
  URL:=http://watch.sourceforge.net/
endef

define Package/watch/description
 watch is the package that has a bunch of small useful utilities that give 
 information about processes using the /proc filesystem. The package 
 includes the programs ps, top, vmstat, w, kill, free, slabtop, and skill.
endef

define Build/Prepare
	/bin/tar xvf $(DL_DIR)/$(PKG_SOURCE) -C $(PKG_BUILD_DIR) --strip-components=1
	$(Build/Patch)
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="$(TARGET_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \

define Package/watch/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/watch $(1)/usr/bin
	
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/proc/libproc-3.2.8.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,watch))
