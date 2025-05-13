#	Makefile for bs, the Battleships game
#
# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: (C) Eric S. Raymond <esr@thyrsus.com>

VERSION=$(shell sed -n <NEWS.adoc '/::/s/^\([0-9][^:]*\).*/\1/p' | head -1)

CC = gcc

# DESTDIR support
DESTDIR = 

bs: bs.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DVERSION='"$(VERSION)"' -o bs bs.c -lncurses

# Note: to suppress the footers with timestamps being generated in HTML,
# we use "-a nofooter".
# To debug asciidoc problems, you may need to run "xmllint --nonet --noout --valid"
# on the intermediate XML that throws an error.
.SUFFIXES: .html .adoc .6

.adoc.6:
	asciidoctor -D. -a nofooter -b manpage $<
.adoc.html:
	asciidoctor -D. -a nofooter -a webfonts! $<

clean:
	rm -f bs bs.6 bs*.tar.gz bs*.rpm *~ *.html

reflow:
	@clang-format --style="{IndentWidth: 8, UseTab: ForIndentation}" -i $$(find . -name "*.[ch]")

cppcheck:
	cppcheck --quiet --template gcc --enable=all --suppress=missingIncludeSystem bs.c

# Note: does not install desktop file.
install: bs.6 uninstall
	install -m 0755 -d $(DESTDIR)/usr/bin
	install -m 0755 -d $(DESTDIR)/usr/share/man/man6
	install -m 0755 -d $(DESTDIR)//usr/share/applications/
	install -m 0755 -d $(DESTDIR)/usr/share/icons/hicolor/32x32/apps/
	install -m 0755 -d $(DESTDIR)/usr/share/appdata
	install -m 0755 bs $(DESTDIR)/usr/bin/
	install -m 0644 bs.6 $(DESTDIR)/usr/share/man/man6/
	install -m 0644 bs.desktop $(DESTDIR)/usr/share/applications/
	install -m 0644 battleship.png $(DESTDIR)/usr/share/icons/hicolor/32x32/apps/
	install -m 0644 bs.adoc $(DESTDIR)/usr/share/appdata/

uninstall:
	rm -f /usr/bin/bs /usr/share/man/man6/bs.6

SOURCES = README COPYING NEWS.adoc control bs.c Makefile bs.adoc bs.desktop

bs-$(VERSION).tar.gz: $(SOURCES) bs.6
	tar --transform='s:^:bs-$(VERSION)/:' --show-transformed-names -cvzf bs-$(VERSION).tar.gz $(SOURCES) bs.6

dist: bs-$(VERSION).tar.gz

release: bs-$(VERSION).tar.gz bs.html
	shipper version=$(VERSION) | sh -e -x

refresh: bs.html
	shipper -N -w version=$(VERSION) | sh -e -x

