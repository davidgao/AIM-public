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

# AIM_SUPPORT_ARCH([space_separated_list])
# E.g. if ARCH=armv7a, define an automake conditional ARCH_ARMV7A
AC_DEFUN([AIM_SUPPORT_ARCH], [
	m4_foreach_w([var], [$1], [
		AM_CONDITIONAL([ARCH_]m4_toupper(var), [test x$ARCH = x]var)
	])
])

AC_DEFUN([AIM_INIT_ARCH], [
	AIM_ARG_VAR([ARCH], [target architecture])
	AIM_SUPPORT_ARCH([armv7a i386 i686 mips mips64 x86_64])
	AS_CASE([$ARCH],
		[arm*], [
			AS_VAR_SET([__enable_io_mem], [yes])
			AS_VAR_SET([__enable_io_port], [no])
			AS_VAR_SET([__enable_io_mem_root], [yes])
			AS_VAR_SET([__enable_io_port_root], [no])
			AS_VAR_SET([__enable_bus_mapper], [yes])
		],
		[mips32], [
			AC_DEFINE(USE_MIPS32, [], [MIPS32 ABI and instruction set])
			AS_VAR_SET([__enable_io_mem], [yes])
			AS_VAR_SET([__enable_io_port], [no])
			AS_VAR_SET([__enable_io_mem_root], [yes])
			AS_VAR_SET([__enable_io_port_root], [no])
		],
		[mips64], [
			AC_DEFINE(USE_MIPS64, [], [MIPS64r2 ABI and instruction set])
			AS_VAR_SET([__enable_io_mem], [yes])
			AS_VAR_SET([__enable_io_port], [no])
			AS_VAR_SET([__enable_io_mem_root], [yes])
			AS_VAR_SET([__enable_io_port_root], [no])
		],
		[i386], [
			AS_VAR_SET([__with_ram_physbase], [0x01000000])
			AS_VAR_SET([__with_kern_start], [0x01000000])
			AS_VAR_SET([__enable_io_mem], [yes])
			AS_VAR_SET([__enable_io_port], [yes])
			AS_VAR_SET([__enable_io_mem_root], [yes])
			AS_VAR_SET([__enable_io_port_root], [yes])
			AS_VAR_SET([__enable_uart_ns16550], [yes])
			AS_VAR_SET([__with_primary_console], [uart-ns16550])
		],
		[x86_64], [
			AS_VAR_SET([__with_ram_physbase], [0x01000000])
			AS_VAR_SET([__with_kern_start], [0x01000000])
			AS_VAR_SET([__enable_io_mem], [yes])
			AS_VAR_SET([__enable_io_port], [yes])
			AS_VAR_SET([__enable_io_mem_root], [yes])
			AS_VAR_SET([__enable_io_port_root], [yes])
			AS_VAR_SET([__enable_uart_ns16550], [yes])
			AS_VAR_SET([__with_primary_console], [uart-ns16550])
		]
	)
])

