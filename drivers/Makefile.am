# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
# Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

SUBDIRS = \
	io \
	serial

noinst_LTLIBRARIES = libdrivers-raw.la libdrivers.la

libdrivers_raw_la_SOURCES =
libdrivers_raw_la_LIBADD = \
	io/libio-raw.la \
	serial/libserial-raw.la
libdrivers_raw_la_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
libdrivers_raw_la_CFLAGS = $(AM_CFLAGS_NOPIC)
libdrivers_raw_la_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)
libdrivers_raw_la_LDFLAGS = $(AM_LDFLAGS_NOPIC)

libdrivers_la_SOURCES =
libdrivers_la_LIBADD = \
	io/libio.la \
	serial/libserial.la

