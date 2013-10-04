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

#ifndef _ocfg_h
#define _ocfg_h

#include "otypes.h"

/*
 * Prototypes
 */
extern int
ocfg_main(int argc, char *argv[]);

/*
 * Externs
 */
extern oint32_t		 cfg_optsize;
extern oint32_t		 cfg_optlevel;
extern oint32_t		 cfg_verbose;
extern oint32_t		 cfg_float_format;
extern mp_prec_t	 cfg_mpfr_prc;
extern mp_rnd_t		 cfg_mpfr_rnd;
extern char		*cfg_progname;
extern jit_state_t	*_jit;

#endif /* _ocfg_h */
