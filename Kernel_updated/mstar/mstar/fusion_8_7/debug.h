/*
   (c) Copyright 2002-2011  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2002-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version
   2 of the License, or (at your option) any later version.
*/

#ifndef __FUSION__DEBUG_H__
#define __FUSION__DEBUG_H__

#include <linux/kernel.h>

#define D_ARRAY_SIZE(array)        ((int)(sizeof(array) / sizeof((array)[0])))

#define FUSION_ASSERT(exp)    do { if (!(exp)) { printk( KERN_ERR "linux-fusion: Assertion [ " #exp " ] failed! (%s:%d)\n", __FILE__, __LINE__ ); *(char*) 0 = 0; } } while (0)
#define FUSION_ASSUME(exp)    do { if (!(exp)) printk( KERN_ERR "linux-fusion: Assumption [ " #exp " ] failed! (%s:%d)\n", __FILE__, __LINE__ ); } while (0)

#ifdef FUSION_ENABLE_DEBUG
#define FUSION_DEBUG(x...)  fusion_debug_printk (x)
#else
#define FUSION_DEBUG(x...)  do {} while (0)
#endif

void fusion_debug_printk( const char *format, ... );


#define D_ASSERT FUSION_ASSERT
#define D_ASSUME FUSION_ASSUME


/*
 * Magic Assertions & Utilities
 */

#define D_MAGIC(spell)             ( (((spell)[sizeof(spell)*8/9] << 24) | \
                                      ((spell)[sizeof(spell)*7/9] << 16) | \
                                      ((spell)[sizeof(spell)*6/9] <<  8) | \
                                      ((spell)[sizeof(spell)*5/9]      )) ^  \
                                     (((spell)[sizeof(spell)*4/9] << 24) | \
                                      ((spell)[sizeof(spell)*3/9] << 16) | \
                                      ((spell)[sizeof(spell)*2/9] <<  8) | \
                                      ((spell)[sizeof(spell)*1/9]      )) )


#define D_MAGIC_CHECK(o,m)         ((o) != NULL && (o)->magic == D_MAGIC(#m))

#define D_MAGIC_SET(o,m)           do {                                              \
                                        D_ASSERT( (o) != NULL );                     \
                                        D_ASSUME( (o)->magic != D_MAGIC(#m) );       \
                                                                                     \
                                        (o)->magic = D_MAGIC(#m);                    \
                                   } while (0)

#define D_MAGIC_SET_ONLY(o,m)      do {                                              \
                                        D_ASSERT( (o) != NULL );                     \
                                                                                     \
                                        (o)->magic = D_MAGIC(#m);                    \
                                   } while (0)

#define D_MAGIC_ASSERT(o,m)        do {                                              \
                                        D_ASSERT( (o) != NULL );                     \
                                        D_ASSERT( (o)->magic == D_MAGIC(#m) );       \
                                   } while (0)

#define D_MAGIC_ASSUME(o,m)        do {                                              \
                                        D_ASSUME( (o) != NULL );                     \
                                        if (o)                                       \
                                             D_ASSUME( (o)->magic == D_MAGIC(#m) );  \
                                   } while (0)

#define D_MAGIC_ASSERT_IF(o,m)     do {                                              \
                                        if (o)                                       \
                                             D_ASSERT( (o)->magic == D_MAGIC(#m) );  \
                                   } while (0)

#define D_MAGIC_CLEAR(o)           do {                                              \
                                        D_ASSERT( (o) != NULL );                     \
                                        D_ASSUME( (o)->magic != 0 );                 \
                                                                                     \
                                        (o)->magic = 0;                              \
                                   } while (0)

#endif
