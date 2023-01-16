.POSIX:
.PHONY: all

include config.mk

all: dvd_fb

dvd_fb: dvd_fb.o
	$(CC) $(LDFLAGS) dvd_fb.o -o dvd_fb $(LDLIBS)

clean:
	rm -f dvd_fb.o dvd_fb
