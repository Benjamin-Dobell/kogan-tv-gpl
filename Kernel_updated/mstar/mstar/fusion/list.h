/*
 *	Fusion Kernel Module
 *
 *	(c) Copyright 2002-2003  Convergence GmbH
 *
 *      Written by Denis Oliver Kropp <dok@directfb.org>
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef __FUSION__LIST_H__
#define __FUSION__LIST_H__

typedef struct _FusionLink {
	struct _FusionLink *next;
	struct _FusionLink *prev;
} FusionLink;

void fusion_list_prepend(FusionLink ** list, FusionLink * link);
void fusion_list_remove(FusionLink ** list, FusionLink * link);
void fusion_list_move_to_front(FusionLink ** list, FusionLink * link);

#define fusion_list_foreach(elem, list)                     \
     for (elem = (void*)(list);                             \
          elem;                                             \
          elem = (void*)(((FusionLink*)(elem))->next))

#define fusion_list_foreach_safe(elem, temp, list)                                             \
     for (elem = (void*)(list), temp = ((elem) ? (void*)(((FusionLink*)(elem))->next) : NULL); \
          elem;                                                                                \
          elem = (void*)(temp), temp = ((elem) ? (void*)(((FusionLink*)(elem))->next) : NULL))

#endif /* __FUSION__LIST_H__ */
