CC = clang
DESTDIR = /
PREFIX = usr

keystat: keystat.c
	$(CC) $(CFLAGS) -std=c99 -o $@ $<

clean:
	rm -f keystat

install: keystat
	install -d $(DESTDIR)$(PREFIX)/bin
	install keystat $(DESTDIR)$(PREFIX)/bin

.PHONY: clean install
