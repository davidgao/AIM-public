/* Copyright (C) xxxx Anonymous (from Linux source code)
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 * Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

#ifndef _LIST_H
#define _LIST_H

#include <util.h>

#ifndef __ASSEMBLER__

/*
 * Simple doubly-linked list implementation
 *
 * The first list node is a so-called sentry node; it should *not* be used
 * to store data.
 */

struct list_head {
	struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}
/* Use this rather than INIT_LIST_HEAD as this one looks more like a
 * regular function call. */
#define list_init(list)		INIT_LIST_HEAD(list)

/*
 * A shortcut for list initialization:
 * struct list_head list = EMPTY_LIST(list);
 */
#define LIST_HEAD_INIT(list)	{ &(list), &(list) }
#define EMPTY_LIST(list)	LIST_HEAD_INIT(list)

/*
 * A shortcut for empty list declaration from Linux:
 * LIST_HEAD(list);
 *
 * Not recommended as it looks like a function call.
 * If we want a declaration we should as well use the style above.
 * This macro exists only for compatibility from Linux source code.
 */
#define LIST_HEAD(name)		struct list_head name = EMPTY_LIST(name)

static inline void __list_add(struct list_head *new,
    struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/* (more friendly) aliases for the two functions above */
#define list_add_after(new, head)	list_add(new, head)
#define list_add_before(new, head)	list_add_tail(new, head)

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	/* TODO: do we need to add list poisons as in Linux? */
}

static inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

static inline int list_is_first(const struct list_head *list,
				const struct list_head *head)
{
	return list->prev == head;
}

static inline int list_is_last(const struct list_head *list,
				const struct list_head *head)
{
	return list->next == head;
}

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

/* Get the structure containing this list entry */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_next_entry(ptr, type, member) \
	list_entry((ptr)->member.next, type, member)
#define list_prev_entry(ptr, type, member) \
	list_entry((ptr)->member.prev, type, member)
/* A more convenient wrapper for list_next_entry() for lists with same type */
#define next_entry(ptr, member) \
	list_next_entry(ptr, typeof(*ptr), member)
#define prev_entry(ptr, member) \
	list_prev_entry(ptr, typeof(*ptr), member)

/*
 * For-each loops.
 */
#define for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)
#define for_each_reverse(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)
/* The two variants are designed for safety against removal */
#define for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
	     pos = n, n = pos->next)
#define for_each_reverse_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; pos != (head); \
	     pos = n, n = pos->prev)

#define for_each_entry(pos, head, member) \
	for (pos = list_entry((head)->next, typeof(*pos), member); \
	     &pos->member != (head); \
	     pos = list_entry(pos->member.next, typeof(*pos), member))
#define for_each_entry_reverse(pos, head, member) \
	for (pos = list_entry((head)->prev, typeof(*pos), member); \
	     &pos->member != (head); \
	     pos = list_entry(pos->member.prev, typeof(*pos), member))
/*
 * Again, for removal, so you can do something like:
 * for_each_entry_safe(pos, n, head, member)
 *     delete(pos);
 */
#define for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))
#define for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		n = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, typeof(*n), member))

#endif /* !__ASSEMBLER__ */

#endif /* _LIST_H */

