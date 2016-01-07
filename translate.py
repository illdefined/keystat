#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess
import sys
import re

if len(sys.argv) >= 2:
    try:
        dump = open(sys.argv[1])
    except:
        print("usage: %s <filename>"%sys.argv[0])
        sys.exit(1)
else:
    dump = sys.stdin

positions_to_translate = [0,1,2]
offset = -8

def load_mapping_from_xmodmap():
    keymap = subprocess.check_output(["xmodmap","-pke"]).decode("utf-8")
    mapping = {}

    for line in keymap.split("\n"):
        parts = line.split()
        if len(parts) > 4:
            mapping[int(parts[1])+offset] = parts[3]
    return mapping
def load_mapping_from_linux_input_h():
    mapping = {}
    with open("/usr/include/linux/input.h") as headerfile:
        regex = "^#define KEY_([^\s]*)\s*(\d*)"
        finds = re.findall(regex, headerfile.read(), flags=re.MULTILINE)
        for keyname, scancode in finds:
            if keyname and scancode:
                mapping[int(scancode)] = keyname
    return mapping
def cmd_exists(cmd): # from http://stackoverflow.com/a/11069822/3890934
    return subprocess.call("type " + cmd, shell=True,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE) == 0

if cmd_exists("xmodmap"):
    try:
        mapping = load_mapping_from_xmodmap()
    except:
        mapping = load_mapping_from_linux_input_h()
else:
    mapping = load_mapping_from_linux_input_h()

table = []

for line in dump:
    parts = line.split()
    if int(parts[3]) > 0:
        for position in positions_to_translate:
            parts[position] = mapping[int(parts[position])]
        table.append(parts)

table.sort(key=lambda x: int(x[3]), reverse=True)

for row in table:
    try:
        print(" ".join(row))
    except Exception:
        break
