/********************************************************************************/
/* Copyright (c) 2004                                                           */
/* Daniel Sleator, David Temperley, and John Lafferty                           */
/* All rights reserved                                                          */
/*                                                                              */
/* Use of the link grammar parsing system is subject to the terms of the        */
/* license set forth in the LICENSE file included with this software,           */ 
/* and also available at http://www.link.cs.cmu.edu/link/license.html           */
/* This license allows free redistribution and use in source and binary         */
/* forms, with or without modification, subject to certain conditions.          */
/*                                                                              */
/********************************************************************************/
/* from utilities.c */
extern int verbosity;                 /* the verbosity level for error messages */
extern long long space_in_use;              /* space used but not yet freed during parse */
extern long long max_space_in_use;          /* maximum of the above for this parse */
extern long long external_space_in_use;     /* space used by "user" */
extern long long max_external_space_in_use; /* maximum of the above */

#define RTSIZE 256
/* size of random table for computing the
   hash functions.  must be a power of 2 */

extern unsigned int randtable[RTSIZE];   /* random table for hashing */

/* from error.c */
extern int   lperrno;
extern char  lperrmsg[];
