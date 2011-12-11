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

PREFIX ?= usr

bin    := $(DESTDIR)/$(PREFIX)/bin

install: \
	$(bin)/keystat-capture \
	$(bin)/keystat-dump \
	$(bin)/keystat-service \
	$(bin)/keystat-translate

$(bin)/keystat-capture: capture $(bin)
	cp -p $< $@

$(bin)/keystat-dump: dump $(bin)
	cp -p $< $@

$(bin)/keystat-service: service.sh $(bin)
	cp -p $< $@

$(bin)/keystat-translate: translate.lua $(bin)
	cp -p $< $@

$(bin):
	mkdir -p $@

.PHONY: clean install
