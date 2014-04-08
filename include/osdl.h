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

#ifndef _osdl_h
#define _osdl_h

#include "otypes.h"

/*
 * Types
 */
struct opoint {		/* Must match SDL_Point */
    oint32_t		 x;
    oint32_t		 y;
};

struct orect {		/* Must match SDL_Rect */
    oint32_t		 x;
    oint32_t		 y;
    oint32_t		 w;
    oint32_t		 h;
};

struct owindow {
    SDL_Window		*__window;
    ohashentry_t	*__handle;		/* convert id to owindow_t */
    orenderer_t		*__renderer;
    oint32_t		 x;
    oint32_t		 y;
    oint32_t		 w;
    oint32_t		 h;
    oint32_t		 min_w;
    oint32_t		 min_h;
    oint32_t		 max_w;
    oint32_t		 max_h;
    ouint32_t		 flags;
    ovector_t		*title;
    oint32_t		 __x;
    oint32_t		 __y;
    oint32_t		 __w;
    oint32_t		 __h;
    oint32_t		 __min_w;
    oint32_t		 __min_h;
    oint32_t		 __max_w;
    oint32_t		 __max_h;
    ouint32_t		 __flags;
    ovector_t		*__title;
};

struct orenderer {
    SDL_Renderer	*__renderer;
    owindow_t		*__window;
    otexture_t		*target;
    oint32_t		 log_w;
    oint32_t		 log_h;
    oint32_t		 max_w;
    oint32_t		 max_h;
    ovector_t		*name;
    ouint32_t		 flags;
    ovector_t		*formats;
    otexture_t		*__target;
    oint32_t		 __log_w;
    oint32_t		 __log_h;
};

struct osurface {
    SDL_Surface		*__surface;
};

struct otexture {
    SDL_Texture		*__texture;
    ohashentry_t	*__handle;		/* convert pointers */
    ouint32_t		 format;
    oint32_t		 access;
    oint32_t		 w;
    oint32_t		 h;
    ouint8_t		 r;
    ouint8_t		 g;
    ouint8_t		 b;
    ouint8_t		 a;
    ouint8_t		 blend;
    ouint8_t		 __r;
    ouint8_t		 __g;
    ouint8_t		 __b;
    ouint8_t		 __a;
    ouint8_t		 __blend;
};

struct ofont {
    TTF_Font		*__font;
};

struct oevent {
    SDL_Event		*__event;
    ouint32_t		 type;			/* type */
    ouint32_t		 time;			/* timestamp */
    union {
	oobject_t	 window;		/* window id if applicable */
	oobject_t	 gesture;		/* gesture id if applicable */
	oobject_t	 finger;		/* finger id if applicable */
    };
    oobject_t		 device;		/* mouse or joystick or touch */
    union {
	ouint16_t	 state;			/* button or key,
						 * Pressed or Released */
	ouint16_t	 event;			/* window event detail */
	ouint16_t	 axis;			/* joystick axis index */
	ouint16_t	 ball;			/* joystick trackball index */
	ouint16_t	 hat;			/* joystick hat index */
    };
    union {
	ouint16_t	 repeat;		/* key repeat if non zero or
						 * button repeat count */
	ouint16_t	 fingers;
    };
    union {
	oint32_t	 button;		/* mouse button */
	oint32_t	 keysym;		/* keysym of pressed key */
	oint32_t	 value;			/* joystick axis value
						 * (-32768 to 32767) */
	ofloat32_t	 pressure;
    };
    union {
	oint32_t	 x;			/* X coordinate */
	oint32_t	 w;			/* window new width */
	ofloat32_t	 fx;
	oint32_t	 start;			/* start of selected
						 * edit text */
    };
    union {
	oint32_t	 y;			/* Y coordinate */
	oint32_t	 h;			/* window new height */
	ofloat32_t	 fy;
	oint32_t	 length;		/* length of edit selected
						 * edit text */
    };
    union {
	oint32_t	 dx;			/* relative X mouse or
						 * joystick ball motion */
	ofloat32_t	 fdx;
	ofloat32_t	 theta;
	ofloat32_t	 ferror;
    };
    union {
	oint32_t	 dy;			/* relative Y mouse or
						 * joystick ball motion */
	ofloat32_t	 fdy;
	ofloat32_t	 dist;
    };
    ovector_t		*text;			/* edit or input text */
    otimer_t		*timer;			/* timer event information */
};

struct otimer {
    ouint32_t		 __timer;		/* SDL_TimerID */
    ouint32_t		 msec;
    oobject_t		 data;
};

/*
 * Prototypes
 */
extern void
init_sdl(void);

extern void
finish_sdl(void);

extern void
odestroy_window(owindow_t *window);

extern void
odestroy_texture(otexture_t *texture);

/*
 * Externs
 */
extern orecord_t	*sdl_record;

#endif /* _osdl_h */
