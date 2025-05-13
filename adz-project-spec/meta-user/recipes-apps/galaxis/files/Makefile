#	Makefile for the galaxis game
#
# SPDX-FileCopyrightText: (C) Eric S. Raymond <esr@thyrsus.com>
# SPDX-License-Identifier: BSD-2-Clause

VERS=$(shell sed -n <NEWS.adoc '/^[0-9]/s/:.*//p' | head -1)

# Flags for use with the Linux ncurses package (recommended)
CFLAGS = -g -DNDEBUG
TERMLIB = -lncurses
CC = gcc

galaxis: galaxis.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o galaxis galaxis.c $(TERMLIB)

# Note: to suppress the footers with timestamps being generated in HTML,
# we use "-a nofooter".
# To debug asciidoc problems, you may need to run "xmllint --nonet --noout --valid"
# on the intermediate XML that throws an error.
.SUFFIXES: .html .adoc .6

.adoc.6:
	asciidoctor -D. -a nofooter -b manpage $<
.adoc.html:
	asciidoctor -D. -a nofooter -a webfonts! $<

install: galaxis.6 uninstall
	install -m 755 -o 0 -g 0 -d $(ROOT)/usr/bin/
	install -m 755 -o 0 -g 0 galaxis $(ROOT)/usr/bin/galaxis
	install -m 755 -o 0 -g 0 -d $(ROOT)/usr/share/man/man6/
	install -m 755 -o 0 -g 0 galaxis.6 $(ROOT)/usr/share/man/man6/galaxis.6

uninstall:
	rm -f ${ROOT}/usr/bin/galaxis ${ROOT}/usr/share/man/man6/galaxis.6

clean:
	rm -f galaxis galaxis.6 galaxis-*.tar.gz *~ *.html

reflow:
	@clang-format --style="{IndentWidth: 8, UseTab: ForIndentation}" -i $$(find . -name "*.[ch]")

CPPCHECKOPTS =
cppcheck:
	cppcheck -DREVISION=$(VERS) $(CPPCHECKOPTS) galaxis.c

version:
	@echo $(VERS)

SOURCES = README.adoc COPYING NEWS.adoc control galaxis.c Makefile galaxis.adoc galaxis.desktop

galaxis-$(VERS).tar.gz: $(SOURCES) galaxis.6
	tar --transform='s:^:galaxis-$(VERS)/:' --show-transformed-names -cvzf galaxis-$(VERS).tar.gz $(SOURCES) galaxis.6

dist: galaxis-$(VERS).tar.gz

release: galaxis-$(VERS).tar.gz galaxis.html
	shipper version=$(VERS) | sh -e -x

refresh: galaxis.html
	shipper -N -w version=$(VERS) | sh -e -x
