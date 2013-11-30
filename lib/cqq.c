/*
 * Copyright (C) 2013  Paulo Cesar Pereira de Andrade.
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

#include "cqq.h"

void
cqq_init(cqq_ptr u)
{
    mpq_init(cqq_realref(u));
    mpq_init(cqq_imagref(u));
}

void
cqq_clear(cqq_ptr u)
{
    mpq_clear(cqq_realref(u));
    mpq_clear(cqq_imagref(u));
}

void
cqq_set(cqq_ptr u, cqq_ptr v)
{
    if (u != v) {
	mpq_set(cqq_realref(u), cqq_realref(v));
	mpq_set(cqq_imagref(u), cqq_imagref(v));
    }
}

void
cqq_set_si(cqq_ptr u, long re, long im)
{
    mpq_set_si(cqq_realref(u), re, 1);
    mpq_set_si(cqq_imagref(u), im, 1);
}

void
cqq_set_ui(cqq_ptr u, unsigned long re, unsigned long im)
{
    mpq_set_ui(cqq_realref(u), re, 1);
    mpq_set_ui(cqq_imagref(u), im, 1);
}

void
cqq_set_z(cqq_ptr u, mpz_t z)
{
    mpq_set_z(cqq_realref(u), z);
    mpq_set_ui(cqq_imagref(u), 0, 1);
}

void
cqq_set_q(cqq_ptr u, mpq_t q)
{
    mpq_set(cqq_realref(u), q);
    mpq_set_ui(cqq_imagref(u), 0, 1);
}

void
cqq_neg(cqq_ptr u, cqq_ptr v)
{
    mpq_neg(cqq_realref(u), cqq_realref(v));
    mpq_neg(cqq_imagref(u), cqq_imagref(v));
}

void
cqq_add(cqq_ptr u, cqq_ptr v, cqq_ptr w)
{
    mpq_add(cqq_realref(u), cqq_realref(v), cqq_realref(w));
    mpq_add(cqq_imagref(u), cqq_imagref(v), cqq_imagref(w));
}

void
cqq_sub(cqq_ptr u, cqq_ptr v, cqq_ptr w)
{
    mpq_sub(cqq_realref(u), cqq_realref(v), cqq_realref(w));
    mpq_sub(cqq_imagref(u), cqq_imagref(v), cqq_imagref(w));
}

void
cqq_mul(cqq_ptr q, cqq_ptr a, cqq_ptr b)
{
    mpq_t	q1;
    mpq_t	q2;

    mpq_init(q1);
    mpq_init(q2);
    mpq_mul(q2, cqq_realref(a), cqq_imagref(b));
    mpq_mul(q1, cqq_imagref(a), cqq_realref(b));
    mpq_add(q2, q2, q1);
    mpq_mul(q1, cqq_realref(a), cqq_realref(b));
    mpq_mul(cqq_realref(q), cqq_imagref(a), cqq_imagref(b));
    mpq_swap(cqq_imagref(q), q2);
    mpq_sub(cqq_realref(q), q1, cqq_realref(q));
    mpq_clear(q1);
    mpq_clear(q2);
}

void
cqq_div(cqq_ptr q, cqq_ptr a, cqq_ptr b)
{
    mpq_t	q0;
    mpq_t	q1;
    mpq_t	q2;

    mpq_init(q0);
    mpq_init(q1);
    mpq_init(q2);
    mpq_mul(q2, cqq_imagref(a), cqq_realref(b));
    mpq_mul(q1, cqq_imagref(b), cqq_realref(a));
    mpq_sub(q2, q2, q1);
    mpq_mul(q1, cqq_realref(a), cqq_realref(b));
    mpq_mul(q0, cqq_imagref(a), cqq_imagref(b));
    mpq_add(q1, q1, q0);
    mpq_mul(cqq_imagref(q), cqq_imagref(b), cqq_imagref(b));
    mpq_mul(cqq_realref(q), cqq_realref(b), cqq_realref(b));
    mpq_add(q0, cqq_realref(q), cqq_imagref(q));
    mpq_div(cqq_imagref(q), q2, q0);
    mpq_div(cqq_realref(q), q1, q0);
    mpq_clear(q0);
    mpq_clear(q1);
    mpq_clear(q2);
}

void
cqq_si_div(cqq_ptr q, long a, cqq_ptr b)
{
    mpq_t	q0;
    mpq_t	q1;
    mpq_t	q2;

    mpq_init(q0);
    mpq_init(q1);
    mpq_init(q2);
    mpq_set_si(q0, a, 1);
    mpq_mul(q1, cqq_imagref(b), cqq_imagref(b));
    mpq_mul(q2, cqq_realref(b), cqq_realref(b));
    mpq_add(q1, q2, q1);
    mpq_mul(cqq_imagref(q), cqq_imagref(b), q0);
    mpq_neg(cqq_imagref(q), cqq_imagref(q));
    mpq_div(cqq_imagref(q), cqq_imagref(q), q1);
    mpq_mul(cqq_realref(q), q0, cqq_realref(b));
    mpq_div(cqq_realref(q), cqq_realref(q), q1);
    mpq_clear(q0);
    mpq_clear(q1);
    mpq_clear(q2);
}

void
cqq_mul_2exp(cqq_ptr u, cqq_ptr v, int w)
{
    mpq_mul_2exp(cqq_realref(u), cqq_realref(v), w);
    mpq_mul_2exp(cqq_imagref(u), cqq_imagref(v), w);
}

void
cqq_div_2exp(cqq_ptr u, cqq_ptr v, int w)
{
    mpq_div_2exp(cqq_realref(u), cqq_realref(v), w);
    mpq_div_2exp(cqq_imagref(u), cqq_imagref(v), w);
}
