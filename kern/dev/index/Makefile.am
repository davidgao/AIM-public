# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
#
# This file is part of AIM.
#
# AIM is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AIM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

include $(top_srcdir)/env.am

noinst_LTLIBRARIES = libindex.la 
noinst_PROGRAMS =

noinst_LTLIBRARIES += libdevlist-raw.la
MODULE_SRCS = devlist.c
libdevlist_raw_la_SOURCES = devlist.c
libdevlist_raw_la_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
libdevlist_raw_la_CFLAGS = $(AM_CFLAGS_NOPIC)
libdevlist_raw_la_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)
libdevlist_raw_la_LDFLAGS = $(AM_LDFLAGS_NOPIC)


if ALGO_DEVLIST
noinst_PROGRAMS += devlist.ko
devlist_ko_SOURCES = devlist.c
devlist_ko_LDFLAGS = $(AM_LDFLAGS) -Wl,-r
endif

libindex_la_SOURCES = index.c

