#
# Copyright (C) 2014 Dog Hunter AG
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ideinoIO
PKG_VERSION:=0.1.2
PKG_RELEASE:=1

GIT_SOURCE:=https://github.com/ideino/ideino-linino-dist.git

MAIN_HASH:=8b9d80d944ff99f4b6de11e7e86036567a1d3fd3
IDEINO_HASH:=a1aea30a33b8f0f42b73eb35bdfb8d3603b77e37
LIB_HASH:=302e568d832b5289f659aa2774ca04194ff88f28
EXAMPLES_HASH:=7bfa0822ac05c90f5042bf25fe4d937a0b19a13c

include $(INCLUDE_DIR)/package.mk

define Package/ideinoIO
	SECTION:=utils
	CATEGORY:=Development
	DEFAULT:=m
	TITLE:=A simple but effective IDE for Node.Js (with LininoIO bindings)
	DEPENDS:=+libpthread +libstdcpp +node
	URL:=https://github.com/ideino/ideino-dist
endef

define Package/ideinoIO/description
	Node.Js IDE with LininoIO support
endef

define Build/Compile
	# NOOP
endef

define Build/Prepare
	(git clone --recursive ${GIT_SOURCE} -b master ${PKG_BUILD_DIR}; \
	cd ${PKG_BUILD_DIR} && git checkout ${MAIN_HASH};\
	cd ${PKG_BUILD_DIR}/ideino && git checkout ${IDEINO_HASH};\
	cd ${PKG_BUILD_DIR}/ideino-linino-lib && git checkout ${LIB_HASH};\
	cd ${PKG_BUILD_DIR}/ideino-workspace && git checkout ${EXAMPLES_HASH};\
	sleep 3;\
	rm -rf .git;\
	rm .gitignore);
endef

define Package/ideinoIO/install
	$(INSTALL_DIR) $(1)
	$(INSTALL_DIR) $(1)/usr/lib/node_modules
	
	$(CP) $(PKG_BUILD_DIR)/ideino $(1)
	$(CP) $(PKG_BUILD_DIR)/_lininofiles $(1)
	$(CP) $(PKG_BUILD_DIR)/ideino-autorun $(1)
	$(CP) $(PKG_BUILD_DIR)/ideino-workspace $(1)
	$(CP) $(PKG_BUILD_DIR)/ideino-linino-lib $(1)/usr/lib/node_modules
endef

define Package/ideinoIO/postinst
#!/bin/sh
# # check if we are on real system
if [ -d /opt/ideino ] && [ -e /opt/_lininofiles/ideino ] && [ -e /opt/_lininofiles/ideinoautorun ] && [ -e /opt/_lininofiles/ideino.js ] && [ -e /opt/_lininofiles/homepage.htm ]
then
	cp /opt/_lininofiles/ideino.js /www/luci-static/resources/linino
	mv /usr/lib/lua/luci/view/linino/homepage.htm /usr/lib/lua/luci/view/linino/homepage.htm.bak
	cp /opt/_lininofiles/homepage.htm /usr/lib/lua/luci/view/linino/
	#
	cp /opt/_lininofiles/ideino /etc/init.d
	cp /opt/_lininofiles/ideinoautorun /etc/init.d
	#
	chmod +x /etc/init.d/ideino
	chmod +x /etc/init.d/ideinoautorun
	chmod +x /opt/ideino-autorun/autorun.sh
	#
	echo""
	echo "Enabling rc.d symlink for ideino"
	echo ""
	/etc/init.d/ideino enable
	/etc/init.d/ideinoautorun enable
	#
	rm -rf /opt/_lininofiles
	#
	echo "/www/luci-static/resources/linino/ideino.js" >> /etc/sysupgrade.conf
	echo "/usr/lib/lua/luci/view/linino/homepage.htm" >> /etc/sysupgrade.conf
	echo "/usr/lib/lua/luci/view/linino/homepage.htm.bak" >> /etc/sysupgrade.conf
	echo "/etc/init.d/ideino" >> /etc/sysupgrade.conf
	echo "/etc/init.d/ideinoautorun" >> /etc/sysupgrade.conf
fi
exit 0
endef

define Package/ideinoIO/prerm
#!/bin/sh
# # check if we are on real system
if [ -x /etc/init.d/ideino ] && [ -x /etc/init.d/ideinoautorun ]
then
	# Killing all the ideino istances
	ps www | grep '[i]deino.js' | awk {'print $1'} | xargs kill -9 > /dev/null 2>&1	

	# Disabling ideino services
	echo "Disabling rc.d symlink for ideino..."
	/etc/init.d/ideino disable
 	/etc/init.d/ideinoautorun disable
fi
exit 0
endef

define Package/ideinoIO/postrm
#!/bin/sh
# # check if we are on real system
if [ -d /opt/ideino ]
then
	rm -rf /opt/ideino
	rm -rf /opt/ideino-autorun
	rm -rf /opt/usr/lib/node_modules/ideino-linino-lib
	rm /etc/init.d/ideino && rm /etc/init.d/ideinoautorun
	rm /www/luci-static/resources/linino/ideino.js
	rm /usr/lib/lua/luci/view/linino/homepage.htm
	mv /usr/lib/lua/luci/view/linino/homepage.htm.bak /usr/lib/lua/luci/view/linino/homepage.htm	
fi
exit 0
endef

$(eval $(call BuildPackage,ideinoIO))
