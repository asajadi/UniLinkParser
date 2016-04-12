/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

#include "link-includes.h"

/***********************************************
Feature Template Set
***********************************************/
Feature_template_set * feature_template_set_create(void) {
    Feature_template_set *fts;
    int i;
    fts = (Feature_template_set *) xalloc(sizeof(Feature_template_set));
    fts->size = next_prime_up(100);
	fts->table = (Feature_template **) xalloc(fts->size * sizeof(Feature_template *));
    fts->count = 0;
    for (i=0; i<fts->size; i++) fts->table[i] = NULL;
    return fts;
}

int feature_template_find_place(char * str, Feature_template_set *fts) {
    /* lookup the given string in the table.  Return a pointer
       to the place it is, or the place where it should be. */
    int h, s, i;
	h = gen_hash_string((unsigned char *) str, fts->size);
	s = gen_stride_hash_string((unsigned char *) str, fts->size);
    for (i=h; 1; i = (i + s)%(fts->size)) {
		if ((fts->table[i] == NULL) || (strcmp(fts->table[i]->name, str) == 0)) return i;
    }
}

void feature_template_table_grow(Feature_template_set *fts) {
    Feature_template_set old;
    int i, p;

    old = *fts;
    fts->size = gen_next_prime_up(2 * old.size);  /* at least double the size */
    fts->table = (Feature_template **) xalloc(fts->size * sizeof(Feature_template *));
    fts->count = 0;
    for (i=0; i<fts->size; i++) fts->table[i] = NULL;
    for (i=0; i<old.size; i++) {
	if (old.table[i] != NULL) {
		p = feature_template_find_place(old.table[i]->name, fts);
	    fts->table[p] = old.table[i];
	    fts->count++;
	}
    }
    /*printf("growing from %d to %d\n", old.size, ss->size);*/
    fflush(stdout);
	xfree((char *) old.table, old.size * sizeof(Feature_template *));
}

Feature_template * feature_template_set_add(Feature_template * source, Feature_template_set * fts) {
    int p;

	//assert(source->name != NULL, "STRING_SET: Can't insert a null string");

	p = feature_template_find_place(source->name, fts);
    if (fts->table[p] != NULL) return fts->table[p];

    fts->table[p] = source;
    fts->count++;

    /* We just added it to the table.
       If the table got too big, we grow it.
       Too big is defined as being more than 3/4 full */
	if ((4 * fts->count) > (3 * fts->size)) feature_template_table_grow(fts);

    return source;
}
Feature_template * feature_template_set_lookup(char * source, Feature_template_set * fts) {
    int p;

	p = feature_template_find_place(source, fts);
    return fts->table[p];
}

void feature_template_set_delete(Feature_template_set *fts) {
    int i;

    if (fts == NULL) return;
    for (i=0; i<fts->size; i++) {
		if (fts->table[i] != NULL){
			feature_free(fts->table[i]->f);
			xfree(fts->table[i], sizeof(Feature_template));
		}
    }
	xfree(fts->table, fts->size * sizeof(Feature_template *));
    xfree(fts, sizeof(Feature_template_set));
}

/***********************************************
Feature connector Set
***********************************************/

Feature_conn_set * feature_conn_set_create(void) {
    Feature_conn_set *fcs;
    int i;
    fcs = (Feature_conn_set *) xalloc(sizeof(Feature_conn_set));
    fcs->size = next_prime_up(100);
	fcs->table = (Feature_connector **) xalloc(fcs->size * sizeof(Feature_connector *));
    fcs->count = 0;
    for (i=0; i<fcs->size; i++) fcs->table[i] = NULL;
    return fcs;
}

int feature_conn_find_place(char * str, Feature_conn_set *fcs) {
    /* lookup the given string in the table.  Return a pointer
       to the place it is, or the place where it should be. */
    int h, s, i;
	h = gen_hash_string((unsigned char *) str, fcs->size);
	s = gen_stride_hash_string((unsigned char *) str, fcs->size);
    for (i=h; 1; i = (i + s)%(fcs->size)) {
		if ((fcs->table[i] == NULL) || (strcmp(fcs->table[i]->conn_name, str) == 0)) return i;
    }
}

void feature_conn_table_grow(Feature_conn_set *fcs) {
    Feature_conn_set old;
    int i, p;

    old = *fcs;
    fcs->size = gen_next_prime_up(2 * old.size);  /* at least double the size */
    fcs->table = (Feature_connector **) xalloc(fcs->size * sizeof(Feature_connector *));
    fcs->count = 0;
    for (i=0; i<fcs->size; i++) fcs->table[i] = NULL;
    for (i=0; i<old.size; i++) {
	if (old.table[i] != NULL) {
		p = feature_conn_find_place(old.table[i]->conn_name, fcs);
	    fcs->table[p] = old.table[i];
	    fcs->count++;
	}
    }
    /*printf("growing from %d to %d\n", old.size, ss->size);*/
    fflush(stdout);
	xfree((char *) old.table, old.size * sizeof(Feature_connector *));
}

Feature_connector * feature_conn_set_add(Feature_connector * source, Feature_conn_set * fcs) {
    int p;

	//assert(source->name != NULL, "STRING_SET: Can't insert a null string");

	p = feature_conn_find_place(source->conn_name, fcs);
    if (fcs->table[p] != NULL) return fcs->table[p];

    fcs->table[p] = source;
    fcs->count++;

    /* We just added it to the table.
       If the table got too big, we grow it.
       Too big is defined as being more than 3/4 full */
	if ((4 * fcs->count) > (3 * fcs->size)) feature_conn_table_grow(fcs);

    return source;
}
Feature_connector * feature_conn_set_lookup(char * source, Feature_conn_set * fcs) {
    int p;

    p = feature_conn_find_place(source, fcs);
    return fcs->table[p];
}

void feature_conn_set_delete(Feature_conn_set *fcs) {
    int i;

    if (fcs == NULL) return;
    for (i=0; i<fcs->size; i++) {
		if (fcs->table[i] != NULL){
			feature_equ_free(fcs->table[i]->equ);
			xfree(fcs->table[i], sizeof(Feature_connector));
		}
    }
	xfree(fcs->table, fcs->size * sizeof(Feature_connector *));
    xfree(fcs, sizeof(Feature_conn_set));
}


