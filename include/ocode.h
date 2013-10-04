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

#ifndef _ocode_h
#define _ocode_h

#include "otypes.h"

/*
 * Prototypes
 */
extern void
init_code(void);

extern void
finish_code(void);

extern void
ocode(void);

extern void
oeval_ast(oast_t *ast);

extern otag_t *
oeval_ast_tag(oast_t *ast);

/*   Perform "ast->l.ast = ast->l.ast->l.ast;" moving the object that
 * becomes unferenced to the cache. */
extern void
omove_left_ast_up(oast_t *ast);

/*   Moves all fields up. This normally should be used to move unary
 * or binary up only, but can be used on ternary operator, or if syntax
 * allows, all fields may be in use. */
extern void
omove_ast_up_full(oast_t *ast, oast_t *move);

/*
 * Externs
 */
extern orecord_t	*current_record;

#endif /* _ocode_h */
