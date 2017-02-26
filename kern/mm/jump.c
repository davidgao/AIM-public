/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/panic.h>

#define JUMP_HANDLER_QUEUE_LENGTH	10
static int __jump_handler_queue_size;
static generic_fp __jump_handler_queue[JUMP_HANDLER_QUEUE_LENGTH];

void jump_handlers_clear(void)
{
	__jump_handler_queue_size = 0;
}

int jump_handlers_add(generic_fp entry)
{
	if (__jump_handler_queue_size > JUMP_HANDLER_QUEUE_LENGTH) {
		/* Bad data structure. Panic immediately to prevent damage. */
		panic("JUMP handler data structure invalid.\n");
	}
	if (__jump_handler_queue_size == JUMP_HANDLER_QUEUE_LENGTH) {
		/* Queue full */
		return EOF;
	}
	__jump_handler_queue[__jump_handler_queue_size] = entry;
	__jump_handler_queue_size += 1;
	return 0;
}

void jump_handlers_apply(void)
{
	for (int i = 0; i < __jump_handler_queue_size; ++i) {
		//(generic_fp)postmap_addr(__jump_handler_queue[i])();
	}
}

