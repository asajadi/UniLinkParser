/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/
#include "link-includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include "error.h"

/*******************************
*   Some General Utilities

*******************************/

String_list* string_list_search(String_list *string_list, char *str){
	String_list *sl;
	for (sl=string_list; (sl!=NULL) && (strcmp(sl->string, str)!=0);sl=sl->next );
	return sl;
}
String_list * string_list_create(String_list **sl){
	String_list * sl1=*sl;
	if(sl1==NULL){
		sl1 = (String_list *)xalloc(sizeof(String_list));
		sl1->next = NULL;
		(*sl)=sl1;
	}
	else{
		for(;sl1->next!=NULL;sl1=sl1->next);
		sl1->next = (String_list *)xalloc(sizeof(String_list));
		sl1=sl1->next;
		sl1->next = NULL;
	}
	return sl1;
}

void  string_list_free(String_list *sl){
	if (sl==NULL)
		return;
	string_list_free(sl->next);
	xfree((String_list *)sl, sizeof(String_list));
	return ;
}
String_list * string_list_ex_create(String_list **sl){
	String_list * sl1=*sl;
	if(sl1==NULL){
		sl1 = (String_list *)xalloc(sizeof(String_list));
		sl1->next = NULL;
		(*sl)=sl1;
	}
	else{
		for(;sl1->next!=NULL;sl1=sl1->next);
		sl1->next = (String_list *)xalloc(sizeof(String_list));
		sl1=sl1->next;
		sl1->next = NULL;
	}
	return sl1;
}

void  string_list_ex_free(String_list *sl){
	if (sl==NULL)
		return;
	string_list_ex_free(sl->next);
	string_delete(sl->string);
	xfree((String_list *)sl, sizeof(String_list));
	return ;
}

int conn_name_is_valid(char * s){
	char cname[MAX_CONNECTOR_NAME];
	int delim;
	if (*s=='@'){
		s++;
	}
	delim = get_conn_delim(s);
	strcpy_to_delim(cname, s, delim);
	if(!isupper(*s) && (strcmp(cname, "d^")!=0)&& (strcmp(cname, "*^")!=0)
		&& (strcmp(cname, "^")!=0)){
		return FALSE;
	}
	return TRUE;
	//for(;*s!='\0'; s++){
	//	if
	//}
}

int  is_link_name(char *str){
	/*Returns TRUE if it is a pure link name (all must be uppercase)*/
	int i;
	int is_upp;
	return conn_name_is_valid(str);
	is_upp = TRUE;
	for (i=0; str[i]!='\0';i++){
		if (!isupper((int)str[i])){
			is_upp=FALSE;
			break;
		}
	}
	return is_upp;
}
void make_file_name(char*trg, char* src, char* aux){
	char *s, *end, file_name[MAX_PATH_NAME];
	int len;
	len = strlen(src);
	end = src+len;
	for (s=end; s!=src && *s!='.'; s--);
	if (s==src)
		s = end;
	strncpy(file_name, src, s-src);
	file_name[s-src] = '\0';
 	sprintf(trg, "%s.%s%s", file_name, aux, s);
}
/*******************************
*   Read Feature Files Utilities

*******************************/
void feature_warning(char *fmt, ...) {
    char temp[1024];
    va_list args;

    va_start(args, fmt);
    vsprintf(temp, fmt, args);
	printf("   Warning. %s \n", temp );
    va_end(args);
    fflush(stderr);
}
void feature_error(Dictionary dict,char *fmt, ...) {
    int i;
    char temp[1024];
    char tokens[1024], t[128];
    va_list args;

    va_start(args, fmt);
    vsprintf(temp, fmt, args);
    tokens[0] = '\0';

	for (i=0; i<10 && dict->feature_token!=NULL && dict->feature_token[0] != '\0' ; i++) {
	sprintf(t, "\"%s\" ", dict->feature_token);
	strcat(tokens, t);
	dict->feature_token = get_a_word(dict,dict->feature_fp);
    }
	lperror(DICTPARSE, ".\n\tFile name: %s, %s\n\t , tokens = %s\n",dict->current_file_name,
	    temp, tokens);
    va_end(args);
    fflush(stderr);

}
Feature * find_feature_path(String_list * path, Feature *f){
	String_list *sl;
	Feature * f1;
	if(f==NULL)
		return NULL;
	//if (strcmp(path->string, ROOT_FEATURE)==0){
	//	return f->child;
	//}
	//else{
	//	f=f->child;
	//}
	for(sl=path, f1=f; sl!=NULL; sl=sl->next,f1=f1->child){
		for(; (f1!=NULL) && (strcmp(sl->string, f1->attribute)!=0);f1=f1->next);
		if(f1==NULL){
			return NULL;
		}
	}
	return f1;
}
Feature * find_feature_path_string(Feature *f, char *path){
	String_list *l_path=NULL;
	String_list *l_path_i;
	Feature *found_f;
	int i;
	char f_name[MAX_FEATURE_ATT];
	while(*path!='\0'){
		for(;isspace(*path); path++);
		for(i=0;!isspace(*path) && *path!='\0'; i++, path++)
			f_name[i]=*path;
		f_name[i]='\0';
		l_path_i = string_list_create(&l_path);
		l_path_i->string = (char*)exalloc(strlen(f_name)+1);
		strcpy(l_path_i->string, f_name);
	}
	found_f = find_feature_path(l_path, f);
	string_list_ex_free(l_path);
	return found_f;
}

int feature_get_character(FILE *fp) {
/* This gets the next character from the input, eliminating comments.
   If we're in quote mode, it does not consider the % character for
   comments */

    int c;

    c = fgetc(fp);
    if (c == '%') {
	while((c != EOF) && (c != '\n')) c = fgetc(fp);
    }
    return c;
}
char * feature_get_a_word(Dictionary dict, FILE * fp) {
    char word[MAX_WORD+1];
    char * s;
    int c, j;
	static int already_got_it='\0';
	if (already_got_it!='\0'){

		word[0]=already_got_it;
		word[1]='\0';
	    s = string_set_add(word, dict->string_set);
		already_got_it='\0';
		return s;
	}
    do {
	c = feature_get_character(fp);
    } while ((c != EOF) && isspace(c));
    if (c == EOF) return NULL;

    for (j=0; (j <= MAX_WORD-1) && (!isspace(c)) && (c != EOF); j++) {
		if (strchr(SPECIAL_CHARS, c)!=NULL){
		if (j==0){
			word[j] = c;
			word[j+1]='\0';
		    s = string_set_add(word, dict->string_set);
		    return s;
		}
		else{
			already_got_it =c;
		}
		break;
	}
	word[j] = c;
	c = feature_get_character(fp);
    }

    if (j == MAX_WORD) {
	error("The dictionary contains a word that is too long.");
    }
    word[j] = '\0';
    s = string_set_add(word, dict->string_set);
    return s;
}

int string_is_equal(char * s, int c) {
    return (is_special(s) && c==s[0] && s[1] == '\0');
}
Feature_label_node * label_find(Feature_label_node * l,  char * label_str) {
	Feature_label_node * l1=NULL;
	for (l1=l; l1!=NULL; l1=l1->next){
		if (strcmp(l1->label, label_str)==0){
			break;
		}
	}
    return l1;

}
int feature_already_defined(Feature *f, char* str){
	Feature * fi;
	int ad=FALSE;
	for(fi=f;fi!=NULL;fi=fi->next){
		if(strcmp(f->attribute, str)==0){
			ad=TRUE;
			break;
		}
	}
	return ad;
}
int feature_append( Feature **f1, Feature * f2) {
	Feature * fi, *fj;
	if((*f1)==NULL){
		(*f1)=f2;
		return TRUE;
	}
	for (fi=*f1; fi!=NULL; fi=fi->next){
		for (fj=f2; fj!=NULL; fj=fj->next){
			if (strcmp(fi->attribute,fj->attribute)==0){
				return FALSE;
			}

		}
	}
	for(fi=*f1;fi->next!=NULL;fi=fi->next);
	fi->next = f2;
	return TRUE;

}

void feature_list_add( Feature_list **f1, Feature * f2) {
	Feature_list * fi, *fj;
	fi = *f1;
	if (f2==NULL){
		return;
	}
	if(*f1==NULL){
		*f1=(Feature_list *)xalloc(sizeof(Feature_list));
		(*f1)->f=f2;
		(*f1)->next=NULL;
		return ;
	}
	for(;fi!=NULL;fj=fi,fi=fi->next){
		if(fi->f==f2){
			return;
		}
	}
	fj->next=(Feature_list *)xalloc(sizeof(Feature_list));
	fj=fj->next;
	fj->f = f2;
	fj->next = NULL;
	return ;

}
void feature_list_append( Feature_list **f1, Feature_list * f2) {
	Feature_list * fi;
	fi = *f1;
	if (f2==NULL){
		return;
	}
	if(*f1==NULL){
		(*f1)=f2;
		return ;
	}
	for(;fi->next!=NULL;fi=fi->next){
		if(fi==f2){
			return;
		}
	}
	fi->next=f2;
	return ;

}
void  set_feature_parents(Feature *f1, Feature * f1_parent){
	Feature *fi;

	feature_list_add(&(f1->parent), f1_parent);
	for (fi=f1->next; fi!=NULL; fi=fi->next){
		fi->parent=f1->parent;
	}
}
void add_to_parents(Feature *f, Feature_list *parent){
Feature_list *iparent;
	if(parent==NULL){
		f->parent=NULL;
		return;
	}
	if(f!=NULL){
		feature_list_append(&(f->parent),parent);
	}
	for(iparent=parent; iparent!=NULL; iparent=iparent->next){
		iparent->f->child = f;
	}
}
void delete_from_childs(Feature *f){
	Feature_list *iparent, *jparent, *prev;
	if(f->child==NULL){
		return ;
	}
	for(iparent=f->child->parent, prev=NULL; iparent!=NULL;){
		jparent=iparent->next;
		if(iparent->f ==f){
			if(prev!=NULL){
				prev->next = iparent->next;
			}
			else{
				f->child->parent=iparent->next;
			}
			xfree(iparent, sizeof(Feature_list));
		}
		else{
			prev=iparent;
		}
		iparent=jparent;
	}
}
void replace_node(Feature *f1, Feature *f2){
	delete_from_childs(f1);
	if(f1->child!=NULL){
		feature_list_add(&(f1->child->parent), f2);
		}
	f2->child=f1->child;
	f1->child=NULL;
}

void feature_copy_table_add(Feature_copy_table **fct, Feature *old_f, Feature *new_f){
	Feature_copy_table * fct_node;
	fct_node = (Feature_copy_table *) xalloc(sizeof(Feature_copy_table));
	fct_node->old_f = old_f;
	fct_node->new_f = new_f;
	fct_node->next = (*fct);
	(*fct) = fct_node;
}
Feature * feature_copy_table_find(Feature_copy_table *fct, Feature *old_f){
	Feature_copy_table * fct_node;
	Feature *found_f=NULL;
	for(fct_node=fct;fct_node!=NULL;fct_node = fct_node->next){
		if (fct_node->old_f == old_f)
			found_f = fct_node->new_f;
			break;
	}
	return found_f;
}
Feature * feature_copy(String_set *ss, Feature *f, Feature_copy_table **fct)
{
	Feature * f1, *fi, *fj, *already_visited;
	if (f==NULL)
		return NULL;
    f1 = (Feature *) xalloc(sizeof(Feature));
	*f1 = *f;
	if (ss!=USE_SOURCE_STRING_SET)
		f1->attribute=string_set_add(f->attribute, ss);
	f1->parent=NULL;
	f1->next = NULL;
	f1->child=NULL;
	f1->share_stat=SHST_NONE;
	feature_copy_table_add(fct, f, f1);
	if(f->child!=NULL){
		if((already_visited=feature_copy_table_find(*fct, f->child))!=NULL){
			f1->child=already_visited;
		}
		else{
			fj=f1->child = feature_copy(ss, f->child, fct);
			feature_list_add(&(fj->parent), f1);
			for (fi=f->child->next; fi!=NULL;fi=fi->next){
				fj->next= feature_copy(ss, fi,fct);
				fj=fj->next;
				fj->parent=f1->child->parent;
			}
		}
	}
    return f1;

}
Feature * feature_copy_driver(String_set *ss, Feature *f){
	//This function copies a feature and sets the root parent.

	Feature_copy_table *fct=NULL;
	Feature_copy_table *fct_node=NULL;
	Feature *f1;
	if (f==NULL){
		return NULL;
	}
	f1 = feature_copy(ss, f, &fct);
	f1->parent=NULL;
	for(fct_node = fct;fct_node!=NULL;fct=fct_node){
		fct_node = fct->next;
		xfree((char*)fct, sizeof(Feature_copy_table));
	}
	return f1;
}

void feature_sibling_copy(String_set *ss, Feature **f1, Feature*f2){
	if (f2==NULL){
		(*f1)=NULL;
        return;
	}
	(*f1)=feature_copy_driver(ss, f2);
	feature_sibling_copy(ss, &((*f1)->next), f2->next);
}

Feature * feature_node_create(void) {
	Feature * f;
	f = (Feature *)xalloc(sizeof(Feature));
	f->child = NULL;
	f->parent = NULL;
	f->share_stat = SHST_NONE;
	f->next = NULL;
	return f;

}
Feature * feature_create(Feature ** f) {
	Feature * f1=*f;
	if(f1==NULL){
		f1 = feature_node_create();
		f1->next = NULL;
		(*f)=f1;
		return f1;
	}
	for(;f1->next!=NULL;f1=f1->next);
	f1->next = feature_node_create();
	f1=f1->next;
	f1->next = NULL;
	return f1;

}
Feature_equ * feature_equ_create(Feature_equ *fe){
	Feature_equ * fe1;
	fe1 = (Feature_equ *)xalloc(sizeof(Feature_equ));
	fe1->agr1_stat=AGR_NOT_INITED;
	fe1->agr2_stat=AGR_NOT_INITED;
	fe1->next = fe;
	return fe1;
}

Feature_connector * feature_connector_create(void){
	Feature_connector * fc;
	fc = (Feature_connector *)xalloc(sizeof(Feature_connector));
	fc->equ = NULL;
	return fc;
}
Feature_label_node * label_create(Dictionary dict, Feature_label_node * l,  char * new_label) {
	Feature_label_node * l1;
	if (label_find(l, new_label)!=NULL){
		feature_error(dict, "The label is already defined");
		return NULL;
	}
    l1 = (Feature_label_node *) xalloc(sizeof(Feature_label_node));
	l1->next = l;
	l = l1;
    return l;

}

void extract_link_name(char *str, char *conn_name){
	/*Returns the upper leading uppercase characters of the link name*/
	char *u;
	for (u=conn_name;isupper((int)*u); u++);
	strncpy(str,  conn_name, u-conn_name);
	str[u-conn_name]='\0';
}

void feature_list_free_nodes(Feature_list * fl){
	if(fl==NULL)
		return;
	feature_list_free_nodes(fl->next);
	xfree((char*)fl, sizeof(Feature_list));
}
void feature_free(Feature * f) {
    Feature * f1;
	Feature_list *parent;
	if((f==NULL) ||(f->share_stat==IS_SHARED)){
		return;
	}
	for (parent=f->parent; parent!=NULL; parent=parent->next){
		parent->f->child=NULL;
	}
	feature_list_free_nodes(f->parent);

     for (; f != NULL; f = f1) {
        f1 = f->next;
		if(f->share_stat==IS_SHARED){
			break;
		}
		feature_free(f->child);
		xfree(f, sizeof(Feature));
    }
}



void shared_features_free(Feature * f) {
    Feature * f1;
	Feature_list *parent;
	if(f==NULL){
		return;
	}
	for (parent=f->parent; parent!=NULL; parent=parent->next){
		parent->f->child=NULL;
	}
	feature_list_free_nodes(f->parent);

     for (; f != NULL; f = f1) {
        f1 = f->next;
		shared_features_free(f->child);
		xfree(f, sizeof(Feature));
    }
}

void feature_list_free(Feature_list * fl){
	if(fl==NULL)
		return;
	feature_list_free(fl->next);
	shared_features_free(fl->f);
	xfree((char*)fl, sizeof(Feature_list));
}
void free_feature_array(Parse_info *pi){
	int i;
	for (i=0; i<pi->N_words ;i++){
			feature_free(pi->feature_array[i]);
		}
}

void feature_label_node_free(Feature_label_node * ln) {
    Feature_label_node * ln1;
	if (ln==NULL)
		return;
     for (; ln != NULL; ln = ln1) {
        ln1 = ln->next;
		xfree(ln, sizeof(Feature_label_node));
    }
}


void feature_equ_free(Feature_equ *fe){
	if (fe==NULL)
		return;
	feature_equ_free(fe->next);
	if(fe->agr1_stat==AGR_PATH)
		string_list_free(fe->agr1.path);
	else
		if(fe->agr1_stat==AGR_FEATURE)
			feature_free(fe->agr1.f);

	if(fe->agr2_stat==AGR_PATH)
		string_list_free(fe->agr2.path);
	else
		if(fe->agr2_stat==AGR_FEATURE)
			feature_free(fe->agr2.f);

	xfree((char *)fe, sizeof(Feature_equ));
	return;
}
Feature * feature_read(Dictionary dict)
{
	Feature * f=NULL;
	Feature * f1=NULL;
	Feature * f2=NULL;
	Feature_template * ft;
	Feature_label_node * ln;
	FILE *fp;
	fp=dict->feature_fp;
	if ((dict->feature_token=get_a_word(dict,fp))==NULL){
		return NULL;
	}
	for (;!string_is_equal(dict->feature_token,']');) {
		//<template>
		if (string_is_equal(dict->feature_token, '<')){
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
			ft=feature_template_set_lookup(dict->feature_token, dict->feature_template_set);
			if(ft==NULL){
				feature_error(dict, "Template not found");
				return 0;
			}
			feature_sibling_copy(dict->string_set, &f1, ft->f->child);
			if (feature_append(&f, f1)==FALSE){
				feature_error(dict, "Feature already defined");
				return NULL;
			}
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
			if (!string_is_equal(dict->feature_token,'>')){
				feature_error(dict,  "I expected \">\" but didn\'t get it.");
				return 0;
			}
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
		}
		//</template>
		else{// else #1
			if(feature_already_defined(f, dict->feature_token)==TRUE){
				feature_error(dict, "Feature already defined");
				return NULL;
			}
			f1=feature_create(&f);
			f1->attribute=string_set_add(dict->feature_token, dict->string_set);
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
			if (!string_is_equal(dict->feature_token, ':')) {
				feature_error(dict, "Expecting a \":\".");
				return NULL;
			}
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
			//<The value is itself a DAG>
			if (string_is_equal(dict->feature_token,'[')){
				if ((f1->child = feature_read(dict))==NULL){
					return 0;
				}
				set_feature_parents(f1->child, f1);
				if (!string_is_equal(dict->feature_token,']')){
					feature_error(dict, "I expected \"]\" but didn\'t get it.");
					return 0;
				}
				if ((dict->feature_token=get_a_word(dict,fp))==NULL){
					return NULL;
				}
			}
			//</The value is itself a DAG>
			else{//else #2
				//<The value is a reference>
				if (string_is_equal(dict->feature_token, '(')){//reference
					if ((dict->feature_token=get_a_word(dict,fp))==NULL){
						return NULL;
					}
					if((ln=label_find(dict->fln, dict->feature_token))==FALSE){
						feature_error(dict, "Label not found");
						return 0;
					}
					f1->child = ln->f->child;
					set_feature_parents(f1->child, f1);
					if ((dict->feature_token=get_a_word(dict,fp))==NULL){
						return NULL;
					}
					if (!string_is_equal(dict->feature_token,')')){
						feature_error(dict, "I expected \")\" but didn\'t get it.");
						return 0;
					}
					if ((dict->feature_token=get_a_word(dict,fp))==NULL){
						return NULL;
					}
				}
				//</The value is a reference>
				else{//else #3
					//<Defining the values>
					for (;!string_is_equal(dict->feature_token,']')  ;) {
						if ( ((dict->feature_token[0]==',')||(dict->feature_token[0]=='(')) && (dict->feature_token[1]=='\0') )
								break;
						if (is_special(dict->feature_token)) {
							feature_error(dict, "I expected a value but didn\'t get it.");
							return NULL;
						}
						if(feature_already_defined(f1->child, dict->feature_token)==TRUE){
							feature_error(dict, "Feature already defined");
							return NULL;
						}
						f2=feature_create(&(f1->child));
						f2->attribute=string_set_add(dict->feature_token, dict->string_set);
						if ((dict->feature_token=get_a_word(dict,fp))==NULL){
							return NULL;
						}
					} //for
					set_feature_parents(f1->child, f1);
	                //</Defining the values>
 				}//else #3
			}//else #2
		}// else #1
		// <Defining a reference>
		if (string_is_equal(dict->feature_token,'(')){
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
			if (is_special(dict->feature_token)) {
				feature_error(dict, "I expected a label but didn\'t get it.");
				return 0;
			}
			if ((dict->fln = label_create(dict, dict->fln, dict->feature_token))==NULL)
			{
				return NULL;
			}
			safe_strcpy(dict->fln->label, dict->feature_token, MAX_FEATURE_LABEL);
			dict->fln->f = f1;
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
			if (!string_is_equal(dict->feature_token,')')){
				feature_error(dict, "I expected \")\" but didn\'t get it.");
				return 0;
			}
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
		}
		// </Defining a reference>
		if ((dict->feature_token[0]==',') && (dict->feature_token[1]=='\0') ){
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return NULL;
			}
		}
	}//for
	if (f==NULL){
		f=feature_node_create();
		f->attribute = string_set_add("?", dict->string_set);
	}
	return f;
}


Feature * feature_read_driver(Dictionary dict){
	Feature * f;
	f=feature_node_create();
	f->attribute = string_set_add(ROOT_FEATURE, dict->string_set);
	if ((f->child = feature_read(dict))==NULL){
		return NULL;
	}
    set_feature_parents(f->child, f);
    feature_label_node_free(dict->fln);
	dict->fln = NULL;
 	safe_strcpy(dict->token, dict->feature_token, MAX_TOKEN_LENGTH);
	return f;
}
int read_general_word_file_features(Dictionary dict, Dict_node * dn){
	Dict_node *dnx;
	Feature * fx;
	Feature *fi, *fj;
	Feature_list * fl;
	fl=(Feature_list *)xalloc(sizeof(Feature_list));
	fl->next = dict->shared_features;
	dict->shared_features = fl;
	if(!(dict->shared_features->f = feature_read_driver(dict))){
		return 0;
	}
	dict->shared_features->f->share_stat=IS_SHARED;
	for (fx = dict->shared_features->f->child; fx != NULL; fx = fx->next) {
		fx->share_stat = IS_SHARED;
	}
	for (dnx = dn; dnx != NULL; dnx = dnx->left) {
		if(dnx->f==NULL){
			dnx->f=dict->shared_features->f;
			continue;
		}
		for(fi=dnx->f->child; fi!=NULL; fi=fi->next){
			for(fj=dict->shared_features->f->child; fj!=NULL; fj=fj->next){
				if(strcmp(fi->attribute, fj->attribute)==0){
					feature_error(dict,"Feature \"%s\" already defined. line number: %d", fi->attribute, dict->line_number);
					return FALSE;
				}
			}
		}
		for(fx=dnx->f->child;fx->next!=NULL;fx=fx->next);
		fx->next = dict->shared_features->f->child;
    }
return TRUE;
}
int is_special(char * s) {
/* returns TRUE if this token is a special token and it is equal to c */
	return ((strchr(SPECIAL_CHARS, s[0])!=NULL) && (s[1]=='\0'));
}
void comment_template(Dictionary dict, FILE *fp){
	while (TRUE){
		dict->feature_token=get_a_word(dict,fp);
		if ((dict->feature_token==NULL) || (strcmp(dict->feature_token,"%")==0) )
			break;
	}
	dict->feature_token=get_a_word(dict,fp);
	return;
}

int  feature_templates_read(Dictionary dict, char * dict_path_name)
{
	Feature_template *ft;
	FILE * fp;

	if ((fp = dictopen(dict_path_name, dict->feature_template_file_name, "r")) == NULL) {
	lperror(NODICT, dict->feature_template_file_name);
	return 0;
    }
	dict->feature_fp = fp;
	dict->current_file_name = dict->feature_template_file_name;
	while((dict->feature_token=get_a_word(dict,fp))!=NULL)
	{
		if(strcmp(dict->feature_token,"%")==0) {
			comment_template(dict, fp);
			if ((dict->feature_token=get_a_word(dict,fp))==NULL){
				return TRUE;
			}
		}
		if(!string_is_equal(dict->feature_token,'<')) {
			feature_error(dict, "Expected \"<\" ");
			return FALSE;
		}

		if ((dict->feature_token=get_a_word(dict,fp))==NULL){
			return FALSE;
		}

		if(feature_template_set_lookup(dict->feature_token, dict->feature_template_set)!=NULL) {
			feature_error(dict, "Feature template already defined!");
			return FALSE;
		}

		ft = (Feature_template *) xalloc(sizeof(Feature_template));
		ft->name = string_set_add(dict->feature_token, dict->string_set);
//		ft->f = NULL;

		if ((dict->feature_token=get_a_word(dict,fp))==NULL){
			return FALSE;
		}
		if(!string_is_equal(dict->feature_token,'>')) {
			feature_error(dict, "Expected \">\" ");
			return FALSE;
		}

		if ((dict->feature_token=get_a_word(dict,fp))==NULL){
			return FALSE;
		}
		if(!string_is_equal(dict->feature_token,':')) {
			feature_error(dict, "Expected \":\" ");
			return FALSE;
		}

		if ((dict->feature_token=get_a_word(dict,fp))==NULL){
			return FALSE;
		}

		if((ft->f = feature_read_driver(dict))==NULL){
			return FALSE;
		}
		if ((dict->feature_token=get_a_word(dict,fp))==NULL){
			return FALSE;
		}
		if(!string_is_equal(dict->feature_token,';')) {
			feature_error(dict, "Expected \";\" ");
			return FALSE;
		}
		feature_template_set_add(ft, dict->feature_template_set);
	}
fclose(fp);
return 1;
}
int feature_is_equal(Feature * f1 , Feature * f2){
	int is_equ=TRUE;
	Feature *fi, *fj;
	if ((f1==NULL)&&(f2==NULL))
		return TRUE;
	if (((f1==NULL)&&(f2!=NULL)) || ((f1!=NULL)&&(f2==NULL)))
		return FALSE;

	for(fi=f1; fi!=NULL; fi=fi->next){
		for(fj=f2; fj!=NULL; fj=fj->next){
			if(strcmp(fi->attribute, fj->attribute)==0)
				break;
		}
		if (fj==NULL)
			return FALSE;
		is_equ = feature_is_equal(fi->child, fj->child);
		if (is_equ==FALSE){
			return FALSE;
		}
	}
	for(fi=f2; fi!=NULL; fi=fi->next){
		for(fj=f1; fj!=NULL; fj=fj->next){
			if(strcmp(fi->attribute, fj->attribute)==0)
				break;
		}
		if (fj==NULL)
			return FALSE;
		is_equ = feature_is_equal(fi->child, fj->child);
		if (is_equ==FALSE){
			return FALSE;
		}
	}
	return is_equ;
}
void concat_subs(Dictionary dict,  Dict_node *dn){
	Feature *sub;
	char word_and_sub[MAX_WORD];
	for (;dn; dn = dn->left){
		sub = find_feature_path_string (dn->f, "root sub");
		if (sub!=NULL){
			if (strchr(dn->string, '.'))
				sprintf(word_and_sub, "%s%s", dn->string, sub->attribute);
			else
				sprintf(word_and_sub, "%s.%s", dn->string, sub->attribute);
			dn->string = string_set_add(word_and_sub, dict->string_set);
		}
	}


}
int different_in_features(Dictionary dict, Dict_node *dn){
	char new_word[MAX_WORD];
	Feature *sub;
	strcpy(new_word,dn->string);
	sub = find_feature_path_string (dn->f, "root sub");
	if (sub!=NULL){
		if (strchr(dn->string, '.'))
			sprintf(new_word, "%s%s", dn->string, sub->attribute);
		else
			sprintf(new_word, "%s.%s", dn->string, sub->attribute);
	}
	if (boolean_abridged_lookup(dict, new_word))
		return FALSE;
	dn->string = string_set_add(new_word, dict->string_set);
	return TRUE;
}
int feature_attrib_is_equal(Feature * f, char* str){
	return (strcmp(f->attribute, str)==0)&&(f->next==NULL) && (f->child==NULL);
}
int  unify_features(Feature * f1 , Feature * f2, Feature **unified_f){
	Feature *f3=NULL;
	Feature *fi, *fj, *fk;
	Feature_list *f1_parent, *f2_parent;
	int unifiable=TRUE, free_f1=TRUE, free_f2=TRUE;
	//preparation
	if((f1==NULL)&&(f2==NULL)){
		return TRUE;
	}
	if(f1==NULL){
		feature_free(f2);
		feature_free(f3);
		f3=NULL;
		return FALSE;
	}
	if(f2==NULL){
		feature_free(f1);
		feature_free(f3);
		f3=NULL;
		return FALSE;
	}
	f1_parent=f1->parent;
	f2_parent=f2->parent;
	for (fi=f1; fi!=NULL; fi=fi->next){
		fi->parent=NULL;
	}
	for (fi=f2; fi!=NULL; fi=fi->next){
		fi->parent=NULL;
	}

	//Obtaining the intersection of leaf nodes

	if (feature_is_equal(f1,f2)){
		f3=f1;
		free_f1=FALSE;
	}
	else if (feature_attrib_is_equal(f1,"?")){
		f3=f2;
		free_f2=FALSE;
	}
	else if(feature_attrib_is_equal(f2,"?")){
			free_f1=FALSE;
			f3=f1;
	}
	else if((f1->child==NULL)&& (f2->child==NULL)){
 		for (fi=f1; fi!=NULL; fi=fi->next){
			for (fj=f2; fj!=NULL; fj=fj->next){
				if (strcmp(fi->attribute ,fj->attribute)==0){
					fk = feature_create(&f3);
					fk->attribute = fi->attribute;
				}
			}
		}
		if(f3==NULL){
			unifiable=FALSE;
		}
	}
	else if ((f1->child != NULL) && (f2->child != NULL)){
		for (fi = f1 ; fi!=NULL; fi=fi->next){
			for (fj = f2 ; fj!=NULL; fj=fj->next){
				if (strcmp(fi->attribute ,fj->attribute)==0){
					break;
				}
			}
			fk = feature_create(&f3);
			fk->attribute=fi->attribute;
			fk->share_stat=fi->share_stat;
			if (fj!=NULL){
				unifiable=unify_features(fi->child, fj->child, NULL);
				replace_node(fj, fk);
			}
			else{
				fk->child = fi->child;
			}

			replace_node(fi, fk);
			if(unifiable==FALSE){
				break;
			}
		}
		if(unifiable==TRUE){
			for (fi = f2 ; fi!=NULL; fi=fi->next){
				for (fj = f1 ; fj!=NULL; fj=fj->next){
					if (strcmp(fi->attribute ,fj->attribute)==0){
						break;
					}
				}
				if (fj==NULL){
					fk = feature_create(&f3);
					fk->attribute=fi->attribute;
					fk->share_stat=fi->share_stat;
					replace_node(fi, fk);
				}
			}
		}

	}

	else{
		unifiable=FALSE;
	}
	add_to_parents(f3, f1_parent);
	add_to_parents(f3, f2_parent);
	if(f3!=NULL){
		for (fi=f3->next; fi!=NULL; fi=fi->next){
			fi->parent=f3->parent;
		}
	}
	else{
		feature_list_free_nodes(f1_parent);
	 	feature_list_free_nodes(f2_parent);
	}
	if (free_f1==TRUE)
		feature_free(f1);
	if (free_f2==TRUE)
		feature_free(f2);
	if(unifiable==FALSE){
		feature_free(f3);
		f3=NULL;
	}
	if (unified_f!=NULL){
		*unified_f=f3;
	}
	return unifiable;
}

int read_agreement(Dictionary dict, Feature_equ *equ ){
	FILE *fp;
	String_list *path;
	Feature *f, *fi;
	Feature_agreement *agr;
	Agr_stat *agr_stat;
	int first_agr=TRUE;
	fp = dict->feature_fp;
	if(equ->agr1_stat == AGR_NOT_INITED){
		agr=&(equ->agr1);
		agr_stat= &(equ->agr1_stat);
	}
	else{
		if(equ->agr2_stat == AGR_NOT_INITED){
			agr=&(equ->agr2);
			agr_stat= &(equ->agr2_stat);
		}
	}

	agr->path=NULL;

	if(string_is_equal(dict->feature_token,'<')) {
		for(;((dict->feature_token=feature_get_a_word(dict,fp))!=NULL) &&(!string_is_equal(dict->feature_token, '>'));){
			if(is_special(dict->feature_token)){
				feature_error(dict, "Expected a feature name ");
				return FALSE;
			}
			*agr_stat=AGR_PATH;
			path = string_list_create(&(agr->path));
			if ((first_agr==TRUE) && (strcmp(dict->feature_token, ROOT_FEATURE)!=0)){
				path->string = string_set_add(ROOT_FEATURE, dict->string_set);
				path = string_list_create(&(agr->path));
				first_agr = FALSE;
			}
			path->string = string_set_add(dict->feature_token, dict->string_set);

		}

		if(!string_is_equal(dict->feature_token,'>')) {
			feature_error(dict, "Expected \">\" ");
			return FALSE;
		}
		dict->feature_token=feature_get_a_word(dict,fp);
		return TRUE;
	}

	if(equ->agr1_stat==AGR_FEATURE){
		feature_error(dict, "Expected path ");
		return FALSE;
	}
	if(string_is_equal(dict->feature_token, '[')){
		if ((agr->f=feature_read_driver(dict))==NULL)
				return FALSE;
		*agr_stat=AGR_FEATURE;
		return TRUE;
	}
	if(is_special(dict->feature_token)){
		feature_error(dict, "Expected a feature name ");
		return FALSE;
	}
	if(equ->agr1_stat==AGR_FEATURE){
		feature_error(dict, "Expected path ");
		return FALSE;
	}


	f=agr->f=feature_node_create();
	f->attribute = string_set_add(ROOT_FEATURE, dict->string_set);
	f->child=NULL;
 	for (;!is_special(dict->feature_token); dict->feature_token=feature_get_a_word(dict,fp)){
		fi = feature_create(&(f->child));
		fi->attribute = string_set_add(dict->feature_token, dict->string_set);
	}
    set_feature_parents(f->child, f);
	*agr_stat=AGR_FEATURE;



	return TRUE;
}
int feature_conn_read(Dictionary dict, char * dict_path_name)
{
	FILE *fp;
	Feature_connector *fc=NULL;
	if ((fp = dictopen(dict_path_name, dict->feature_equ_file_name, "r")) == NULL) {
	lperror(NODICT, dict->feature_equ_file_name);
	return 0;
    }
	dict->feature_fp = fp;
	dict->current_file_name = dict->feature_equ_file_name;
	for(;(dict->feature_token=feature_get_a_word(dict,fp))!=NULL;)//for#1
	{
		if(is_special(dict->feature_token) || !is_link_name(dict->feature_token)){
			feature_error(dict, "Expected a connector name ");
			return FALSE;
		}
		if(feature_conn_set_lookup(dict->feature_token, dict->feature_conn_set)!=NULL) {
			feature_error(dict, "connector already defined!");
			return FALSE;
		}
		fc = feature_connector_create();
		fc->conn_name = string_set_add(dict->feature_token, dict->string_set);

		if ((dict->feature_token=feature_get_a_word(dict,fp))==NULL){
			return FALSE;
		}

		if(!string_is_equal(dict->feature_token,':')) {
			feature_error(dict, "Expected \":\" ");
			return FALSE;
		}
		for(;(!string_is_equal(dict->feature_token, ';'))&&((dict->feature_token=feature_get_a_word(dict,fp))!=NULL);){//for#2
			fc->equ = feature_equ_create(fc->equ);

			if (read_agreement(dict, fc->equ)==FALSE){
				return FALSE;
			}

			if(!string_is_equal(dict->feature_token,'=')) {
				feature_error(dict, "Expected \"=\" ");
				return FALSE;
			}

			if ((dict->feature_token=feature_get_a_word(dict,fp))==NULL){
				return FALSE;
			}

			if(read_agreement(dict, fc->equ)==FALSE){
				return FALSE;
			}

			if((!string_is_equal(dict->feature_token,','))&&((!string_is_equal(dict->feature_token,';')))) {
				feature_error(dict, "Expected \",\" ");
				return FALSE;
			}

		}//for#2
		if ((dict->feature_token==NULL) || (!string_is_equal(dict->feature_token,';'))) {
			feature_error(dict, "Expected \";\" ");
			return FALSE;
		}
		feature_conn_set_add(fc, dict->feature_conn_set);

	}//for#1
fclose(fp);
return TRUE;
}
void initialize_feature_array(Parse_info *pi){
	int i;
	for (i=0; i<pi->N_words ;i++){
			pi->feature_array[i]=NULL;
		}
}

void unify_sentence(Sentence sent, Parse_Options opts) {

    int *indices, *unified_list;
    int in;
    int N_linkages_found;
    int N_linkages_unified, N_valid_linkages;
    int overflowed;
    Linkage_info *link_info;
    link_info = sent->link_info;
    N_linkages_found = sent->num_linkages_found;
    N_valid_linkages = 0;

	overflowed = (sent->num_linkages_found<0);
    indices = (int *) xalloc(N_linkages_found * sizeof(int));
    if (overflowed) {
	for (in=0; in<N_linkages_found; in++) {
	    indices[in] = -(in+1);
	}
    }
    else {
		for (in=0; in<N_linkages_found; in++) {
			indices[in] = in;
		}
    }

	if(!opts->unify_features){
		sent->unification_info = (int *) xalloc(sent->num_linkages_found * sizeof(int));
		for(in=0;in<N_linkages_found; in++){
			sent->unification_info[in] = indices[in];
		}
		xfree(indices, N_linkages_found * sizeof(int));
		return;
	}
    unified_list = (int *) xalloc(N_linkages_found * sizeof(int));
	N_linkages_unified = N_valid_linkages = 0;
    for (in=0; (in < N_linkages_found) &&
	       (!resources_exhausted(opts->resources)); in++) {
               extract_links(indices[in], sent->null_count, sent->parse_info);
			   if (unify_linkage(sent)){
				   unified_list[N_valid_linkages++]=indices[in];
			   }
		   }


	sent->num_linkages_found = N_valid_linkages;
	sent->unification_info = (int *) xalloc(sent->num_linkages_found * sizeof(int));
	for(in=0;in<N_valid_linkages; in++){
		sent->unification_info[in] = unified_list[in];
	}
    xfree(indices, N_linkages_found * sizeof(int));
    xfree(unified_list, N_linkages_found * sizeof(int));
}
int unify_linkage(Sentence sent){
	char  upper_link_name[MAX_CONNECTOR_NAME];
	int i;
	int unifiable;
	Feature *f1;
	Feature *f2;
	Feature *f3;
	Feature_connector *fc;
	Feature_equ *equ_i;
	Parse_info *pi;
	struct Link_s *link_array;
	pi = sent->parse_info;
	link_array = pi->link_array;
	compute_link_names(sent);
	for (i=0; i<pi->N_links;i++){
		extract_link_name(upper_link_name, link_array[i].name);
		if ((fc = feature_conn_set_lookup(link_array[i].name,sent->dict->feature_conn_set))==NULL){
			if ((fc = feature_conn_set_lookup(upper_link_name, sent->dict->feature_conn_set))==NULL){
					continue;
				}
			}
		for(equ_i=fc->equ; equ_i!=NULL; equ_i=equ_i->next){
			if ((equ_i->agr1_stat==AGR_PATH) && (equ_i->agr2_stat==AGR_FEATURE)){
				f1=find_feature_path(equ_i->agr1.path,pi->feature_array[link_array[i].l]);
				if(f1==NULL){
					return 0;
				}
				//if (!feature_attrib_is_equal(f1,"?")){
				//	return FALSE;
				//}

				f3 = feature_copy_driver(USE_SOURCE_STRING_SET, equ_i->agr2.f);
				unifiable = unify_features(f1, f3->child, NULL);
				delete_from_childs(f3);
				xfree(f3,sizeof(Feature));
				if (unifiable == FALSE)
					return FALSE;
			}
			else{
				if ((equ_i->agr1_stat==AGR_FEATURE) && (equ_i->agr2_stat==AGR_PATH)){
					f2=find_feature_path(equ_i->agr2.path,pi->feature_array[link_array[i].r]);
					if(f2==NULL){
						return 0;
					}
					//if (!feature_attrib_is_equal(f2,"?")){
					//	return FALSE;
					//}
					f3 = feature_copy_driver(USE_SOURCE_STRING_SET, equ_i->agr1.f);
					//set_new_node(f3, f2->child->parent);
					unifiable = unify_features(f2, f3->child, NULL);
					delete_from_childs(f3);
					xfree(f3,sizeof(Feature));
					if (unifiable == FALSE)
						return FALSE;
				}
				else{
					if ((equ_i->agr1_stat==AGR_PATH) && (equ_i->agr2_stat==AGR_PATH)){
						f1=find_feature_path(equ_i->agr1.path,pi->feature_array[link_array[i].l]);
	 					if(f1==NULL){
							return 0;
						}
						f2=find_feature_path(equ_i->agr2.path,pi->feature_array[link_array[i].r]);
						if(f1==NULL){
							return 0;
						}
						if ((unify_features(f1, f2, NULL))==FALSE){
							return FALSE;
						}

					}
				}
			}

		}
	}
	return TRUE;
}
void compute_feature_array(Parse_info *pi){
	int i;
	free_feature_array(pi);
	for (i=0;i<pi->N_words; i++)
	{
		pi->feature_array[i]=feature_copy_driver(USE_SOURCE_STRING_SET, pi->chosen_disjuncts[i]->feature);
	}

}
void print_a_feature(Feature *f, char *fp){

	if (f==NULL){
		return;
	}
	if(strcmp(f->attribute,ROOT_FEATURE)!=0)
		strcat(fp, f->attribute);
	if (f->child!=NULL){
		if(strcmp(f->attribute,ROOT_FEATURE)!=0)
			strcat(fp, ": ");
		if (f->child->child!=NULL){
			strcat(fp, "[ ");
			print_a_feature(f->child, fp);
			strcat(fp, " ]");
		}
		else{
			if (f->child->next!=NULL){
				strcat(fp, "{");
				print_a_feature(f->child,fp);
				strcat(fp, "}");
			}
			else{
				print_a_feature(f->child, fp);
			}
		}
	}
	if(f->next!=NULL){
		strcat(fp, " , ");
		print_a_feature(f->next, fp);
	}
}
char * print_features(Linkage linkage){
	int i;
	char *fp;
	char temp[MAX_LINE]="";
	fp=(char*)exalloc(sizeof(char)*1);
	fp[0]='\0';
	fp = strappend(fp, "\n Features:\n");
	for (i=0;i<linkage->num_words; i++){
		sprintf(temp,"\n\n %s:\n ", linkage->word[i]);
		fp = strappend(fp,temp);
		temp[0]='\0';
		print_a_feature(linkage->feature_array[i],temp);
		fp = strappend(fp,temp);
	}
	fp = strappend(fp,"\n");
	return fp;
}

/***********************
External usage functions
************************/
void set_ex_feature_attrib(Feature *f){
	Feature *fi;
	char *attrib;
	for(fi=f; fi!=NULL; fi=fi->next){
		attrib = fi->attribute;
		fi->attribute = (char*)exalloc(sizeof(char)*(strlen(attrib)+1));
		strcpy(fi->attribute, attrib);
		set_ex_feature_attrib(fi->child);
	}
}
Feature * ex_feature_copy_driver(String_set *ss, Feature *f){
	return feature_copy_driver (ss, f);
}
void exfeature_list_free_nodes(Feature_list * fl){
	if(fl==NULL)
		return;
	exfeature_list_free_nodes(fl->next);
	exfree((char*)fl, sizeof(Feature_list));

}

void exfeature_free(Feature * f) {
 	feature_free(f);
}
/***********************/
