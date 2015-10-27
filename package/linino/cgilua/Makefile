#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cgilua
PKG_VERSION:=5.2.2
PKG_RELEASE:=1
PKG_MAINTAINER:=Dirk Chang <dirk@kooiot.com>
PKG_LICENSE:=MIT

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/keplerproject/cgilua.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=6f232249afba145e2070d09f5598b99b92903d27
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/cgilua
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=CGILua
  URL:=http://keplerproject.github.io/cgilua/
  DEPENDS:=+lua
endef

define Package/cgilua/description
	CGILua is a tool for creating dynamic Web pages and manipulating input data
	from Web forms.
endef

TARGET_CFLAGS += $(FPIC)
# add make variable overrides here
MAKE_FLAGS +=

define Build/Configure
endef

define Package/cgilua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/cgilua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/cgilua/*.lua $(1)/usr/lib/lua/cgilua
endef

$(eval $(call BuildPackage,cgilua))
