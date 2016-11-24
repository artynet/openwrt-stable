#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luaterm
PKG_VERSION:=8f2b46f
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/devurandom/lua-term.git
PKG_SOURCE_VERSION:=8f2b46ff3439ba908d281e890d20a54d63559af1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

PKG_BUILD_DEPENDS:=lua +liblua

define Package/luaterm
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LuaTerm
  URL:=http://keplerproject.github.com/luaterm/
  MAINTAINER:=W. Michael Petullo <mike@flyn.org>
  DEPENDS:=+liblua
endef

define Package/luaterm/description
	Lua utility module for terminals
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC) $(TARGET_CPPFLAGS) -Wall -Werror -pedantic -std=c99 -D_XOPEN_SOURCE=700

TARGET_LDFLAGS += -llua

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/luaterm/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/term.so $(1)/usr/lib/lua/term.so
endef

$(eval $(call BuildPackage,luaterm))
