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

#include <linux/types.h>

#include "list.h"

void fusion_list_prepend(FusionLink ** list, FusionLink * link)
{
	link->prev = NULL;
	link->next = *list;

	if (*list)
		(*list)->prev = link;

	*list = link;
}

void fusion_list_remove(FusionLink ** list, FusionLink * link)
{
	if (link->prev)
		link->prev->next = link->next;
	else
		*list = link->next;

	if (link->next)
		link->next->prev = link->prev;

	link->next = link->prev = NULL;
}

void fusion_list_move_to_front(FusionLink ** list, FusionLink * link)
{
	if (*list == link)
		return;

	link->prev->next = link->next;

	if (link->next)
		link->next->prev = link->prev;

	link->prev = NULL;
	link->next = *list;

	(*list)->prev = link;

	*list = link;
}
