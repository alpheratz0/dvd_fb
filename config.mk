# Copyright (C) 2023 <alpheratz99@protonmail.com>
# This program is free software.

VERSION   = 0.0.1

CC        = cc
CFLAGS    = -std=c99 -pedantic -Wall -Wextra -Os -DVERSION=\"$(VERSION)\"
LDLIBS    = -ltfb
LDFLAGS   = -s

PREFIX    = /usr/local
