/*
	Copyright (C) 2023-2024 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 2 as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA 02111-1307 USA

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

#define UnitsPerSecond 220
#define MicrosecondsPerSecond 1000000

#define RedChannel(c) ((c >> 16) & 0xff)
#define GreenChannel(c) ((c >> 8) & 0xff)
#define BlueChannel(c) (c & 0xff)

#define LerpChannel(ch, v) ((ch * v) / 0xff)
#define MakeColor(r, g, b) ((r << 16) | (g << 8) | b)

#define LerpColor(c, v) \
	MakeColor( \
		LerpChannel(RedChannel(c), v), \
		LerpChannel(GreenChannel(c), v), \
		LerpChannel(BlueChannel(c), v) \
	)

static int fb_acquired;
static uint32_t dvd_logo_colorized[sizeof(dvd_logo_alpha)];

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
set_dvd_logo_color(uint32_t color)
{
	for (size_t i = 0; i < sizeof(dvd_logo_colorized) / sizeof(dvd_logo_colorized[0]); ++i) {
		dvd_logo_colorized[i] = LerpColor(color, dvd_logo_alpha[i]);
	}
}

static void
draw_dvd_logo(int x, int y)
{
	for (int cy = 0; cy < DVD_LOGO_HEIGHT; ++cy) {
		for (int cx = 0; cx < DVD_LOGO_WIDTH; ++cx) {
			tfb_draw_pixel(x + cx, y + cy, dvd_logo_colorized[cy*DVD_LOGO_WIDTH+cx]);
		}
	}
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
	set_dvd_logo_color(rand());
	w = tfb_screen_width();
	h = tfb_screen_height();

	tfb_clear_screen(0x000000);

	// Make sure true != false.
	while (true != false) {
		tfb_draw_rect(x, y, DVD_LOGO_WIDTH, DVD_LOGO_HEIGHT, 0x000000);

		if ((x += xdir) % (w - DVD_LOGO_WIDTH) == 0)
			xdir *= -1, set_dvd_logo_color(rand());

		if ((y += ydir) % (h - DVD_LOGO_HEIGHT) == 0)
			ydir *= -1, set_dvd_logo_color(rand());

		draw_dvd_logo(x, y);
		usleep(MicrosecondsPerSecond / UnitsPerSecond);
	}

	/* UNREACHABLE */
	return 0;
}
