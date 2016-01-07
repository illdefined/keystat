CPPFLAGS ?= -pedantic-errors -Wall
CFLAGS   ?= -pipe -Wextra -O2

CPPFLAGS += -std=c99 -D_XOPEN_SOURCE=600 -ftabstop=4

everything: capture dump

capture: capture.c
dump: dump.c

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

.c:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $>

clean:
	rm -f capture dump

PREFIX          ?= usr
LIBDIR          ?= lib
SYSTEMD_UNITDIR ?= $(LIBDIR)/systemd/system

bin             := $(DESTDIR)/$(PREFIX)/bin
unit            := $(DESTDIR)/$(SYSTEMD_UNITDIR)
env             := $(DESTDIR)/etc/default

install: \
	$(bin)/keystat-capture \
	$(bin)/keystat-dump \
	$(bin)/keystat-translate \
	$(unit)/keystat.service \
	$(env)/keystat

$(bin)/keystat-capture: capture $(bin)
	cp -p $< $@

$(bin)/keystat-dump: dump $(bin)
	cp -p $< $@

$(bin)/keystat-translate: translate.py $(bin)
	cp -p $< $@

$(unit)/keystat.service: keystat.service $(unit)
	cp -p $< $@

$(env)/keystat: keystat.env $(env)
	cp -p $< $@

$(bin):
	mkdir -p $@

$(unit):
	mkdir -p $@

$(env):
	mkdir -p $@

.PHONY: clean install
