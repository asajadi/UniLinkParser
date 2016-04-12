/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

#include "link-includes.h"

/*
 The following functions can be found in string-set.c, the only difference is that
  the String_set parameter is omitted, so, they can be used by hash sets other that String_sets,
   */

int gen_hash_string(unsigned char *str, int table_size) {
    unsigned int accum = 0;
    for (;*str != '\0'; str++) accum = ((256*accum) + (*str)) % (table_size);
    return accum;
}

int gen_stride_hash_string(unsigned char *str, int table_size) {
  /* This is the stride used, so we have to make sure that its value is not 0 */
    unsigned int accum = 0;
    for (;*str != '\0'; str++) accum = ((17*accum) + (*str)) % (table_size);
    if (accum == 0) accum = 1;
    return accum;
}

int gen_next_prime_up(int start) {
/* return the next prime up from start */
    int i;
    start = start | 1; /* make it odd */
    for (;;) {
	for (i=3; (i <= (start/i)); i += 2) {
	    if (start % i == 0) break;
	}
	if (start % i == 0) {
	    start += 2;
	} else {
	    return start;
	}
    }
}

