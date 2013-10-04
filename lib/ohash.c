/*
 * Copyright (C) 2008-2012  Paulo Cesar Pereira de Andrade.
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

#define pwp(v)			(*(oword_t *)v)
#define pdp(v)			(*(ofloat_t *)v)
#define rat(v)			((orat_t)v)
#define mpz(v)			((ompz_t)v)
#define mpq(v)			((ompq_t)v)
#define mpr(v)			((ompr_t)v)
#define pddp(v)			(*(ocdd_t *)v)
#define cqq(v)			((ocqq_t)v)
#define mpc(v)			((ompc_t)v)

/*
 * Prototypes
 */
static oword_t
hash_str(oobject_t name, oword_t length);

static oword_t
hash_ptr(oobject_t name);

static oword_t
hash_mpz(ompz_t z);

static oword_t
hash_mpq(ompq_t q);

static oword_t
hash_mpr(ompr_t r);

static oword_t
hash_cqq(ocqq_t qq);

static oword_t
hash_mpc(ompc_t rr);

static oword_t
hash_obj(oobject_t name);

static obool_t
cmp_str(oobject_t a, oobject_t b, oword_t length);

static obool_t
cmp_mpr(ompr_t a, ompr_t b);

static obool_t
cmp_cqq(ocqq_t a, ocqq_t b);

static obool_t
cmp_obj(oobject_t lobj, oobject_t robj, otype_t ltype);

static obool_t
cmp_mpc(ompc_t a, ompc_t b);

static void
rehash(ohash_t *hash);

/*
 * Initialization
 */
static ovector_t	*rehash_vector;

/*
 * Implementation
 */
void
init_hash(void)
{
    oadd_root((oobject_t *)&rehash_vector);
}

void
finish_hash(void)
{
    orem_root((oobject_t *)&rehash_vector);
}

void
onew_hash(oobject_t *pointer, oword_t size)
{
    oword_t		length;

    assert(size > 0);
    for (length = 1; size; length <<= 1)
	size >>= 1;
    onew_vector_base(pointer, t_void, t_hash, length, sizeof(ohash_t));
}

void
onew_entry_base(oobject_t *pointer, oobject_t name, oobject_t value,
		otype_t type, oword_t length)
{
    oentry_t		*entry;

    onew_object(pointer, type, length);
    entry = *pointer;
    entry->name = name;
    entry->value = value;
}

void
oput_hash(ohash_t *hash, oentry_t *entry)
{
    oword_t		 key;
    otype_t		 type;
    oentry_t		*prev, *ptr;

    type = otype(entry->name);
    entry->key = hash_obj(entry->name);
    key = entry->key & (hash->size - 1);
    prev = ptr = hash->entries[key];
    for (; ptr; prev = ptr, ptr = ptr->next) {
	if (unlikely(cmp_obj(entry->name, ptr->name, type))) {
	    /*   Replace existing entry. */
	    if (ptr == prev)
		hash->entries[key] = entry;
	    else
		prev->next = entry;
	    entry->next = ptr->next;
	    return;
	}
    }

    /*   Add new entry. */
    entry->next = hash->entries[key];
    hash->entries[key] = entry;
    ++hash->count;

    if (hash->count > (hash->size >> 1) + (hash->size >> 2))
	rehash(hash);
}

oentry_t *
oget_hash(ohash_t *hash, oobject_t name)
{
    oword_t		 key;
    otype_t		 type;
    oentry_t		*entry;

    if (name) {
	type = otype(name);
	key = hash_obj(name) & (hash->size - 1);
	for (entry = hash->entries[key]; entry; entry = entry->next) {
	    if (cmp_obj(name, entry->name, type))
		return (entry);
	}
    }

    return (null);
}

oentry_t *
oget_hash_string(ohash_t *hash, ouint8_t *name, oint32_t length)
{
    oword_t		 key;
    oentry_t		*entry;
    ovector_t		*vector;

    key = hash_str(name, length) & (hash->size - 1);
    for (entry = hash->entries[key]; entry; entry = entry->next) {
	if (otype(entry->name) == t_vector_uint8) {
	    vector = entry->name;
	    if (length == vector->length &&
		memcmp(name, vector->v.obj, length) == 0)
		return (entry);
	}
    }

    return (null);
}

void
orem_hash(ohash_t *hash, oentry_t *entry)
{
    oword_t		 key;
    oentry_t		*ptr, *prev;

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

void
oclr_hash(ohash_t *hash)
{
    if (hash->count) {
	hash->count = 0;
	/* let gc collect it */
	memset(hash->entries, 0, sizeof(oobject_t) * hash->size);
    }
}

static oword_t
hash_str(oobject_t name, oword_t length)
{
    oword_t		 i, key;
    union {
	ouint8_t	*c;
	oword_t		*w;
	oobject_t	 v;
    } ptr;

    for (i = 0, key = 0, ptr.v = name; i < length / sizeof(oword_t); i++)
	key = (key << (key & 1)) ^ ptr.w[i];
    for (i *= sizeof(oword_t); i < length; i++)
	key = (key << (key & 1)) ^ ptr.c[i];

    return (key);
}

static oword_t
hash_ptr(oobject_t name)
{
    union {
	oword_t		w;
	oobject_t	v;
    } ptr;

    ptr.v = name;

    return (ptr.w >> 3);
}

static oword_t
hash_mpz(ompz_t z)
{
    if (z->_mp_size < 0)
	return (~hash_str(z->_mp_d, -z->_mp_size * sizeof(oword_t)));
    return (hash_str(z->_mp_d, z->_mp_size * sizeof(oword_t)));
}

static oword_t
hash_mpq(ompq_t q)
{
    return (hash_mpz(mpq_numref(q)) ^ hash_mpz(mpq_denref(q)));
}

static oword_t
hash_mpr(ompr_t r)
{
    if (!mpfr_number_p(r) || mpfr_zero_p(r))
	return (0);

    return (hash_str(r->_mpfr_d, r->_mpfr_prec / __WORDSIZE) ^ r->_mpfr_exp);
}

static oword_t
hash_cqq(ocqq_t qq)
{
    return (hash_mpq(cqq_realref(qq)) ^ hash_mpq(cqq_imagref(qq)));
}

static oword_t
hash_mpc(ompc_t rr)
{
    return (hash_mpr(mpc_realref(rr)) ^ hash_mpr(mpc_imagref(rr)));
}

static oword_t
hash_obj(oobject_t name)
{
    ovector_t		*vector;

    switch (otype(name)) {
	case t_word:		return (hash_str(name, sizeof(oword_t)));
	case t_float:		return (hash_str(name, sizeof(ofloat_t)));
	case t_rat:		return (hash_str(name, sizeof(rat_t)));
	case t_mpz:		return (hash_mpz(mpz(name)));
	case t_mpq:		return (hash_mpq(mpq(name)));
	case t_mpr:		return (hash_mpr(mpr(name)));
	case t_cdd:		return (hash_str(name, sizeof(ocdd_t)));
	case t_cqq:		return (hash_cqq(cqq(name)));
	case t_mpc:		return (hash_mpc(mpc(name)));
	case t_vector_int8:	case t_vector_uint8:
	    vector = name;
	    return (hash_str(vector->v.obj, vector->length));
	default:
	    return (hash_ptr(name));
    }

    return (0);
}

static obool_t
cmp_str(oobject_t a, oobject_t b, oword_t length)
{
    return (memcmp(a, b, length) == 0);
}

static obool_t
cmp_mpr(ompr_t a, ompr_t b)
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
cmp_cqq(ocqq_t a, ocqq_t b)
{
    return (mpq_cmp(cqq_realref(a), cqq_realref(b)) == 0 &&
	    mpq_cmp(cqq_imagref(a), cqq_imagref(b)) == 0);
}

static obool_t
cmp_mpc(ompc_t a, ompc_t b)
{
    return (cmp_mpr(mpc_realref(a), mpc_realref(b)) &&
	    cmp_mpr(mpc_imagref(a), mpc_imagref(b)));
}

static obool_t
cmp_obj(oobject_t lobj, oobject_t robj, otype_t ltype)
{
    ovector_t		*lvec, *rvec;

    if (lobj == robj)		return (true);
    if (ltype != otype(robj))	return (false);
    switch (ltype) {
	case t_word:		return (pwp(lobj) == pwp(robj));
	case t_float:		return (cmp_str(lobj, robj, sizeof(ofloat_t)));
	case t_rat:		return (cmp_str(lobj, robj, sizeof(rat_t)));
	case t_mpz:		return (mpz_cmp(mpz(lobj), mpz(robj)) == 0);
	case t_mpq:		return (mpq_cmp(mpq(lobj), mpq(robj)) == 0);
	case t_mpr:		return (cmp_mpr(mpr(lobj), mpr(robj)));
	case t_cdd:		return (cmp_str(lobj, robj, sizeof(ocdd_t)));
	case t_cqq:		return (cmp_cqq(cqq(lobj), cqq(robj)));
	case t_mpc:		return (cmp_mpc(mpc(lobj), mpc(robj)));
	case t_vector_int8:	case t_vector_uint8:
	    lvec = lobj;
	    rvec = robj;
	    return (lvec->length == rvec->length &&
		    memcmp(lvec->v.obj, rvec->v.obj, lvec->length) == 0);
	default:
	    return (false);
    }
}

static void
rehash(ohash_t *hash)
{
    oentry_t		*entry;
    oword_t		 i, j, key;

    if ((hash->size << 1) < hash->size)
	return;

    if (rehash_vector == null)
	onew_vector((oobject_t *)&rehash_vector, t_void, hash->count);
    else if (rehash_vector->length <= hash->count)
	orenew_vector(rehash_vector, hash->count);

    for (i = j = 0; i < hash->size; i++) {
	for (entry = hash->entries[i]; entry; entry = entry->next)
	    rehash_vector->v.ptr[j++] = entry;
    }

    orenew_vector((ovector_t *)hash, hash->size << 1);
    memset(hash->entries, 0, sizeof(oentry_t *) * hash->size);

    for (--j; j >= 0; --j) {
	entry = rehash_vector->v.ptr[j];
	key = entry->key & (hash->size - 1);
	entry->next = hash->entries[key];
	hash->entries[key] = entry;
    }
}
