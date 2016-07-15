#!/bin/bash
if [[ $1 == "--help" || $1 == "help" ]]; then
	echo "Usage: connect.sh interface SSID PASSWORD"
	exit;
fi

if [ -z "$1" ]; then
echo "Please specify the network interface"
exit
fi

ip link set $1 down
ip link set $1 up

if [ -z "$2" ]; then
echo "SSID is empty"
exit
fi

if [ -z "$3" ]; then
echo "PASSWORD is empty"
exit
fi

echo "INTERFACE is $1"
echo "SSID is $2"
echo "PASSWORD is $3"

config_file=tmp;
echo "tempory configuration file is $config_file"

wpa_passphrase $2 $3 > $config_file
wpa_supplicant -B -i $1 -c $config_file -dd
echo "connection done"
echo "configuring IP"
dhclient -v $1
