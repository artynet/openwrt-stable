#
# Copyright (c) 2015 Arduino Srl. All right reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ciao
PKG_REVISION:=6fb18de3758c93ab9da2959cb6161357b918b3e0
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git

PKG_SOURCE:=$(PKG_NAME)-$(PKG_REVISION).tar.gz
PKG_SOURCE_URL:=https://github.com/arduino-org/Ciao.git
PKG_SOURCE_VERSION:=$(PKG_REVISION)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_REVISION)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_REVISION)

include $(INCLUDE_DIR)/package.mk

define Package/ciao
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Ciao
  TITLE:=The new Ciao CPU-MCU Bridge
  DEPENDS:=+python +python-mini +python-paho-mqtt +python-expat +python-openssl +pyserial
endef

define Package/ciao/description
 The new Ciao Bridge lets linux and arduino communicate
endef

define Download/sleekxmpp
	URL:=http://download.linino.org/sources
	FILE:=sleekxmpp.tar.gz
	MD5SUM:=9a49f33ce532f92c58e569342b951e65
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(eval $(call Download,sleekxmpp))
	$(TAR) -xvf $(DL_DIR)/sleekxmpp.tar.gz -C $(PKG_BUILD_DIR)
endef

define Build/Compile
	# NOOP
endef

define Package/ciao/install
	$(INSTALL_DIR) $(1)/usr/lib/python2.7/ciao
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ciao/*.py $(1)/usr/lib/python2.7/ciao
	
	$(INSTALL_DIR) $(1)/usr/lib/python2.7/ciao/conf
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ciao/conf/*.conf $(1)/usr/lib/python2.7/ciao/conf

	$(INSTALL_DIR) $(1)/usr/lib/python2.7/ciao/connectors
	$(CP) -r $(PKG_BUILD_DIR)/ciao/connectors/* $(1)/usr/lib/python2.7/ciao/connectors

	# Installing run-ciao script in /usr/bin
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/run-ciao $(1)/usr/bin
	$(INSTALL_BIN) ./files/ciao-daemon $(1)/usr/bin

	# Installing "ciao" service
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ciao $(1)/etc/init.d

	# Installing "sleekxmpp" python module
	# $(INSTALL_DIR) $(1)/usr/lib/python2.7
	# $(CP) -R $(PKG_BUILD_DIR)/sleekxmpp $(1)/usr/lib/python2.7

	# Installing ciaotools.py
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ciaotools.py $(1)/usr/lib/python2.7
endef

define Package/ciao/postinst
#!/bin/sh
# check if we are on a real system
if [ -e /etc/sysctl.conf ]
then
	sed -i '/kernel.printk/d' /etc/sysctl.conf
	echo "kernel.printk = 3 4 1 3" >> /etc/sysctl.conf
	/sbin/sysctl -p 2>&1 /dev/null

	# starting service
	/etc/init.d/ciao start
fi

if [ -e /usr/lib/python2.7/asyncore.py ]
then
    sed -i -e s/512/8192/g /usr/lib/python2.7/asyncore.py
fi
endef

define Package/ciao/prerm
# check if we are on a real system
#!/bin/sh
/etc/init.d/ciao stop
/etc/init.d/ciao disable
endef

define Package/ciao/postrm
#!/bin/sh
# check if we are on a real system
if [ -e /etc/sysctl.conf ]
then
	sed -i '/kernel.printk/d' /etc/sysctl.conf
	/sbin/sysctl -p 2>&1 /dev/null
fi
endef

$(eval $(call BuildPackage,ciao))
