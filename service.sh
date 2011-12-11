#!/bin/sh

set -e

for device in /sys/class/input/event*
do
	if grep -F -q keyboard "${device}/device/name"
	then
		exec keystat-capture "$1" "/dev/input/$(basename ${device})"
	fi
done

echo "No suitable device found" >&2
exit 1
