/*
	Copyright (C) 2023 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License version 2 as published by the
	Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place, Suite 330, Boston, MA 02111-1307 USA

*/

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tfblib/tfblib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "dvd_logo.h"

#define UNITS_PER_SECOND                   (220)
#define MICROSECONDS_PER_SECOND            (1000*1000)

#define red(c) ((c>>16)&0xff)
#define green(c) ((c>>8)&0xff)
#define blue(c) ((c>>0)&0xff)

#define channellerp(ch,v) ((ch*v)/0xff)

#define colorzlerp(c,v) \
	((channellerp(red(c), v)   << 16) | \
	 (channellerp(green(c), v) <<  8) | \
	 (channellerp(blue(c), v)  <<  0))

static int fb_acquired;

static void
die(const char *fmt, ...)
{
	va_list args;

	fputs("dvd_fb: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static void
set_signal_handler(int sig, void(*handler)(int))
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);

	sa.sa_flags = 0;
	sa.sa_handler = handler;

	if (sigaction(sig, &sa, NULL) == -1)
		die("couldn't register signal handler");
}

static void
draw_dvd_logo(int x, int y, uint32_t color)
{
	int cx, cy;
	for (cy = 0; cy < DVD_LOGO_HEIGHT; ++cy)
		for (cx = 0; cx < DVD_LOGO_WIDTH; ++cx)
			tfb_draw_pixel(x + cx, y + cy,
					colorzlerp(color, dvd_logo_alpha[cy*DVD_LOGO_WIDTH+cx]));
}

static void
h_exit(void)
{
	if (fb_acquired)
		tfb_release_fb();
}

static void
h_sigint(int sig)
{
	(void) sig;
	exit(1);
}

static void
usage(void)
{
	puts("usage: dvd_fb [-hv]");
	exit(0);
}

static void
version(void)
{
	puts("dvd_fb version "VERSION);
	exit(0);
}

int
main(int argc, char **argv)
{
	int x, y, w, h, xdir, ydir;
	uint32_t color;

	while (++argv, --argc > 0) {
		if ((*argv)[0] == '-' && (*argv)[1] != '\0' && (*argv)[2] == '\0') {
			switch ((*argv)[1]) {
			case 'h': usage(); break;
			case 'v': version(); break;
			default: die("invalid option %s", *argv); break;
			}
		} else {
			die("unexpected argument: %s", *argv);
		}
	}

	if (atexit(h_exit) != 0)
		die("atexit: failed to set exit handler");

	if (tfb_acquire_fb(0, NULL, NULL) != TFB_SUCCESS)
		die("couldn't acquire framebuffer");

	fb_acquired = 1;
	set_signal_handler(SIGINT, h_sigint);
	srand(getpid());

	x = y = 0;
	xdir = ydir = 1;
	color = rand();
	w = tfb_screen_width();
	h = tfb_screen_height();

	tfb_clear_screen(0x000000);

	for (;;) {
		tfb_draw_rect(x, y, DVD_LOGO_WIDTH, DVD_LOGO_HEIGHT, 0x000000);

		if ((x += xdir) % (w - DVD_LOGO_WIDTH) == 0)
			xdir *= -1, color = rand();

		if ((y += ydir) % (h - DVD_LOGO_HEIGHT) == 0)
			ydir *= -1, color = rand();

		draw_dvd_logo(x, y, color);
		usleep(MICROSECONDS_PER_SECOND / UNITS_PER_SECOND);
	}

	/* UNREACHABLE */
	return 0;
}
