# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
#
# This program is part of AIM.
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

noinst_LTLIBRARIES = libio-raw.la libio.la
noinst_HEADERS =

SRCS = 

if IO_MEM
SRCS += io-mem.c
noinst_HEADERS += io-mem.h
endif

if IO_PORT
SRCS += io-port.c
noinst_HEADERS += io-port.h
endif

libio_raw_la_SOURCES = $(SRCS)
libio_raw_la_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
libio_raw_la_CFLAGS = $(AM_CFLAGS_NOPIC)
libio_raw_la_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)
libio_raw_la_LDFLAGS = $(AM_LDFLAGS_NOPIC)

libio_la_SOURCES = $(SRCS)

