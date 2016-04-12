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
/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

#include "link-includes.h"

int main() {

    Dictionary    dict;
    Parse_Options opts;
    Sentence      sent;
    Linkage       linkage;
	Transfer trans;
    char *        diagram;
    int           i, num_linkages;
    char *        input_string[] = {"it is a translation system based on link-grammar"};

	opts  = parse_options_create();
	parse_options_set_unify_features(opts, TRUE);//Default is TRUE

    dict  = dictionary_create("4.0.dict", "4.0.knowledge", NULL, NULL, "morphology/morphemes.dict");

    for (i=0; i<1; ++i) {
		sent = sentence_create(input_string[i], dict);
		num_linkages = sentence_parse(sent, opts);
		if (num_linkages > 0) {
			linkage = linkage_create(0, sent, opts);
			printf("%s\n", diagram = linkage_print_diagram(linkage));
			string_delete(diagram);
			linkage_delete(linkage);

		}
		sentence_delete(sent);
    }

    dictionary_delete(dict);
    parse_options_delete(opts);

	printf("\n space in use : %lld", space_in_use);
	printf("\n external space in use : %lld", external_space_in_use);
	return 0;
}
