#
# Copyright (C) 2012  Paulo Cesar Pereira de Andrade
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Authors:
#	Paulo Cesar Pereira de Andrade
#

bin_PROGRAMS = owl

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(WARN_CFLAGS) $(DEBUG_CFLAGS) $(OWL_CFLAGS)

owl_LDADD = @OWL_LIBS@ -lm -ldl
owl_DEPENDENCIES= @OWL_LIBS@

owl_SOURCES =		\
	owl.c

SUBDIRS =		\
	lib

DIST_SUBDIRS =		\
	check		\
	include		\
	lib
