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

dist_noinst_DATA = boot.lds.S

noinst_LTLIBRARIES = \
	libentry.la \
	libarch.la

libentry_la_SOURCES = \
	bootasm.S
libentry_la_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
libentry_la_CFLAGS = $(AM_CFLAGS_NOPIC) -Os
libentry_la_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)

libarch_la_SOURCES =
libarch_la_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
libarch_la_CFLAGS = $(AM_CFLAGS_NOPIC) -Os
libarch_la_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)

