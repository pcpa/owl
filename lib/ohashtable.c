/*
 * Copyright (C) 2014  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#include "owl.h"

#define hash_lock()		omutex_lock(&hash_mutex)
#define hash_unlock()		omutex_unlock(&hash_mutex)

/*
 * Prototypes
 */
static inline oword_t
key_hash(ouint8_t *vector, oword_t key, oword_t length);

static void
new_hashtable(oobject_t *pointer, oword_t size);

static oword_t
mpz_hash(ompz_t z);

static inline oword_t
mpq_hash(ompq_t q);

static oword_t
mpr_hash(ompr_t r);

static inline oword_t
cqq_hash(cqq_t qq);

static inline oword_t
mpc_hash(ompc_t rr);

static oword_t
hashentry_key(ohashentry_t *entry);

static inline obool_t
mpr_compare(ompr_t a, ompr_t b);

static obool_t
hashentry_compare(ohashentry_t *left, ohashentry_t *right);

static void
rehashtable(ohashtable_t *hash);

static void
native_hash(oobject_t list, oint32_t ac);

/*
 * Initialization
 */
static pthread_mutex_t		 hash_mutex;
static ovector_t		*rehash_vector;

/*
 * Implementation
 */
void
init_hashtable(void)
{
    obuiltin_t		*builtin;

    omutex_init(&hash_mutex);
    oadd_root((oobject_t *)&rehash_vector);
    builtin = onew_builtin("hash", native_hash, t_void, false);
    oend_builtin(builtin);
}

void
finish_hashtable(void)
{
    orem_root((oobject_t *)&rehash_vector);
}

static inline oword_t
key_hash(ouint8_t *vector, oword_t key, oword_t length)
{
    oword_t		 i;
    ouint8_t		*ptr;

    for (i = 0, ptr = vector; i < length; i++)
	key = ((key << 5) + key) + ptr[i];

    return (key);
}

void
okey_hashentry(ohashentry_t *entry)
{
    entry->key = hashentry_key(entry);
}

void
oput_hashentry(ohashtable_t *hash, ohashentry_t *entry)
{
    oword_t		 key;
    ohashentry_t	*prev, *ptr;

    key = entry->key & (hash->size - 1);
    prev = ptr = hash->entries[key];
    for (; ptr; prev = ptr, ptr = ptr->next) {
	if (hashentry_compare(entry, ptr)) {
	    /* replace existing entry */
	    if (ptr == prev)
		hash->entries[key] = entry;
	    else
		prev->next = entry;
	    entry->next = ptr->next;
	    return;
	}
    }

    /* add new entry */
    if (prev)
	prev->next = entry;
    else
	hash->entries[key] = entry;

    ++hash->count;

    if (hash->count > (hash->size >> 1) + (hash->size >> 2))
	rehashtable(hash);
}

ohashentry_t *
oget_hashentry(ohashtable_t *hash, ohashentry_t *entry)
{
    oword_t		 key;
    ohashentry_t	*ptr;

    key = entry->key & (hash->size - 1);
    for (ptr = hash->entries[key]; ptr; ptr = ptr->next)
	if (hashentry_compare(entry, ptr)) {
	    return (ptr);
	}

    return (null);
}

void
orem_hashentry(ohashtable_t *hash, ohashentry_t *entry)
{
    oword_t		 key;
    ohashentry_t	*ptr, *prev;

    key = entry->key & (hash->size - 1);
    for (ptr = prev = hash->entries[key]; ptr; prev = ptr, ptr = ptr->next) {
	if (ptr == entry) {
	    --hash->count;
	    if (ptr == prev)
		hash->entries[key] = ptr->next;
	    else
		prev->next = ptr->next;
	    break;
	}
    }
}

static void
new_hashtable(oobject_t *pointer, oword_t size)
{
    oword_t		 i;

    /* limit initial size to 1 << 16 */
    if (size <= 0)
	size = 16;
    else if (size > (1 << 16))
	size = 1 << 16;
    else if (size & (size - 1)) {
	for (i = 1 << 15; i >= 0; i >>= 1)
	    if (size & i)
		break;
	size = i << 1;
    }

    onew_vector_base(pointer, t_void, t_hashtable, size, sizeof(ohashtable_t));
}

static oword_t
mpz_hash(ompz_t z)
{
    oword_t		key;
    oword_t		length;

    if (z->_mp_size < 0) {
	key = -1;
	length = -z->_mp_size;
    }
    else {
	key = 0;
	length = z->_mp_size;
    }

    return (key_hash((ouint8_t *)z->_mp_d, key, length / sizeof(oword_t)));
}

static inline oword_t
mpq_hash(ompq_t q)
{
    return (mpz_hash(mpq_numref(q)) ^ mpz_hash(mpq_denref(q)));
}

static oword_t
mpr_hash(ompr_t r)
{
    oword_t		key;

    if (!mpfr_number_p(r) || mpfr_zero_p(r))	return (0);
    if (r->_mpfr_sign < 0)			key = -1;
    else					key = 0;

    key = key_hash((ouint8_t *)r->_mpfr_d, key,
		   (r->_mpfr_prec - 1) / sizeof(oword_t));

    return ((key ^ r->_mpfr_exp) & 0x7fffffff);
}

static inline oword_t
cqq_hash(ocqq_t qq)
{
    return (mpq_hash(cqq_realref(qq)) ^ mpq_hash(cqq_imagref(qq)));
}

static inline oword_t
mpc_hash(ompc_t rr)
{
    return (mpr_hash(mpc_realref(rr)) ^ mpr_hash(mpc_imagref(rr)));
}

static oword_t
hashentry_key(ohashentry_t *entry)
{
    switch (entry->nt) {
	case t_word:
	    return (key_hash((ouint8_t *)&entry->nv.w, 0, sizeof(oword_t)));
	case t_float:
	    return (key_hash((ouint8_t *)&entry->nv.d, 0, sizeof(ofloat_t)));
	case t_mpz:
	    return (mpz_hash(entry->nv.z));
	case t_rat:
	    return (key_hash((ouint8_t *)&entry->nv.x, 0, sizeof(rat_t)));
	case t_mpq:
	    return (mpq_hash(entry->nv.q));
	case t_mpr:
	    return (mpr_hash(entry->nv.r));
	case t_cdd:
	    return (key_hash((ouint8_t *)&entry->nv.dd, 0, sizeof(ocdd_t)));
	case t_cqq:
	    return (cqq_hash(entry->nv.qq));
	case t_mpc:
	    return (mpc_hash(entry->nv.cc));
	case t_string:
	    return (key_hash(entry->nv.v->v.u8, 0, entry->nv.v->length));
	default:
	    return (key_hash((ouint8_t *)&entry->nv.o, 0, sizeof(oobject_t)));
    }
}

static inline obool_t
mpr_compare(ompr_t a, ompr_t b)
{
    if (mpfr_signbit(a) == mpfr_signbit(b)) {
	if (likely(mpfr_number_p(a))) {
	    if (likely(mpfr_number_p(b)))
		return (mpfr_lessgreater_p(a, b) == 0);
	    return (false);
	}
	if (likely(!mpfr_number_p(b))) {
	    if (mpfr_nan_p(a))
		return (mpfr_nan_p(b));
	    return (mpfr_inf_p(b));
	}
    }
    return (false);	
}

static obool_t
hashentry_compare(ohashentry_t *l, ohashentry_t *r)
{
    if (l->nt != r->nt)
	return (false);
    switch (l->nt) {
	case t_word:
	    return (l->nv.w == r->nv.w);
	case t_float:
	    return (l->nv.d == r->nv.d);
	case t_mpz:
	    return (mpz_cmp(l->nv.z, r->nv.z) == 0);
	case t_rat:
	    return (orat_cmp(&l->nv.x, &r->nv.x) == 0);
	case t_mpq:
	    return (mpq_cmp(l->nv.q, r->nv.q) == 0);
	case t_mpr:
	    return (mpr_compare(l->nv.r, r->nv.r));
	case t_cdd:
	    return (l->nv.ii.re == r->nv.ii.re && l->nv.ii.im == r->nv.ii.im);
	case t_cqq:
	    return (mpq_cmp(cqq_realref(l->nv.qq), cqq_realref(r->nv.qq)) == 0 &&
		    mpq_cmp(cqq_imagref(l->nv.qq), cqq_imagref(r->nv.qq)) == 0);
	case t_mpc:
	    return (mpr_compare(mpc_realref(l->nv.cc), mpc_realref(r->nv.cc)) &&
		    mpr_compare(mpc_imagref(l->nv.cc), mpc_imagref(r->nv.cc)));
	case t_string:
	    return (l->nv.v->length == r->nv.v->length &&
		    memcmp(l->nv.v->v.obj, r->nv.v->v.obj, l->nv.v->length) == 0);
	default:
	    return (l->nv.o == r->nv.o);
    }
}

static void
rehashtable(ohashtable_t *hash)
{
    oword_t		 i, j, key;
    ohashentry_t	*entry, **entries;

    if (hash->size << 1 < hash->size)
	return;

    hash_lock();

    if (rehash_vector == null)
	onew_vector((oobject_t *)&rehash_vector, t_void, hash->count);
    else if (rehash_vector->length <= hash->count)
	orenew_vector(rehash_vector, hash->count);

    entries = (ohashentry_t **)rehash_vector->v.obj;
    for (i = j = 0; i < hash->size; i++)
	for (entry = hash->entries[i]; entry; entry = entry->next)
	    entries[j++] = entry;

    orenew_vector((ovector_t *)hash, hash->size << 1);
    memset(hash->entries, 0, sizeof(oentry_t *) * hash->size >> 1);

    for (--j; j >= 0; --j) {
	entry = entries[j];
	key = entry->key & (hash->size - 1);
	entry->next = hash->entries[key];
	hash->entries[key] = entry;
    }

    /* gc protected vector no longer required */
    memset(rehash_vector->v.ptr, 0, sizeof(oobject_t) * hash->count);
    hash_unlock();
}

static void
native_hash(oobject_t list, oint32_t ac)
{
    GET_THREAD_SELF()

    new_hashtable(&thread_self->obj, 16);
    thread_self->r0.v.o = thread_self->obj;
    thread_self->r0.t = t_hashtable;
}
