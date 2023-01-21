.POSIX:
.PHONY: all install uninstall

include config.mk

all: dvd_fb

dvd_fb: dvd_fb.o
	$(CC) $(LDFLAGS) dvd_fb.o -o dvd_fb $(LDLIBS)

install: dvd_fb
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dvd_fb $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dvd_fb

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dvd_fb

clean:
	rm -f dvd_fb.o dvd_fb
