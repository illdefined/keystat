#!/bin/sh

for device in /sys/class/input/event*
do
	if fgrep keyboard "${device}/device/name" >/dev/null
	then
		echo "/dev/input/$(basename ${device})"
	fi
done
