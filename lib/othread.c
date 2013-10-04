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
 * Initialization
 */
#if HAVE_TLS
__thread othread_t	*thread_self;
#else
pthread_key_t		 thread_self_key;
#endif

/*
 * Implementation
 */
void
init_thread(void)
{
#if HAVE_TLS
    thread_self = thread_main;
#else
    if (pthread_key_create(&thread_self_key, null))
	oerror("pthread_key_create: %s", strerror(errno));
    if (pthread_setspecific(thread_self_key, thread_main))
	oerror("pthread_setspecific: %s", strerror(errno));
#endif
}

void
finish_thread(void)
{
#if !HAVE_TLS
    pthread_key_delete(thread_self_key);
#endif
}
