#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/LININO_ALL_PROFILE
	NAME:=Linino All Profiles
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/LININO_ALL_PROFILE/Description
	Select this in order to build an image for every Linino's board.
endef

$(eval $(call Profile,LININO_ALL_PROFILE))

define Profile/LININO_YUNONECHOW_PROFILE
	NAME:=Linino YunOneChow
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/LININO_YUNONECHOW_PROFILE/Description
	Select this in order to build an image for Yun, One and ChowChow
endef

$(eval $(call Profile,LININO_YUNONECHOW_PROFILE))

define Profile/LININO_YUNONE_PROFILE
	NAME:=Linino YunOne
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/LININO_YUNONE_PROFILE/Description
	Select this in order to build an image for Yun and One 
endef

$(eval $(call Profile,LININO_YUNONE_PROFILE))

define Profile/LININO_YUN
	NAME:=Linino Arduino Yun
	PACKAGES:=kmod-usb-core kmod-usb2 luci-app-arduino-webpanel
endef

define Profile/LININO_YUN/Description
	Package set optimized for the Arduino Yun based on
	Atheros AR9331.
endef

$(eval $(call Profile,LININO_YUN))

define Profile/LININO_ONE
	NAME:=Linino One
	PACKAGES:=kmod-usb-core kmod-usb2 luci-webpanel-linino
endef

define Profile/LININO_ONE/Description
	Package set optimized for the Linino One based on
	Atheros AR9331.
endef

$(eval $(call Profile,LININO_ONE))

define Profile/LININO_FREEDOG
	NAME:=Linino Freedog
	PACKAGES:=kmod-usb-core kmod-usb2 luci-webpanel-linino
endef

define Profile/LININO_FREEDOG/Description
	Package set optimized for the Linino Freedog based on
	Atheros AR9331.
endef

$(eval $(call Profile,LININO_FREEDOG))

define Profile/LININO_CHOWCHOW
	NAME:=Linino Chowchow
	PACKAGES:=kmod-usb-core kmod-usb2 luci-webpanel-linino
endef

define Profile/LININO_CHOWCHOW/Description
	Package set optimized for the Linino Chowchow based on
	Atheros AR9331.
endef

$(eval $(call Profile,LININO_CHOWCHOW))

define Profile/LININO_CHIWAWA
        NAME:=Linino Chiwawa
        PACKAGES:=kmod-usb-core kmod-usb2 luci-webpanel-linino
endef

define Profile/LININO_CHIWAWA/Description
        Package set optimized for the Linino Chiwawa based on
        Atheros AR9331.
endef

$(eval $(call Profile,LININO_CHIWAWA))


