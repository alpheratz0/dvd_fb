# Copyright (C) 2022-2023 <alpheratz99@protonmail.com>
# This program is free software.

VERSION = 0.0.1

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Os $(INCS) -DVERSION=\"$(VERSION)\"
LDFLAGS = -s -ltfb

CC = cc
