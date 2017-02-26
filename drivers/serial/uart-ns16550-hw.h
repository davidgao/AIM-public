/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DRIVERS_SERIAL_UART_NS16550_HW_H
#define _DRIVERS_SERIAL_UART_NS16550_HW_H

/* Check for platform-specific UART frequencies UART_FREQ as well as
 * UART_BASE */
#include <platform.h>
/* If the platform has not defined UART frequency, we assume that it's
 * an IBM PC UART, whose frequency is 1.8432MHz */
#ifndef UART_FREQ
#define UART_FREQ		1843200
#endif

/*
 * NS16550 registers.
 *
 * References:
 * https://www.freebsd.org/doc/en/articles/serial-uart/
 */

#define UART_BAUDRATE		115200

/*
 * TODO: verify if this register specification, as given in Loongson 2H
 * manual, is compatible to others.
 */
#define UART_RCV_BUFFER		0	/* R/O */
#define UART_TRANS_HOLD		0	/* W/O */
#define UART_INTR_ENABLE	1
#define UART_INTR_ID		2	/* R/O */
#define UART_FIFO_CONTROL	2	/* W/O */
#define UART_LINE_CONTROL	3
#define UART_MODEM_CONTROL	4	/* W/O? */
#define UART_LINE_STATUS	5	/* R/O */
#define UART_MODEM_STATUS	6	/* R/O */

#define UART_DIVISOR_LSB	0	/* DLAB = 1 */
#define UART_DIVISOR_MSB	1	/* DLAB = 1 */

/* Interrupt Enable Register contents */
#define UART_IER_DSSI		0x08	/* Modem Status Interrupt */
#define UART_IER_LSI		0x04	/* Line Status Interrupt */
#define UART_IER_THREI		0x02	/* Transmitter Holding Register Empty */
#define UART_IER_RBFI		0x01	/* Receive Buffer Available */

/* Interrupt Identifier Register contents */
#define UART_IIR_IMASK		0x0e	/* AND this to get interrupt type */
# define UART_IIR_RECVERR	0x06	/* Receive Error */
# define UART_IIR_RBF		0x04	/* Data Available */
# define UART_IIR_STALE		0x0c	/* Stale Data */
# define UART_IIR_TBE		0x02	/* Transmit Buffer has more room */
# define UART_IIR_DSS		0x00	/* Modem Status Change */
#define UART_IIR_nIP		0x01	/* Interrupt Pending (negative) */

/* FIFO Control Register contents */
#define UART_FCR_RTB_1		0x00	/* Trigger when received 1 byte */
#define UART_FCR_RTB_4		0x40	/* ... 4 bytes */
#define UART_FCR_RTB_8		0x80	/* ... 8 bytes */
#define UART_FCR_RTB_14		0xc0	/* ... 14 bytes */
#define UART_FCR_RST_TRANSMIT	0x04	/* Reset Transmit FIFO */
#define UART_FCR_RST_RECEIVER	0x02	/* Reset Receiver FIFO*/
#define UART_FCR_ENABLE		0x01	/* Enable FIFO */

/* Line Control Register Masks */

/* Divisor Latch Access Bit */
#define UART_LCR_DLAB		0x80

/* Break Control Bit */
#define UART_LCR_BREAK		0x40

/* Parity selection */
#define UART_LCR_PARITY_NONE	0x00
#define UART_LCR_PARITY_ODD	0x08
#define UART_LCR_PARITY_EVEN	0x18
#define UART_LCR_PARITY_MARK	0x28
#define UART_LCR_PARITY_SPACE	0x38

/* Data length */
#define UART_LCR_DATA_5BIT	0x0
#define UART_LCR_DATA_6BIT	0x1
#define UART_LCR_DATA_7BIT	0x2
#define UART_LCR_DATA_8BIT	0x3

/* Stop bit selection */
#define UART_LCR_STOP_1BIT	0x0
#define UART_LCR_STOP_2BIT	0x4

/* Modem Control Register contents */
#define UART_MCR_LOOPBACK_CTL	0x10	/* Loopback control */
#define UART_MCR_OUT2		0x08	/* OUT 2 signal, for interrupts */
#define UART_MCR_OUT1		0x04	/* OUT 1 signal */
#define UART_MCR_RTSC		0x02	/* Request-To-Send control */
#define UART_MCR_DTRC		0x01	/* Data-Terminal-Ready control */

/* Line Status Register contents */
#define UART_LSR_ERROR		0x80	/* Error occurred */
#define UART_LSR_TEMT		0x40	/* Transmitter Empty */
#define UART_LSR_THRE		0x20	/* Transmitter Holding Register Empty */
#define UART_LSR_BREAK_INTR	0x10	/* Break Interrupt */
#define UART_LSR_FRAME_ERROR	0x08	/* self-descriptive */
#define UART_LSR_PARITY_ERROR	0x04	/* ditto */
#define UART_LSR_OVERRUN_ERROR	0x02	/* ditto */
#define UART_LSR_DATA_READY	0x01	/* ditto */

/* Modem Status Register contents, we don't care mostly */

/* 
 * The following four bits are marked complement in Loongson 2H manual, somehow
 * contradicts the description in the references.
 * Loongson 3A also defined the following bits as complements, but we don't
 * care anyway.
 */
#define UART_MSR_DCD		0x80	/* Data Carrier Detect */
#define UART_MSR_RI		0x40	/* Ring Indicator */
#define UART_MSR_DSR		0x20	/* Data Set Ready */
#define UART_MSR_CTS		0x10	/* Clear To Send */
/* Deltas */
#define UART_MSR_DDCD		0x08
#define UART_MSR_TERI		0x04	/* RI low-to-high transition */
#define UART_MSR_DDSR		0x02
#define UART_MSR_DCTS		0x01

#endif /* _DRIVERS_SERIAL_UART_NS16550_HW_H */

