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
include $(top_srcdir)/modules.am

SUBDIRS = \
	arch \
	dev \
	init \
	mm \
	panic \
	trap

BUILT_SOURCES = vmaim.lds

noinst_LTLIBRARIES = libkernel.la
noinst_PROGRAMS = vmaim.elf
nodist_noinst_DATA = vmaim.s
CLEANFILES = boot.lds vmaim.s

# This list should always be ordered carefully
KERN_LIBS = \
	arch/$(ARCH)/libentry.la \
	dev/libdev.la \
	init/libearlyinit.la \
	mm/libmm.la \
	panic/libpanic.la \
	trap/libtrap.la \
	arch/$(ARCH)/libarch.la \
	../drivers/libdrivers.la \
	../lib/libc/libc.la

libkernel_la_SOURCES =
libkernel_la_LIBADD = $(KERN_LIBS)

vmaim_elf_SOURCES =
vmaim_elf_LDADD = $(MODULES) libkernel.la
EXTRA_vmaim_elf_DEPENDENCIES = vmaim.lds
vmaim_elf_LDFLAGS = $(AM_LDFLAGS) -T vmaim.lds

# This file has double suffix, makefile rule cannot wildcard him
vmaim.lds: $(srcdir)/arch/$(ARCH)/vmaim.lds.S
	$(AM_V_GEN) $(COMPILE) -E -P $^ -o $@

