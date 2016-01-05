#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess
import sys
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
keymap = subprocess.check_output(["xmodmap","-pke"]).decode("utf-8")
mapping = {}

for line in keymap.split("\n"):
    parts = line.split()
    if len(parts) > 4:
        mapping[int(parts[1])+offset] = parts[3]

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
