CC = clang
DESTDIR = /
PREFIX = usr

all: capture dump

capture: capture.c
	$(CC) $(CFLAGS) -std=c99 -o $@ $<

dump: dump.c
	$(CC) $(CFLAGS) -std=c99 -o $@ $<

clean:
	rm -f keystat

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -T capture $(DESTDIR)$(PREFIX)/bin/keystat-capture
	install -T dump $(DESTDIR)$(PREFIX)/bin/keystat-dump
	install -T device.sh $(DESTDIR)$(PREFIX)/bin/keystat-device

.PHONY: clean install
