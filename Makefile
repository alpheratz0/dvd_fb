.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: dvd_fb

dvd_fb: dvd_fb.o
	$(CC) $(LDFLAGS) -o dvd_fb dvd_fb.o $(LDLIBS)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dvd_fb $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dvd_fb
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f dvd_fb.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/dvd_fb.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dvd_fb
	rm -f $(DESTDIR)$(MANPREFIX)/man1/dvd_fb.1

dist: clean
	mkdir -p dvd_fb-$(VERSION)
	cp -R COPYING config.mk Makefile README dvd_fb.1 \
		dvd_fb.c dvd_logo.h dvd_fb-$(VERSION)
	tar -cf dvd_fb-$(VERSION).tar dvd_fb-$(VERSION)
	gzip dvd_fb-$(VERSION).tar
	rm -rf dvd_fb-$(VERSION)

clean:
	rm -f dvd_fb dvd_fb.o dvd_fb-$(VERSION).tar.gz
