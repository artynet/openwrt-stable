#!/bin/sh /etc/rc.common
# Copyright (C) 2008 OpenWrt.org

START=17

start () {

	boardname=$(awk '/machine/ {print tolower($4)}' /proc/cpuinfo)

	if [ $boardname="chowchow" ]
	then 

		sed -i s/eth1/eth0/g /etc/config/network
		# sed -i s#\'0\'#\'1\'#g /etc/config/wireless
		uci "set" "wireless.@wifi-device[0].disabled=1";
		uci "set" "wireless.@wifi-device[1].disabled=0";

		/sbin/uci commit wireless;
		/sbin/wifi down;
		/sbin/wifi up;

	fi

}