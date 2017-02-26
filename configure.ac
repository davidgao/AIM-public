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

# autoconf requirements
AC_PREREQ([2.69])

AC_INIT([AIM], [0.2], [davidgao1001@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([./m4])

# Disable the default -O2 which overrides the -Os for the bootloader
# -g will be handled later.
: ${CFLAGS=""}

AC_PROG_CC
AIM_PROG_CC_C11
AS_CASE([$aim_prog_cc_c11],
	[no], [AC_MSG_ERROR([Compiler does not support C11.])]
)
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE
AC_C_TYPEOF
AIM_GCC_FUNC_ATTRIBUTE([noreturn])
AIM_GCC_FUNC_ATTRIBUTE([weak])
AIM_GCC_FUNC_ATTRIBUTE([noinline])
AIM_GCC_FUNC_ATTRIBUTE([visibility])
AIM_GCC_VAR_ATTRIBUTE([used])
# TODO: test for compiler functionality instead of flag acceptance
AIM_REQUIRE_COMPILE_FLAG([-fplan9-extensions])
AIM_REQUIRE_COMPILE_FLAG([-fno-stack-protector])

# Builtin functions - not required
AX_GCC_BUILTIN(__builtin_popcount)
AX_GCC_BUILTIN(__builtin_popcountl)
AX_GCC_BUILTIN(__builtin_popcountll)

# Builtin functions - required since we are lazy
AX_GCC_BUILTIN(__builtin_bswap32)
AX_GCC_BUILTIN(__builtin_bswap64)

AM_PROG_AS
AM_PROG_AR
AC_CHECK_TOOL([OBJDUMP], [objdump])
AC_CHECK_TOOL([OBJCOPY], [objcopy])

LT_INIT

# Compiler functionalities
AIM_ARG_ENABLE([g], [FLAGS_G], [pass -g to compiler(s)], [yes])
AS_VAR_IF([enable_g], [yes], [AIM_REQUIRE_COMPILE_FLAG([-g])])
AIM_ARG_ENABLE([ggdb], [FLAGS_GGDB], [pass -ggdb to compiler(s)], [yes])
AS_VAR_IF([enable_ggdb], [yes], [AIM_REQUIRE_COMPILE_FLAG([-ggdb])])
AIM_ARG_ENABLE([o2], [FLAGS_O2], [pass -O2 to compiler(s)], [yes])
AS_VAR_IF([enable_o2], [yes], [AIM_REQUIRE_COMPILE_FLAG([-O2])])
AIM_ARG_ENABLE([wall], [FLAGS_WALL], [pass -Wall to compiler(s)], [yes])
AS_VAR_IF([enable_wall], [yes], [AIM_REQUIRE_COMPILE_FLAG([-Wall])])
AIM_ARG_ENABLE([wextra], [FLAGS_WEXTRA], [pass -Wextra to compiler(s)], [yes])
AS_VAR_IF([enable_wextra], [yes], [AIM_REQUIRE_COMPILE_FLAG([-Wextra])])
AIM_ARG_ENABLE([waddress], [FLAGS_WADDRESS], [pass -Waddress to compiler(s)], [yes])
AS_VAR_IF([enable_waddress], [yes], [AIM_REQUIRE_COMPILE_FLAG([-Waddress])])
AIM_ARG_ENABLE([wconversion], [FLAGS_WCONVERSION], [pass -Wconversion to compiler(s)], [yes])
AS_VAR_IF([enable_wconversion], [yes], [AIM_REQUIRE_COMPILE_FLAG([-Wconversion])])
AIM_ARG_ENABLE([werror], [FLAGS_WERROR], [pass -Werror to compiler(s)], [yes])
AS_VAR_IF([enable_werror], [yes], [AIM_REQUIRE_COMPILE_FLAG([-Werror])])

AC_SUBST([AIM_CFLAGS])

AIM_INIT_ARCH
AIM_INIT_MACH

# Extra Functionalities
AIM_ARG_ENABLE_FEATURE([debug], [DEBUG], [Debugging (kpdebug)], [yes])
AIM_ARG_ENABLE_FEATURE([tests], [TESTS], [in-kernel test framework], [yes])
AIM_ARG_ENABLE([firmware], [BUILD_FIRMWARE], [simple firmware shipped with AIM], [yes])
AIM_ARG_ENABLE([bootloader], [BUILD_BOOTLOADER], [simple bootloader shipped with AIM], [yes])

# msim
AIM_ARG_WITH([msim-kbd-pa], [MSIM_KBD_PHYSADDR],
	[MSIM keyboard physical address])
AIM_ARG_WITH([msim-lp-pa], [MSIM_LP_PHYSADDR],
	[MSIM printer physical address])
AIM_ARG_WITH([msim-disk-pa], [MSIM_DISK_PHYSADDR],
	[MSIM disk physical address])
AIM_ARG_WITH([msim-order-pa], [MSIM_ORDER_PHYSADDR],
	[MSIM IPC device physical address])
AIM_ARG_WITH([msim-mb-base], [MSIM_ORDER_MAILBOX_BASE],
	[MSIM IPC device mailbox base address])
AIM_ARG_WITH([msim-mb-order], [MSIM_ORDER_MAILBOX_ORDER],
	[order of MSIM IPC device mailbox (size=1<<order)])
AIM_ARG_WITH([msim-rtc-pa], [MSIM_RTC_PHYSADDR],
	[MSIM real time clock physical address])
AIM_ARG_WITH([msim-firmware-bin], [MSIM_FIRMWARE_BIN],
	[MSIM firmware binary file name])
AIM_ARG_WITH([msim-disk-img], [MSIM_DISK_IMG],
	[MSIM disk image file name])
AIM_ARG_WITH([highram-base], [HIGHRAM_BASE],
	[MIPS high RAM base address])
AIM_ARG_WITH([highram-size], [HIGHRAM_SIZE],
	[MIPS high RAM size])

# Loongson 3A
AIM_ARG_ENABLE_FEATURE([loongson3a-ram-detection], [LOONGSON3A_RAM_DETECTION],
	[RAM space detection on Loongson 3A port (RECOMMENDED)])
AIM_ARG_ENABLE_FEATURE([pagesize-16k], [PAGESIZE_16K],
	[16K pages (for Loongson 3A)])
AIM_ARG_ENABLE_FEATURE([ide-bmdma], [IDE_BMDMA],
	[PCI IDE Bus-mastering DMA])

# MEMORY
AIM_ARG_WITH([ram-physbase], [RAM_PHYSBASE], [physical memory start point],
	[0x00000000])

AIM_ARG_WITH([kern-start], [KERN_START], [kernel loading physical address],
	[0x00000000])

AIM_ARG_WITH([mem-size], [MEM_SIZE], [amount of memory installed], [DETECT])

# CPU
AIM_ARG_WITH([cpus], [NR_CPUS], [number of cores], [DETECT])
AIM_ARG_WITH([max-cpus], [MAX_CPUS], [maximum number of cores], [NR_CPUS])
AIM_ARG_WITH([cpu-freq], [CPU_FREQ], [cpu ticks per second], [DETECT])

# Kernel hyperparameters
# TODO: for addresses, make two default configurations for 32-bit and 64-bit
# respectively.
AIM_ARG_WITH([kern-base], [KERN_BASE], [start of kernel address], [0x80000000])

AIM_ARG_WITH([kmmap-base], [KMMAP_BASE], [start of kernel dynamic mapping],
	[0xf0000000])

AIM_ARG_WITH([reserved-base], [RESERVED_BASE], [start of reserved high address],
	[0xfffff000])

AIM_ARG_WITH([usertop], [USERTOP],
	[top of user program address space], [0x7ff00000])

AIM_ARG_WITH([ustacksize], [USTACKSIZE],
	[size of user stack], [32768])

AIM_ARG_WITH([kstacksize], [KSTACKSIZE],
	[size of kernel stack], [32768])

AIM_ARG_WITH([root-partition-id], [ROOT_PARTITION_ID],
	[root partition number], [4])

AIM_ARG_WITH([max-processes], [MAX_PROCESSES], [maximum number of processes],
	[65536])

AIM_ARG_WITH([pipe-size], [PIPE_SIZE], [pipe size], [16384])

AIM_ARG_WITH([fwstack-order], [FWSTACKORDER],
	[order of firmware stack per CPU (size=1<<order)])

AIM_ARG_WITH([timer-freq], [TIMER_FREQ],
	[timer interrupt frequency], [100])

# Drivers
AIM_ARG_ENABLE([io-mem], [IO_MEM], [memory-mapped IO driver])
AIM_ARG_ENABLE_FEATURE([io-mem-root], [IO_MEM_ROOT],
	[memory-mapped IO bus as device tree root])
AIM_ARG_ENABLE([io-port], [IO_PORT], [port IO driver])
AIM_ARG_ENABLE_FEATURE([io-port-root], [IO_PORT_ROOT],
	[port IO bus as device tree root])
AIM_ARG_ENABLE([uart-lite], [UART_LITE], [UART-lite driver])
AIM_ARG_ENABLE([uart-msim], [UART_MSIM], [MSIM UART driver])
AIM_ARG_ENABLE([uart-ns16550], [UART_NS16550], [NS16550 UART driver])
AIM_ARG_ENABLE([block-msim], [BLOCK_MSIM], [MSIM block device driver])
AIM_ARG_ENABLE([sd-zynq], [SD_ZYNQ], [zynq SD driver])
AIM_ARG_ENABLE([timer-a9], [TIMER_A9], [Cortex-A9 MPCore timer driver])
AIM_ARG_ENABLE([bus-mapper], [BUS_MAPPER], [bus mapper driver])

# Primary Driver Selection
AIM_ARG_WITH([primary-console], [PRIMARY_CONSOLE], [primary console])
AIM_ARG_WITH([primary-storage], [PRIMARY_STORAGE], [primary storage])
AIM_ARG_WITH([primary-timing], [PRIMARY_TIMING], [primary timing])

AS_CASE([$with_primary_console],
	[UART_LITE],[
		AC_DEFINE_UNQUOTED([CONSOLE_NEED_CR], [], [Primary console needs CR])
	]
)

# Algorithms
# MUST have unique names.
# Two kinds of algorithm roles exist:
# - Hard-wired: Provides fundamental functionalities. Kernel hard-wire calls
#   to their init functions.
# - Modular: Provides higher-level functionalities. Must be linked within
#   the kernel but initialized via initcalls. Most of these initcalls are of
#   early stage.
# Algorithms register their interface structure when initialization routines
# are called. This routine SHOULD NOT really initialize the algorithm.
# The "with" syntax is used so that at most ONE algorithm is selected for each
# role.
# First fit is named "ff" as page allocator, "flff" as simple allocator.
AIM_ARG_WITH([simple-allocator], [SIMPLE_ALLOCATOR], [non-caching memory object allocator], [FLFF])
AM_CONDITIONAL([ALGO_FLFF], [test x$with_simple_allocator = xFLFF])

AIM_ARG_WITH([page-allocator], [PAGE_ALLOCATOR], [page allocator], [FF])
AM_CONDITIONAL([ALGO_FF], [test x$with_page_allocator = xFF])

AIM_ARG_WITH([caching-allocator], [CACHING_ALLOCATOR], [caching allocator], [SLAB])
AM_CONDITIONAL([ALGO_SLAB], [test x$with_caching_allocator = xSLAB])

AIM_ARG_WITH([dev-index], [DEVICE_INDEX], [device index], [DEVLIST])
AM_CONDITIONAL([ALGO_DEVLIST], [test x$with_dev_index = xDEVLIST])

AIM_ARG_WITH([kmmap-keeper], [KMMAP_KEEPER], [kmmap keeper], [KMLIST])
AM_CONDITIONAL([ALGO_KMLIST], [test x$with_kmmap_keeper = xKMLIST])

# change mips32 and mips64 to mips afterwards, because the CFLAGS are
# different, but the source files are in the same directory.
if test x$ARCH = xmips64; then
	ARCH=mips
fi

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
	Makefile
	boot/Makefile
	boot/arch/Makefile
	boot/arch/armv7a/Makefile
	boot/arch/i386/Makefile
	boot/arch/mips/Makefile
	drivers/Makefile
	drivers/io/Makefile
	drivers/serial/Makefile
	include/Makefile
	include/aim/Makefile
	include/arch/Makefile
	include/arch/armv7a/Makefile
	include/arch/i386/Makefile
	include/arch/mips/Makefile
	include/arch/mips/mach-loongson3a/Makefile
	kern/Makefile
	kern/arch/Makefile
	kern/arch/armv7a/Makefile
	kern/arch/i386/Makefile
	kern/arch/mips/Makefile
	kern/dev/Makefile
	kern/init/Makefile
	kern/mm/Makefile
	kern/mm/kmmap/Makefile
	kern/panic/Makefile
	lib/Makefile
	lib/libc/Makefile
])
AC_OUTPUT

AIM_SUMMARY

