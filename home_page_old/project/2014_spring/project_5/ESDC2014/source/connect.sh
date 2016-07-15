#!/bin/bash
if [[ $1 == "--help" || $1 == "help" ]]; then
	echo "Usage: connect.sh interface SSID PASSWORD"
	exit;
fi

if [ -z "$1" ]; then
echo "Please specify the network interface"
exit
fi

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

config_file=/home/root/tmp

echo "Generating configuration file..."
wpa_passphrase $2 $3 > $config_file
echo "Done"

echo "Setting up wpa_supplicant daemon..."
wpa_supplicant -B -i $1 -c $config_file
echo "Done"


#rm $config_file
echo "Runing dhclient for DHCP"
dhclient -v $1
echo "Done"
echo "You have connected to $2, though $1"
