/*
    GLIB - Library of useful routines for C programming
    Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
   (c) Copyright 2001-2009  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi>,
              Claudio Ciccani <klan@users.sf.net> and
              Michael Emmel <memmel@gmail.com>.
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __FUSION_HASH_H__
#define __FUSION_HASH_H__

#include <stdbool.h>

#include "types.h"


#define FUSION_HASH_MIN_SIZE 11
#define FUSION_HASH_MAX_SIZE 13845163

typedef enum {
     FHT_PTR,
     FHT_STRING,
     FHT_INT
} FusionHashType;

typedef struct _FusionHashNode  FusionHashNode;

struct _FusionHashNode
{
    void      *key;
    void      *value;
    FusionHashNode *next;
};

struct __Fusion_FusionHash
{
    int       magic;
    FusionHashType key_type;
    FusionHashType value_type;
    int             size;
    int             nnodes;
    FusionHashNode      **nodes;

    bool  free_keys;
    bool  free_values;
};

typedef bool (*FusionHashIteratorFunc)( FusionHash *hash,
                                        void       *key,
                                        void       *value,
                                        void       *ctx );


int
fusion_hash_resize (FusionHash  *hash);

int
fusion_hash_create (FusionHashType key_type,
                    FusionHashType value_type,
                    int  size, FusionHash **ret_hash );

int
fusion_hash_remove (FusionHash    *hash,
                    const void *  key,
                    void **old_key,
                    void **old_value);

int
fusion_hash_insert( FusionHash *hash, void  *key, void  *value );

int
fusion_hash_replace (FusionHash *hash,
                     void *   key, 
                     void *   value,
                     void **old_key,
                     void **old_value);
void
fusion_hash_destroy( FusionHash *hash );

void
fusion_hash_set_autofree( FusionHash *hash, bool free_keys, bool free_values );

void *
fusion_hash_lookup (FusionHash *hash, const void * key);

void
fusion_hash_iterate( FusionHash             *hash,
                     FusionHashIteratorFunc  func,
                     void                   *ctx );

unsigned int
fusion_hash_size (FusionHash *hash);

bool fusion_hash_should_resize ( FusionHash    *hash);




typedef struct {
     FusionHash     *hash;
     int             index;
     FusionHashNode *next;
} FusionHashIterator;

static inline void *
fusion_hash_iterator_next( FusionHashIterator *iterator )
{
     FusionHashNode *node = NULL;

     if (iterator->next) {
          node           = iterator->next;
          iterator->next = node->next;
     }
     else {
          FusionHash *hash = iterator->hash;

          D_MAGIC_ASSERT( hash, FusionHash );

          for (iterator->index++; iterator->index < hash->size; iterator->index++) {
               node = hash->nodes[iterator->index];
               if (node) {
                    iterator->next = node->next;

                    break;
               }
          }
     }

     return node ? node->value : NULL;
}

static inline void *
fusion_hash_iterator_init( FusionHashIterator *iterator,
                           FusionHash         *hash )
{
     D_MAGIC_ASSERT( hash, FusionHash );

     iterator->hash  = hash;
     iterator->index = -1;
     iterator->next  = NULL;

     return fusion_hash_iterator_next( iterator );
}


#define fusion_hash_foreach( elem, iterator, hash )                                       \
     for ((elem) = (__typeof__(elem)) fusion_hash_iterator_init( &iterator, hash );       \
          (elem) != NULL;                                                                 \
          (elem) = (__typeof__(elem)) fusion_hash_iterator_next( &iterator ))


#endif /*__FUSION_HASH_H__*/

