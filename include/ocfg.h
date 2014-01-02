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

#include <setjmp.h>

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
extern oint32_t		 cfg_stack_size;
/*
 * if use_semaphore is set to zero, it will use the method described in:
 *	David R. Butenhof. Programming with POSIX Threads. Addison-Wesley. ISBN 0-201-63392-2.
 * otherwise, it will use another custom method that should not spin
 * and wait reading a volatile variable, but unfortunately, it appears
 * to not be always reliable, and/or is dependent on Linux kernel version.
 *
 * note that use_semaphore is desirable if debugging with helgrind as
 * helgrind will understand the semaphore semantics and not fill the
 * default 1000+ warnings and tell you to go fix your program :-)
 *
 * maybe use_semaphore just needs some way to "flush state", possibly just
 * reading once the value of a volatile variable?
 * (I want to believe there is no way for a thread to somehow "escape" from
 * pthread_mutex_lock() due to the sem_wait() and sem_post()'s in the
 * signal handler...)
 * but there is something weird going on, as it requires 3 semaphores, or
 * will sometimes deadlock (probably when it is waiting for gc_mutex)
 */
extern obool_t		 cfg_use_semaphore;
extern mp_prec_t	 cfg_mpfr_prc;
extern mp_rnd_t		 cfg_mpfr_rnd;
extern char		*cfg_progname;
extern sigjmp_buf	 cfg_env;
extern jit_state_t	*_jit;

#endif /* _ocfg_h */
