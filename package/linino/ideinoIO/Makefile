#
# Copyright (C) 2013 Arduino LLC. All right reserved
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ideinoIO
PKG_VERSION:=0.1.0
PKG_RELEASE:=5

GIT_SOURCE:=https://github.com/ideino/ideino-linino-dist.git

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
	Node.Js IDE
endef

define Build/Compile
	# NOOP
endef

define Build/Prepare
	(git clone --recursive ${GIT_SOURCE} -b master ${PKG_BUILD_DIR}; \
	cd ${PKG_BUILD_DIR};\
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

define Package/ideinoIO/preinst
#!/bin/sh
# # check if we are on real system
if [ ! -d /opt ]
then
	mkdir -p /opt  
fi
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
	ps www | grep -i [n]ode | awk {'print $1'} | xargs kill -9 > /dev/null 2>&1
	
	echo "Stopping ideino services...."
	/etc/init.d/ideino stop
 	/etc/init.d/ideinoautorun stop
	
	echo "Disabling rc.d symlink for ideino"
	/etc/init.d/ideino disable
 	/etc/init.d/ideinoautorun disable
fi
exit 0
endef

define Package/ideinoIO/postrm
#!/bin/sh
# # check if we are on real system
if [ -d /opt/ideino ] && [ -e /etc/init.d/ideino ] && [ -e /etc/init.d/ideinoautorun ] && [ -e /www/luci-static/resources/linino/ideino.js ] && [ -e /usr/lib/lua/luci/view/linino/homepage.htm ] && [ -e /usr/lib/lua/luci/view/linino/homepage.htm.bak ]
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
