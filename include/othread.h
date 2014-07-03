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

#ifndef _othread_h
#define _othread_h

#include "otypes.h"

#include <setjmp.h>
#include <signal.h>

/* Virtual machine stack frame grows down and layout is WIIWW:
 *	fp + N			= argN
 *	...
 * 	fp + (8+word*3)		= arg0
 *	fp + (8+word*2)		= return address
 *	fp + (word+8)		= prev fp
 *	fp + (word+4)		= frame record bottom size (varargs)
 *	fp + (word)		= frame record type
 *	fp + 0			= next (next frame while constructing call)
 *	fp - size		= local variables
 *	...
 *	fp - N			= word aligned stack pointer
 *
 * [W]ord
 * [I]nt  (32 bit)
 *
 * 32 bit:
 *	size = 20
 *	arg0 = fp + 20
 *	ret  = fp + 16		W
 *	prev = fp + 12		W
 *	size = fp + 8		I
 *	type = fp + 4		I
 *	next = fp		W
 *	loc0 = fp - size
 * 64 bit:
 *	size = 32
 *	arg0 = fp + 32
 *	ret  = fp + 24		W
 *	prev = fp + 16		W
 *	size = fp + 12		I
 *	type = fp + 8		I
 *	next = fp		W
 *	loc0 = fp - size
 *
 * The stack frame format is particularly easy to cast to C structures,
 * what has the desired side effect of making builtin functions easier
 * (less error prone) to implement.
 */

#if __WORDSIZE == 32
#  define NEXT_OFFSET			 0
#  define TYPE_OFFSET			 4
#  define SIZE_OFFSET			 8
#  define PREV_OFFSET			12
#  define RET_OFFSET			16
#  define THIS_OFFSET			20
#else
#  define NEXT_OFFSET			 0
#  define TYPE_OFFSET			 8
#  define SIZE_OFFSET			12
#  define PREV_OFFSET			16
#  define RET_OFFSET			24
#  define THIS_OFFSET			32
#endif

#if HAVE_TLS
#  define GET_THREAD_SELF()		/**/
#else
#  define GET_THREAD_SELF()						\
    othread_t *thread_self = pthread_getspecific(thread_self_key);
#endif

#define omutex_lock(mutex)						\
    do {								\
	if (pthread_mutex_lock(mutex))					\
	    oerror("pthread_mutex_lock: %s", strerror(errno));		\
    } while (0)
#define omutex_unlock(mutex)						\
    do {								\
	if (pthread_mutex_unlock(mutex))				\
	    oerror("pthread_mutex_unlock: %s", strerror(errno));	\
    } while (0)
#define omutex_init(mutex)						\
    do {								\
	if ((errno = pthread_mutex_init(mutex, null)))			\
	    oerror("pthread_mutex_init: %s", strerror(errno));		\
    } while (0)
#define omutex_destroy(mutex)						\
    do {								\
	if ((errno = pthread_mutex_destroy(mutex)))			\
	    oerror("pthread_mutex_destroy: %s", strerror(errno));	\
    } while (0)
#define othreads_lock()		omutex_lock(&othread_mutex)
#define othreads_unlock()	omutex_unlock(&othread_mutex)
#define othread_kill(signo)						\
    do {								\
        pthread_kill(thread_self->pthread, signo);			\
    } while (0)

/*
 * Types
 */
struct oframe {
    oobject_t		next;
    oint32_t		type;
    oint32_t		size;
    oobject_t		prev;
    oobject_t		ret;
};

struct oregister {
#if __WORDSIZE == 64
    oint32_t		 _;
#endif
    oint32_t		 t;
    union {
	oword_t		 w;
#if __WORDSIZE == 32
	oint64_t	 l;
	struct {
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	    oint32_t	 l;
	    oint32_t	 h;
#  else
	    oint32_t	 h;
	    oint32_t	 l;
#  endif
	} s;
#endif
	ofloat_t	 d;
	rat_t		 r;
	ocdd_t		 dd;
	oobject_t	 o;
	ovector_t	*v;
    } v;
    __cqq_struct	 qq;
    __mpc_struct	 cc;
    ovector_t		*vec;
};

struct othread {
    oregister_t		 r0;
    oregister_t		 r1;
    oregister_t		 r2;
    oregister_t		 r3;
    oint8_t		*fp;		/* frame pointer */
    oint8_t		*sp;		/* stack pointer */
    oint8_t		*bp;		/* base of stack address */
    oint8_t		*ex;		/* exception stack pointer */
    oobject_t		 ev;		/* exception value */
    oobject_t		 obj;		/* gc protected general purpose use */

    /* instruction pointer, either:
     *	o thread execution start address
     *	o last known address (usually signal handler return address)
     *	  when printing a fatal backtrace */
    oint8_t		*ip;

    __cqq_struct	 qq;
    __mpc_struct	 cc;
    __mpf_struct	 f;
    ovector_t		*str;
    othread_t		*next;
    pthread_t		 pthread;

    obool_t		 run;		/* thread running? */
    obool_t		 ret;		/* thread result is not void? */
    oint32_t		 xcpt;

    sigjmp_buf		 env;		/* exceptions */

    otype_t		 type;		/* stack type */
    oint32_t		 frame;		/* frame size */
    oint32_t		 stack;		/* stack size */
};

struct oexception {
    uint8_t		*ip;		/* pointer to exception dispatcher */
    uint8_t		*fp;		/* saved frame pointer */
    uint8_t		*sp;		/* saved stack pointer */
    uint8_t		*ex;		/* previous exception handler */
    uint8_t		*th;		/* previous this pointer */
};

/*
 * Prototypes
 */
extern void
init_thread(void);

/* special later thread init function */
extern void
init_thread_builtin(void);

extern void
finish_thread(void);

/*
 * Externs
 */
#if HAVE_TLS
extern __thread othread_t	*thread_self;
#else
extern pthread_key_t		 thread_self_key;
#endif

#endif /* _ethread_h */
