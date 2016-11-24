#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=findutils
PKG_VERSION:=4.4.2
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/findutils
# PKG_MD5SUM:=99180208ec2a82ce71f55b0d7389f1b3
PKG_MAINTAINER:=Roger D <rogerdammit@gmail.com>
PKG_LICENSE:=GPL-3.0

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/findutils
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=+USE_GLIBC:librt
  TITLE:=findutils
  URL:=http://www.gnu.org/software/findutils/
endef

define Package/findutils/description
	The GNU Find Utilities are the basic directory searching utilities of the GNU operating system. 
	These programs are typically used in conjunction with other programs to provide modular and powerful 
	directory search and file locating capabilities to other commands. 
endef

CONFIGURE_VARS += \
    ac_cv_func_mempcpy=n \
    gl_cv_func_wcwidth_works=yes \
    ac_cv_func_fnmatch_gnu=yes \

define Package/findutils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{find,locate,updatedb,xargs} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/{bigram,code,frcode} $(1)/usr/lib/
endef

define Package/findutils/preinst
#!/bin/sh
for x in find locate updatedb xargs; do
  [ -L "$${IPKG_INSTROOT}/usr/bin/$$x" ] && rm -f "$${IPKG_INSTROOT}/usr/bin/$$x"
done
exit 0
endef

define Package/findutils/postrm
#!/bin/sh
for x in find locate updatedb xargs; do
  /bin/busybox $$x -h 2>&1 | grep -q BusyBox && ln -sf ../../bin/busybox /usr/bin/$$x
done
exit 0
endef

$(eval $(call BuildPackage,findutils))
