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

#include <getopt.h>
#include <stdio.h>

/*
 * Prototypes
 */
static int
cfg_parse_options(int argc, char *argv[]);

/*
 * Initialization
 */
oint32_t		 cfg_optsize;
oint32_t		 cfg_optlevel;
oint32_t		 cfg_verbose;
oint32_t		 cfg_float_format;
mp_prec_t		 cfg_mpfr_prc;
mp_rnd_t		 cfg_mpfr_rnd;
char			*cfg_progname;
jit_state_t		*_jit;

/*
 * Implementation
 */
int
ocfg_main(int argc, char *argv[])
{
    cfg_parse_options(argc, argv);

    init_jit(cfg_progname);
    init_object();
    init_thread();
    init_hash();
    init_string();
    init_symbol();
    init_stream();
    init_parser();
    init_read();
    init_tag();
    init_function();
    init_write();
    init_code();
    init_data();
    init_eval();
    init_cache();
    init_realize();
    init_emit();

    opush_input(std_input);

    ocode();
#if 1
    orealize();

    _jit = jit_new_state();
    oemit();
    //if (cfg_verbose)
	//jit_print();
    jit_clear_state();
    if (cfg_verbose)
	jit_disassemble();

    init_vm();
    ovm(thread_main);
    finish_vm();

    jit_destroy_state();
#endif

    finish_emit();
    finish_realize();
    finish_cache();
    finish_eval();
    finish_data();
    finish_code();
    finish_write();
    finish_function();
    finish_tag();
    finish_read();
    finish_parser();
    finish_symbol();
    finish_string();
    finish_hash();
    finish_stream();
    finish_thread();
    finish_object();

    finish_jit();

    return (0);
}

static int
cfg_parse_options(int argc, char *argv[])
{
    static char		*short_options = "O::v::";
    static struct option long_options[] = {
	{ "help",		0, 0, 'h' },
	{ "fdefault-float",	1, 0, 'f' },
	{ "fmpfr-prec",		1, 0, 'p' },
	{ 0,			0, 0, 0   }
    };

    int 		 error;
    int			 opt_index;
    int			 opt_short;
    char		*endptr;

    cfg_progname = argv[0];
    cfg_mpfr_prc = mpfr_get_default_prec();
    cfg_mpfr_rnd = mpfr_get_default_rounding_mode();

    for (error = 0; !error;) {
	opt_short = getopt_long_only(argc, argv, short_options,
				     long_options, &opt_index);
	if (opt_short < 0)
	    break;
	switch (opt_short) {
	    case 'h':
	    default:
	    fail:
		fprintf(stderr,"\
Options:\n\
  -help                    Display this information\n\
  -O[0-n]                  Number of optimization passes\n\
  -Os                      Optimize for size\n\
  -v[0-3]                  Verbose output level\n\
  -fdefault-float={double|mpfr}\n\
                           Default float format\n\
  -fmpfr-prec={2-n}        Mpfr precision\n");
		error = 1;
		break;
	    case 'O':
		if (optarg) {
		    if (optarg[0] == 's' && optarg[1] == '\0')
			cfg_optsize = 1;
		    else {
			cfg_optlevel = strtol(optarg, &endptr, 10);
			if (*endptr || cfg_optlevel < 0)
			    goto fail;
		    }
		}
		else
		    cfg_optlevel = 3;
		break;
	    case 'v':
		if (optarg) {
		    cfg_verbose = strtol(optarg, &endptr, 10);
		    if (*endptr || cfg_verbose < 0)
			goto fail;
		}
		else
		    cfg_verbose = 1;
		break;
	    case 'f':
		if (strcmp(optarg, "mpfr") == 0)
		    cfg_float_format = 1;
		else if (strcmp(optarg, "double"))
		    goto fail;
		break;
	    case 'p':
		cfg_mpfr_prc = strtol(optarg, &endptr, 10);
		if (*endptr ||
		    cfg_mpfr_prc < MPFR_PREC_MIN ||
		    cfg_mpfr_prc > MPFR_PREC_MAX)
		    goto fail;
		mpfr_set_default_prec(cfg_mpfr_prc);
		break;
	}
     }

    if (error)
	exit(error);

    return (optind);
}
