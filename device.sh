#!/bin/sh

for device in /sys/class/input/event*
do
	if fgrep -q keyboard "${device}/device/name"
	then
		echo "/dev/input/$(basename ${device})"
	fi
done
