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
              Michael Emmel <mike.emmel@gmail.com>.
 
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

#include <linux/slab.h>
#include <linux/string.h>

#include <asm/errno.h>

#include "debug.h"
#include "hash.h"


//D_DEBUG_DOMAIN( Fusion_Hash, "Fusion/Hash", "Hash table implementation" );

#define HASH_STR(h,p) \
{\
    h = *p;\
    if (h)\
        for (p += 1; *p != '\0'; p++)\
            h = (h << 5) - h + *p;\
}\

static const unsigned int primes[] =
{
     11,
     19,
     37,
     73,
     109,
     163,
     251,
     367,
     557,
     823,
     1237,
     1861,
     2777,
     4177,
     6247,
     9371,
     14057,
     21089,
     31627,
     47431,
     71143,
     106721,
     160073,
     240101,
     360163,
     540217,
     810343,
     1215497,
     1823231,
     2734867,
     4102283,
     6153409,
     9230113,
     13845163,
};


static const unsigned int nprimes = D_ARRAY_SIZE( primes );

static void
fusion_hash_node_destroy (FusionHash *hash,FusionHashNode *node,
                          void **old_key,void **old_value);

static unsigned int
spaced_primes_closest (unsigned int num)
{
     unsigned int i;
     for (i = 0; i < nprimes; i++)
          if (primes[i] > num)
               return primes[i];
     return primes[nprimes - 1];
}

static __inline__ FusionHashNode**
fusion_hash_lookup_node (FusionHash *hash,
                         const void *key)
{
     FusionHashNode **node;

     /*TODO We could also optimize pointer hashing*/
     if (hash->key_type == FHT_STRING )
     {
          unsigned int h;
          const signed char *p = key;
          HASH_STR(h,p)
          node = &hash->nodes[h % hash->size];
     }
     else
          node = &hash->nodes[((unsigned long)key) % hash->size];

     /* Hash table lookup needs to be fast.
     *  We therefore remove the extra conditional of testing
     *  whether to call the key_equal_func or not from
     *  the inner loop.
     */
     if (hash->key_type == FHT_STRING ) {
          while(*node && strcmp((const char *)(*node)->key,(const char*)key))
               node = &(*node)->next;
     } else
          while (*node && (*node)->key != key)
               node = &(*node)->next;

     return node;

}

/**
 * fusion_hash_create_local:
 * @key_type: Type of hash key the hash is optimized for strings ints and pointers 
 * @value_type: Type of hash data optimized for strings ints and pointers 
 * @size: Inital size of the hash table 
 * @ret_hash:the new hash table
 * Creates a new #FusionHash that uses local memory
 * 
 * Return value: a new #FusionHash.
 **/
int
fusion_hash_create (FusionHashType key_type, FusionHashType value_type,
                          int  size, FusionHash **ret_hash )
{
     FusionHash *hash;

     if (!ret_hash)
          return -EINVAL;

     if (size < FUSION_HASH_MIN_SIZE)
          size = FUSION_HASH_MIN_SIZE;

     hash = kzalloc( sizeof (FusionHash), GFP_KERNEL );
     if (!hash)
          return -ENOMEM;

     hash->key_type           = key_type;
     hash->value_type         = value_type;
     hash->size               = size;
     hash->nnodes             = 0;
     hash->nodes = kzalloc(size * sizeof (FusionHashNode*), GFP_KERNEL );

     if (!hash->nodes) {
          kfree( hash );
          return -ENOMEM;
     }

     D_MAGIC_SET(hash, FusionHash );

     *ret_hash = hash;

     return 0;
}

void
fusion_hash_destroy( FusionHash *hash )
{
     int i;
     FusionHashNode *node, *next;
     D_MAGIC_ASSERT( hash, FusionHash );

     for (i = 0; i < hash->size; i++) {
          for (node = hash->nodes[i]; node; node = next) {
               next = node->next;
               fusion_hash_node_destroy(hash, node, NULL, NULL);
          }
     }
     kfree(hash->nodes);
     D_MAGIC_CLEAR( hash );
     kfree(hash);
}

void
fusion_hash_set_autofree( FusionHash *hash, bool free_keys, bool free_values )
{
     D_MAGIC_ASSERT( hash, FusionHash );

     hash->free_keys   = free_keys;
     hash->free_values = free_values;
}

/**
 * fusion_hash_lookup:
 * @hash: a #FusionHash.
 * @key: the key to look up.
 * 
 * Looks up a key in a #FusionHash. Note that this function cannot
 * distinguish between a key that is not present and one which is present
 * and has the value %NULL. If you need this distinction, use
 * hash_lookup_extended().
 * 
 * Return value: the associated value, or %NULL if the key is not found.
 **/
void *
fusion_hash_lookup (FusionHash *hash, const void * key)
{
     FusionHashNode *node;
     D_MAGIC_ASSERT( hash, FusionHash );
     node = *fusion_hash_lookup_node (hash, key);
     return node ? node->value : NULL;
}

/**
 * fusion_hash_insert:
 * @hash: a #FusionHash.
 * @key: a key to insert.
 * @value: the value to associate with the key.
 * 
 * Inserts a new key and value into a #FusionHash.
 * If the key already exists in the #FusionHash DR_BUG is returned 
 * If you think a key may exist you should call fusion_hash_replace
 * Generally this is only used on a new FusionHash
 **/
int
fusion_hash_insert( FusionHash *hash,
                    void       *key,
                    void       *value )
{
     FusionHashNode **node;
     D_MAGIC_ASSERT( hash, FusionHash );

     node = fusion_hash_lookup_node (hash, key);

     if (*node) {
          printk( KERN_DEBUG "fusion_hash: key already exists\n" );
          return -EINVAL;
     }
     else {
          (*node) = kzalloc(sizeof(FusionHashNode), GFP_KERNEL);
          if ( !(*node) )
               return -ENOMEM;

          (*node)->key = key;
          (*node)->value = value;
          hash->nnodes++;
          if ( fusion_hash_should_resize(hash) )
               fusion_hash_resize(hash);
     }
     return 0;
}

/**
 * hash_replace:
 * @hash: a #FusionHash.
 * @key: a key to insert.
 * @value: the value to associate with the key.
 * 
 * Inserts a new key and value into a #FusionHash similar to 
 * hash_insert(). The difference is that if the key already exists 
 * in the #FusionHash, it gets replaced by the new key. 
 * If you supplied a  oldkey pointer or oldkey value they are returned
 * otherwise free is called the key if table type is not type FHT_INT
 * and free is called on the old value if not supplied
 **/
int
fusion_hash_replace (FusionHash *hash,
                     void *   key, 
                     void *   value,
                     void **old_key,
                     void **old_value)
{
     FusionHashNode **node;
     D_MAGIC_ASSERT( hash, FusionHash );

     node = fusion_hash_lookup_node (hash, key);

     if (*node) {
          if ( old_key)
               *old_key = (*node)->key;
          else if ( hash->key_type != FHT_INT ) {
               if (hash->free_keys) {
                    kfree((*node)->key);
               }
          }

          if ( old_value)
               *old_value = (*node)->value;
          else if ( hash->value_type != FHT_INT ) {
               if (hash->free_values) {
                    kfree((*node)->value);
               }
          }
     }
     else {
          *node = kzalloc( sizeof(FusionHashNode), GFP_KERNEL);

          if ( !(*node) )
               return -ENOMEM;

          hash->nnodes++;
     }
     (*node)->key = (void*)key;
     (*node)->value = (void*)value;

     return 0;
}

/**
 * fusion_hash_remove:
 * @hash: a #FusionHash.
 * @key: the key to remove.
 * @old_key: returned old_key 
 * @old_value: returned old_value 
 * Removes a key and its associated value from a #FusionHash.
 *
 * If the #FusionHash was created using hash_new_full(), the
 * key and value are freed using the supplied destroy functions, otherwise
 * you have to make sure that any dynamically allocated values are freed 
 * yourself.
 * If you supplied a  oldkey pointer or oldkey value they are returned
 * otherwise free is called the key if table type is not type FHT_INT
 * and free is called on the old value if not supplied
 * 
 **/
int
fusion_hash_remove (FusionHash    *hash,
                    const void *  key,
                    void **old_key,
                    void **old_value)
{
     FusionHashNode **node, *dest;
     D_MAGIC_ASSERT( hash, FusionHash );

     node = fusion_hash_lookup_node (hash, key);
     if (*node) {
          dest = *node;
          (*node) = dest->next;
          fusion_hash_node_destroy(hash, dest, old_key, old_value);
          hash->nnodes--;
          return 0;
     }
     return 0;
}

/**
 * hash_foreach:
 * @hash: a #FusionHash.
 * @func: the function to call for each key/value pair.
 * @user_data: user data to pass to the function.
 * 
 * Calls the given function for each of the key/value pairs in the
 * #FusionHash.  The function is passed the key and value of each
 * pair, and the given @user_data parameter.  The hash table may not
 * be modified while iterating over it (you can't add/remove
 * items). To remove all items matching a predicate, use
 * hash_foreach_remove().
 **/
void
fusion_hash_iterate( FusionHash             *hash,
                     FusionHashIteratorFunc  func,
                     void                   *ctx )
{
     int i;
     FusionHashNode *node;
     FusionHashNode *next;

     D_MAGIC_ASSERT( hash, FusionHash );

     for (i = 0; i < hash->size; i++) {
          for (node = hash->nodes[i]; node; node = next) {
               next = node->next;

               if ( func(hash, node->key, node->value, ctx))
                    return;
          }
     }
}

/**
 * hash_size:
 * @hash: a #FusionHash.
 * 
 * Returns the number of elements contained in the #FusionHash.
 * 
 * Return value: the number of key/value pairs in the #FusionHash.
 **/
unsigned int
fusion_hash_size (FusionHash *hash)
{
     D_MAGIC_ASSERT( hash, FusionHash );
     return hash->nnodes;
}

/**
 * fusion_hash_should_resize:
 * Call the function after adding or removing several
 * values it has a decent heurisitc to determine if
 * the hash has grown to large
 */
bool fusion_hash_should_resize ( FusionHash    *hash)
{
     D_MAGIC_ASSERT( hash, FusionHash );
     if ((hash->size >= 3 * hash->nnodes &&
          hash->size > FUSION_HASH_MIN_SIZE) ||
         (3 * hash->size <= hash->nnodes &&
          hash->size < FUSION_HASH_MAX_SIZE))
          return true;
     return false;
}

/* Hash Functions
 * Resize the hash to minumim for this number of entries
 */
int
fusion_hash_resize (FusionHash *hash)
{
     FusionHashNode **new_nodes;
     FusionHashNode *node;
     FusionHashNode *next;
     unsigned int hash_val;
     int new_size;
     int i;
     D_MAGIC_ASSERT( hash, FusionHash );

     new_size = spaced_primes_closest (hash->nnodes);
     if (new_size > FUSION_HASH_MAX_SIZE )
          new_size = FUSION_HASH_MAX_SIZE;
     if (new_size <  FUSION_HASH_MIN_SIZE)
          new_size = FUSION_HASH_MIN_SIZE;

     new_nodes = kzalloc (new_size * sizeof(FusionHashNode*), GFP_KERNEL);
     if (!new_nodes)
          return -ENOMEM;

     for (i = 0; i < hash->size; i++)
          for (node = hash->nodes[i]; node; node = next) {
               next = node->next;
               /*TODO We could also optimize pointer hashing*/
               if (hash->key_type == FHT_STRING ) {
                    unsigned int h;
                    const signed char *p = node->key;
                    HASH_STR(h, p)
                    hash_val = h % new_size;
               }
               else
                    hash_val = ((unsigned long)node->key) % new_size;

               node->next = new_nodes[hash_val];
               new_nodes[hash_val] = node;
          }
     kfree(hash->nodes);
     hash->nodes = new_nodes;
     hash->size = new_size;
     return 0;
}


static void
fusion_hash_node_destroy (FusionHash *hash,FusionHashNode *node,
                          void **old_key,void **old_value)
{
     if (!node )
          return;

     if ( old_key)
          *old_key = node->key;
     else if ( hash->key_type != FHT_INT ) {
          if (hash->free_keys) {
               kfree(node->key );
          }
     }

     if ( old_value)
          *old_value = node->value;
     else if ( hash->value_type != FHT_INT ) {
          if (hash->free_values) {
               kfree(node->value );
          }
     }

     kfree(node);
}

