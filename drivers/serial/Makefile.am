# Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
# Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

noinst_LTLIBRARIES = libserial-raw.la libserial.la
noinst_PROGRAMS =
noinst_HEADERS =

SRCS =
MODULE_SRCS =

if UART_NS16550
MODULE_SRCS += uart-ns16550.c
noinst_PROGRAMS += uart-ns16550.ko
uart_ns16550_ko_SOURCES = uart-ns16550.c #uart-ns16550-kernel.c
uart_ns16550_ko_LDFLAGS = $(AM_LDFLAGS) -Wl,-r
noinst_HEADERS += uart-ns16550-hw.h
endif

libserial_raw_la_SOURCES = $(SRCS) $(MODULE_SRCS)
libserial_raw_la_CPPFLAGS = $(AM_CPPFLAGS_NOPIC)
libserial_raw_la_CFLAGS = $(AM_CFLAGS_NOPIC)
libserial_raw_la_CCASFLAGS = $(AM_CCASFLAGS_NOPIC)
libserial_raw_la_LDFLAGS = $(AM_LDFLAGS_NOPIC)

libserial_la_SOURCES = $(SRCS)

