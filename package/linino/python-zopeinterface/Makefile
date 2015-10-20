#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zopeinterface
PKG_VERSION:=4.1.2
PKG_RELEASE:=1

PKG_SOURCE:=zope.interface-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://pypi.python.org/packages/source/z/zope.interface/
PKG_MD5SUM:=

PKG_BUILD_DIR:=$(BUILD_DIR)/zope.interface-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-zopeinterface
  SECTION:=language-python
  CATEGORY:=Languages
  SUBMENU:=Python
  TITLE:=zope-interface
  URL:=http://sourceforge.net/projects/zopeinterface
  DEPENDS:=+python
endef

define Package/python-zopeinterface/description
Python module interface to the cURL library.
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix=/usr --root=$(PKG_INSTALL_DIR))
endef

define Package/python-zopeinterface/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
		$(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-zopeinterface))
