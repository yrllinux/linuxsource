#!/bin/sh
#set socket recv max buff size to 2M
echo 2097152 > /proc/sys/net/core/rmem_max

#set socket send max buff size to 2M
echo 2097152 > /proc/sys/net/core/wmem_max

#set socket set ip queue size to 4K for dataswitch
echo 4096 > /proc/sys/net/ipv4/ip_queue_maxlen