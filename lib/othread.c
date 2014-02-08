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
 * Types
 */
typedef struct nat_join {
    othread_t		*thread;
} nat_join_t;

typedef struct nat_mutex {
    pthread_mutex_t	*mutex;
} nat_mutex_t;

/*
 * Prototypes
 */
static void
native_join(oobject_t list, oint32_t size);

static void
native_mutex(oobject_t list, oint32_t size);

/* FIXME should be made language/parser builtins for inline jit generation */
static void
native_lock(oobject_t list, oint32_t size);

static void
native_unlock(oobject_t list, oint32_t size);

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
init_thread_builtin(void)
{
    obuiltin_t		*builtin;

    builtin = onew_builtin("join", native_join, t_void, false);
    onew_argument(builtin, t_void);		/* thread */
    oend_builtin(builtin);

    builtin = onew_builtin("mutex", native_mutex, t_void, false);
    oend_builtin(builtin);

    builtin = onew_builtin("lock", native_lock, t_void, false);
    onew_argument(builtin, t_void);		/* mutex */
    oend_builtin(builtin);

    builtin = onew_builtin("unlock", native_unlock, t_void, false);
    onew_argument(builtin, t_void);		/* mutex */
    oend_builtin(builtin);
}

void
finish_thread(void)
{
#if !HAVE_TLS
    pthread_key_delete(thread_self_key);
#endif
}

static void
native_join(oobject_t list, oint32_t ac)
{
    GET_THREAD_SELF()
    nat_join_t		*alist;

    alist = (nat_join_t *)list;
    if (alist->thread == null || otype(alist->thread) != t_thread)
	othrow(except_invalid_argument);
    if (pthread_equal(thread_self->pthread, alist->thread->pthread))
	othrow(except_invalid_argument);

    /* Do not fail if attempting to join a thread no longer running.
     * The thread return value, if any is gc protected by the fact
     * there is at least one reference to it. */
    pthread_join(alist->thread->pthread, null);

    ovm_move(&thread_self->r0, &alist->thread->r0);
}

static void
native_mutex(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    pthread_mutex_t	*mutex;

    onew_object(&thread_self->obj, t_mutex, sizeof(pthread_mutex_t));
    mutex = thread_self->obj;
    omutex_init(mutex);

    thread_self->r0.v.o = mutex;
    thread_self->r0.t = t_mutex;
}

static void
native_lock(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    nat_mutex_t		*alist;

    alist = (nat_mutex_t *)list;
    if (alist->mutex == null || otype(alist->mutex) != t_mutex)
	othrow(except_invalid_argument);

    omutex_lock(alist->mutex);
}

static void
native_unlock(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    nat_mutex_t		*alist;

    alist = (nat_mutex_t *)list;
    if (alist->mutex == null || otype(alist->mutex) != t_mutex)
	othrow(except_invalid_argument);

    omutex_unlock(alist->mutex);
}
