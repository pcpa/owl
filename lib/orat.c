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

#include "owl.h"

/*
 * Implementation
 */
obool_t
orat_set_si(orat_t u, oword_t n, oword_t d)
{
    orat_num(u) = n;
    orat_den(u) = d;

    return (orat_canonicalize(u));
}

int
orat_cmp(orat_t u, orat_t v)
{
    GET_THREAD_SELF()
    obool_t		b;
    oword_t		n;
    oword_t		d;

    /* if signs differ */
    b = false;
    if (orat_num(u) < 0) {
	if (orat_num(v) >= 0)
	    return (-1);
	b = true;
    }
    else if (orat_num(v) < 0)
	return (1);

    /* if same denominator */
    if (orat_den(u) == orat_den(v)) {
	if (orat_num(u) < orat_num(v))
	    return (-1);
	if (orat_num(u) > orat_num(v))
	    return (1);
	return (0);
    }

    /* if same numerator */
    if (orat_num(u) == orat_num(v)) {
	if (orat_den(u) > orat_den(v))
	    return (-1);
	if (orat_den(u) < orat_den(v))
	    return (1);
	return (0);
    }

    n = ow_mul_w_w(orat_num(u), orat_den(v));
    d = ow_mul_w_w(orat_num(v), orat_den(u));
    if (unlikely(n == MININT || d == MININT)) {
	mpq_set_si(thr_qr, orat_num(u), orat_den(u));
	mpq_set_si(thr_qi, orat_num(v), orat_den(v));

	return (mpq_cmp(thr_qr, thr_qi));
    }

    if (n < 0)
	n = -n;
    if (d < 0)
	d = -d;

    if (b)
	return (n < d ? 1 : n > d ? -1 : 0);
    return (n < d ? -1 : n > d ? 1 : 0);
}

int
orat_cmp_si(orat_t u, oword_t v)
{
    GET_THREAD_SELF()
    obool_t		b;
    oword_t		n;
    oword_t		d;

    /* if signs differ */
    b = false;
    if (orat_num(u) < 0) {
	if (v >= 0)
	    return (-1);
	b = true;
    }
    else if (v < 0)
	return (1);

    /* if same numerator */
    if (orat_num(u) == v) {
	if (orat_den(u) > 1)
	    return (-1);
	if (orat_den(u) < 1)
	    return (1);
	return (0);
    }

    n = orat_num(u);
    d = ow_mul_w_w(v, orat_den(u));
    if (unlikely(d == MININT)) {
	mpq_set_si(thr_qr, orat_num(u), orat_den(u));
	mpq_set_si(thr_qi, v, 1);

	return (mpq_cmp(thr_qr, thr_qi));
    }

    if (n < 0)
	n = -n;
    if (d < 0)
	d = -d;

    if (b)
	return (n < d ? 1 : n > d ? -1 : 0);
    return (n < d ? -1 : n > d ? 1 : 0);
}

obool_t
orat_add(orat_t u, orat_t v, orat_t w)
{
    oword_t		n;
    oword_t		d;
    rat_t		r;

    n = ow_mul_w_w(orat_num(v), orat_den(w));
    d = ow_mul_w_w(orat_num(w), orat_den(v));
    rat_num(r) = n + d;
    rat_den(r) = ow_mul_w_w(orat_den(v), orat_den(w));
    if (unlikely(rat_den(r) == MININT || n == MININT || d == MININT ||
		 oadd_over_p(rat_num(r), n, d)))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_add_si(orat_t u, orat_t v, oword_t w)
{
    oword_t		d;
    rat_t		r;

    d = ow_mul_w_w(w, orat_den(v));
    rat_num(r) = orat_num(v) + d;
    rat_den(r) = orat_den(v);
    if (unlikely(d == MININT || oadd_over_p(rat_num(r), orat_num(v), d)))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_sub(orat_t u, orat_t v, orat_t w)
{
    oword_t		n;
    oword_t		d;
    rat_t		r;

    n = ow_mul_w_w(orat_num(v), orat_den(w));
    d = ow_mul_w_w(orat_num(w), orat_den(v));
    rat_num(r) = n - d;
    rat_den(r) = ow_mul_w_w(orat_den(v), orat_den(w));
    if (unlikely(rat_den(r) == MININT || n == MININT || d == MININT ||
		 osub_over_p(rat_num(r), n, d)))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_sub_si(orat_t u, orat_t v, oword_t w)
{
    oword_t		d;
    rat_t		r;

    d = ow_mul_w_w(w, orat_den(v));
    rat_num(r) = orat_num(v) - d;
    rat_den(r) = orat_den(v);
    if (unlikely(d == MININT || osub_over_p(rat_num(r), orat_num(v), d)))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_si_sub(orat_t u, oword_t v, orat_t w)
{
    oword_t		n;
    rat_t		r;

    n = ow_mul_w_w(v, orat_den(w));
    rat_num(r) = n - orat_num(w);
    rat_den(r) = orat_den(w);
    if (unlikely(n == MININT || osub_over_p(rat_num(r), n, orat_num(w))))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_mul(orat_t u, orat_t v, orat_t w)
{
    rat_t		r;

    rat_num(r) = ow_mul_w_w(orat_num(v), orat_num(w));
    rat_den(r) = ow_mul_w_w(orat_den(v), orat_den(w));

    if (unlikely(rat_num(r) == MININT || rat_den(r) == MININT))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_mul_si(orat_t u, orat_t v, oword_t w)
{
    rat_t		r;

    rat_num(r) = ow_mul_w_w(orat_num(v), w);
    rat_den(r) = orat_den(v);

    if (unlikely(rat_num(r) == MININT))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_div(orat_t u, orat_t v, orat_t w)
{
    rat_t		r;

    rat_num(r) = ow_mul_w_w(orat_num(v), orat_den(w));
    rat_den(r) = ow_mul_w_w(orat_den(v), orat_num(w));

    if (unlikely(rat_num(r) == MININT || rat_den(r) == MININT))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_div_si(orat_t u, orat_t v, oword_t w)
{
    rat_t		r;

    rat_num(r) = orat_num(v);
    rat_den(r) = ow_mul_w_w(orat_den(v), w);

    if (unlikely(rat_den(r) == MININT))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_si_div(orat_t u, oword_t v, orat_t w)
{
    rat_t		r;

    rat_num(r) = ow_mul_w_w(v, orat_den(w));
    rat_den(r) = orat_num(w);

    if (unlikely(rat_num(r) == MININT))
	return (false);

    if (!orat_canonicalize(&r))
	return (false);

    orat_num(u) = rat_num(r);
    orat_den(u) = rat_den(r);

    return (true);
}

obool_t
orat_canonicalize(orat_t rat)
{
    GET_THREAD_SELF()
    oword_t		n;
    oword_t		d;
    oword_t		m;

    n = orat_num(rat);
    d = orat_den(rat);

    if (unlikely(d == 0))
	othread_kill(SIGFPE);

    if (unlikely(n == MININT || d == MININT))
	return (false);

    if (d == 1)
	return (true);
    if (n == 0) {
	orat_den(rat) = 1;
	return (true);
    }

    if (n < 0)
	n = -n;
    for (;;) {
	if ((m = d % n) == 0)
	    break;
	d = n;
	n = m;
    }
    if (d != 1) {
	orat_num(rat) /= n;
	orat_den(rat) /= n;
    }

    if (orat_den(rat) < 0) {
	orat_num(rat) = -orat_num(rat);
	orat_den(rat) = -orat_den(rat);
    }

    return (true);
}
