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

#ifndef _owl_h
#define _owl_h

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <complex.h>
#include <limits.h>
#include <pthread.h>
#include <gmp.h>
#include <mpfr.h>
#include <mpc.h>
#include <stddef.h>
#include <string.h>

#if SDL
#  include <SDL.h>
#  include <SDL_image.h>
#  include <SDL_ttf.h>
#  include <SDL_mixer.h>
#  include <SDL_opengl.h>
#  include <GL/glu.h>
#endif

#include "cqq.h"
#include "odefs.h"
#include "otypes.h"
#include "oobject.h"
#include "oparser.h"

#include "ocache.h"
#include "ocfg.h"
#include "ocode.h"
#include "odata.h"
#include "odebug.h"
#include "oemit.h"
#include "oerror.h"
#include "oeval.h"
#include "ofunction.h"
#include "ohash.h"
#include "ohashtable.h"
#include "omath.h"
#include "orat.h"
#include "oread.h"
#include "orealize.h"
#if SDL
#  include "osdl.h"
#  include "ogl.h"
#  include "oglu.h"
#endif
#include "ostream.h"
#include "ostring.h"
#include "osymbol.h"
#include "otag.h"
#include "othread.h"
#include "ovm.h"
#include "owrite.h"

#endif /* _owl_h */
