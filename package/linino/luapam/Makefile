#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luapam
PKG_VERSION:=3818ee6
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/devurandom/lua-pam.git
PKG_SOURCE_VERSION:=3818ee6346a976669d74a5cbc2a83ad2585c5953
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

PKG_BUILD_DEPENDS:=lua +liblua +libpam

define Package/luapam
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LuaFileSystem
  URL:=http://keplerproject.github.com/luapam/
  MAINTAINER:=W. Michael Petullo <mike@flyn.org>
  DEPENDS:=+liblua +libpam +luaterm
endef

define Package/luapam/description
	Lua module for PAM authentication 
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC) $(TARGET_CPPFLAGS) -Wall -Werror -pedantic -std=c99 -D_XOPEN_SOURCE=700

TARGET_LDFLAGS += -llua -lpam

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/luapam/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pam.so $(1)/usr/lib/lua/pam.so
endef

$(eval $(call BuildPackage,luapam))
