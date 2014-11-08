#
# Copyright (C) 2014 Dog Hunter AG
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NPM_NAME:=ideino-linino-lib
PKG_NAME:=node-$(PKG_NPM_NAME)
PKG_VERSION:=0.1.0
PKG_RELEASE:=4

# PKG_SOURCE:=$(PKG_VERSION).tar.gz
# PKG_SOURCE_URL:=https://github.com/ideino/ideino-linino-dist/archive/
# PKG_MD5SUM:=60e0c8577596d3f925df48ed8c5e0dc4

PKG_BUILD_DEPENDS:=node
PKG_NODE_VERSION:=0.10.25

include $(INCLUDE_DIR)/package.mk

define Package/node-ideino-linino-lib
  DEPENDS:=+node +libstdcpp
  SUBMENU:=Node.js
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Linino library for Dog Hunter boards
  URL:=https://github.com/ideino/ideino-linino-dist
endef

define Package/node-ideino-linino-lib/description
	Linino library for Dog Hunter boards
endef

define Build/Prepare
	git clone --recursive https://github.com/ideino/ideino-linino-dist.git -b master ${PKG_BUILD_DIR}
	$(Build/Patch)
endef

define Build/Compile
	CC="$(TOOLCHAIN_DIR)/bin/mips-openwrt-linux-gcc" \
	CXX="$(TOOLCHAIN_DIR)/bin/mips-openwrt-linux-g++" \
	AR="$(TOOLCHAIN_DIR)/bin/mips-openwrt-linux-ar" \
	RANLIB="$(TOOLCHAIN_DIR)/bin/mips-openwrt-linux-ranlib" \
	LINK="$(TOOLCHAIN_DIR)/bin/mips-openwrt-linux-g++" \
	LD="$(TOOLCHAIN_DIR)/bin/mips-openwrt-linux-g++" \
	PATH="/usr/bin/:$(PATH)" \
	GYPFLAGS="-Dv8_use_mips_abi_hardfloat=false -Dv8_can_use_fpu_instructions=false" \
	LDFLAGS="-L$(TOOLCHAIN_DIR)/lib/ -Wl,-rpath-link $(TOOLCHAIN_DIR)/lib/" \
	npm_config_arch=mips \
	npm_config_nodedir=$(BUILD_DIR)/node-v$(PKG_NODE_VERSION)/ \
	PREFIX="$(PKG_INSTALL_DIR)/usr/" \
	npm install --target_arch=mips -g $(PKG_BUILD_DIR)/ideino-linino-lib
endef

define Package/node-ideino-linino-lib/install
	$(INSTALL_DIR) $(1)/usr/lib
	
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/node_modules $(1)/usr/lib
endef

define Package/node-ideino-linino-lib/postrm
#!/bin/sh
if [ `df | grep /opt` ]
then
	rm -rvf /opt/usr/lib/node_modules/$(PKG_NPM_NAME)
else	
	rm -rvf /usr/lib/node_modules/$(PKG_NPM_NAME)
fi
endef

$(eval $(call BuildPackage,node-ideino-linino-lib))
