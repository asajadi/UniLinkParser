/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/
#include "link-includes.h"
#define BAD_WORD (MAX_SENTENCE+1)
#define STREMPTY(str) *str==END_STR
//#define INVALID_CONN "(){};[]&|:"
char   maperrmsg[1024];

void mapping_error(Transfer trans, char * msg, char *t ) {
    char tokens[5];
	strncpy(tokens, t, 4);
	tokens[4]='\0';
	sprintf(maperrmsg, "\nMapping error (Line %d): %s, tokens: %s\n", trans->map_line_num, msg, tokens);
}



/***********************************************
*	string utilites
*
************************************************/
static int is_number(char *s) {
    /* returns TRUE if the string s is a sequence of digits. */
    while(*s != '\0') {
	if (!isdigit((int)*s)) return FALSE;
	s++;
    }
    return TRUE;
}
char *  itoa ( int value, char * str){
	sprintf(str, "%d", value);
	return str;
}
char * exstrcpy(char * src){
	char * trg = exalloc(strlen(src)+1);
	strcpy(trg, src);
	return trg;

}
char* strcpy_to_delim(char *trg, char * src, int c){
	char *pos;
	pos=strchr(src,c);
	if (pos!=NULL){
		strncpy(trg,src, pos-src);
		trg[pos-src]='\0';
	}
	else{
		strcpy(trg,src);
	}
	return trg;
}
char* strcpy_from_delim(char *trg, char * src, int c){
	char *pos;
	pos=strchr(src,c);
	if (pos!=NULL){
		strcpy(trg,pos);
	}
	else{
		trg[0]='\0';
	}
	return trg;
}
char *strappend(char *s, char*t){
	char * temp_str;
	temp_str=s;
	s=(char*)exalloc(sizeof(char)*(strlen(s)+strlen(t)+1));
	sprintf(s,"%s%s",temp_str,t);
	string_delete(temp_str);
	return s;
}

int  get_conn_delim(char *connector_name){
	char *cdelim_pos_1, *cdelim_pos_2;
	int cdelim;
	cdelim_pos_1 = strchr(connector_name, '.');
	cdelim_pos_2 = strchr(connector_name, '[');
	if (!cdelim_pos_1 && !cdelim_pos_2)
        cdelim = '\0';
	else if (!cdelim_pos_1 && cdelim_pos_2)
		cdelim = *cdelim_pos_2;
	else if (!cdelim_pos_2 && cdelim_pos_1)
		cdelim = *cdelim_pos_1;
	else{
		cdelim = (cdelim_pos_1 < cdelim_pos_2) ? *cdelim_pos_1 : *cdelim_pos_2;
	}
	return cdelim;
}
char * get_next_dis(Transfer trans, char* word, char * individual_dis_str, char * s){
	char *wpos, *dpos;
	if ((wpos=strchr(s,'('))==NULL){
		mapping_error(trans,"expecting a \"(\"", s);
		return NULL;
	}
	strncpy(word,s, wpos-s);
	word[wpos-s]='\0';
	if ((dpos=strchr(wpos,')'))==NULL){
		mapping_error(trans,"expecting a \")\"", s);
		return NULL;
	}
	if ((dpos=strchr(dpos,'('))==NULL){
		mapping_error(trans,"expecting a \"(\"", s);
		return NULL;
	}

	if ((dpos=strchr(dpos,')'))==NULL){
		mapping_error(trans,"expecting a \")\"", s);
		return NULL;
	}
	strncpy(individual_dis_str, wpos, dpos-wpos+1);
	individual_dis_str[dpos-wpos+1]='\0';
	if (*(dpos+1)==',')
		dpos++;
	return dpos+1;
}
char * get_next_conn(Transfer trans, char* cname, char * individual_dis_str){
	char *pos, *pos1, *pos2;
	pos1=strchr(individual_dis_str, ',');
	pos2=strchr(individual_dis_str, ')');
	pos = ((pos1<pos2) && (pos1!=NULL)) ? pos1 : pos2;
	strncpy(cname,individual_dis_str, pos-individual_dis_str);
	cname[pos-individual_dis_str]='\0';
	if(conn_name_is_valid(cname)==FALSE){
		mapping_error(trans,"Invalid connector name", cname);
		return NULL;
	}
	if (*pos==',')
		pos++;
	return pos;
}
void num_list_append(char * tar, char * s,int multi){
	int pos = strlen(tar);
	if (pos && !STREMPTY(s)){
		if (multi == 1)	pos--;
		tar[pos++] = ',';
	}
	if (multi == 0)
		tar[pos++] = '[';
	while (*s != '\0')
			tar[pos++] = *s++;
	if (multi != -1)
		tar[pos++] = ']';
	tar[pos] = '\0';
}
int num_list_get_index(char * num_list, int index, int multi_index){
	char  temp[MAX_LINK_NAME];
	char * s;
	s = num_list;
	while (--index){
		if (*s == OPEN_BRACKET)
			while(!STREMPTY(s) && *s++ != CLOSE_BRACKET) ;
		while(!STREMPTY(s) && *s++ != VIRGULE);
	}
	if (*s == OPEN_BRACKET) s++;
	while (multi_index--)
		while(!STREMPTY(s) && *s != CLOSE_BRACKET && *s++ != VIRGULE);
	while(!STREMPTY(s) && *s != VIRGULE && *s != CLOSE_BRACKET)
		temp[index++] = *s++;
	temp[index] = END_STR;

	if (!index)
		return BAD_WORD;
	index = atoi(temp);
	return index;
}
int num_list_find_index(char * num_list, int num){
	char  num_str[3];
	char * s, *pos1, *pos2, *pos;
	int delim;
	int index;
	s = num_list;
	for (index = 1;*s!='\0';index++){
		if (*s=='['){
			for (s++;*s!=']'; ){
				pos1 = strchr(s, ',');
				pos2 = strchr(s, ']');
				pos = ((pos1<pos2) && (pos1!=NULL)) ? pos1 : pos2;
				delim = *pos;
				strcpy_to_delim(num_str, s, delim);
				if (num == atoi(num_str))
					return index;
				s = ((pos1<pos2) && (pos1!=NULL)) ? pos1+1 : pos2;
			}
			s++;
			if (*s!='\0')
				s++;
		}
		else{
			pos = strchr(s, ',');
			if (pos==NULL){
				pos = strchr(s, '\0');
				strcpy_to_delim(num_str, s, '\0');
				s=pos;
			}
			else{
				strcpy_to_delim(num_str, s, ',');
				s = pos+1;
			}
			if (num == atoi(num_str))
				return index;

		}
	}
	return -1;
}

/***********************************************
Lexicon Functions
***********************************************/
Lexicon * lexicon_create(void) {
    Lexicon *lex;
    int i;
    lex = (Lexicon *) exalloc(sizeof(Lexicon));
    lex->size = next_prime_up(100);
	lex->table = (Lex_entry **) exalloc(lex->size * sizeof(Lex_entry *));
    lex->count = 0;
    for (i=0; i<lex->size; i++) lex->table[i] = NULL;
    return lex;
}

int lex_entry_find_place(char * str, Lexicon *lex) {
    /* lookup the given string in the table.  Returns a pointer
       to the place it is located, or the place where it should be. */
    int h, s, i;
	h = gen_hash_string((unsigned char *) str, lex->size);
	s = gen_stride_hash_string((unsigned char *) str, lex->size);
    for (i=h; 1; i = (i + s)%(lex->size)) {
		if ((lex->table[i] == NULL) || (strcmp(lex->table[i]->src, str) == 0)) return i;
    }
}

void lex_entry_table_grow(Lexicon *lex) {
    Lexicon old;
    int i, p;

    old = *lex;
    lex->size = gen_next_prime_up(2 * old.size);  /* at least double the size */
    lex->table = (Lex_entry **) exalloc(lex->size * sizeof(Lex_entry *));
    lex->count = 0;
    for (i=0; i<lex->size; i++) lex->table[i] = NULL;
    for (i=0; i<old.size; i++) {
	if (old.table[i] != NULL) {
		p = lex_entry_find_place(old.table[i]->src, lex);
	    lex->table[p] = old.table[i];
	    lex->count++;
	}
    }
    /*printf("growing from %d to %d\n", old.size, ss->size);*/
    fflush(stdout);
	exfree((char *) old.table, old.size * sizeof(Lex_entry *));
}

Lex_entry * lexicon_add(Lex_entry * source, Lexicon * lex) {
    int p;

	//assert(source->name != NULL, "STRING_SET: Can't insert a null string");

	p = lex_entry_find_place(source->src, lex);
	if (lex->table[p] != NULL)
		return lex->table[p];


    lex->table[p] = source;
    lex->count++;

	if ((4 * lex->count) > (3 * lex->size)) lex_entry_table_grow(lex);

    return source;
}
Lex_entry * lexicon_lookup(char * source, Lexicon * lex) {
    int p;

	p = lex_entry_find_place(source, lex);
    return lex->table[p];
}
void lex_entry_delete(Lex_entry *lex_entry){
	int i;
	if (lex_entry != NULL){
		string_delete(lex_entry->src);
		for (i=0; i<lex_entry->num; i++)
			string_delete(lex_entry->trg[i]);
		exfree(lex_entry, sizeof(Lex_entry));
	}

}
void lexicon_delete(Lexicon *lex) {
    int i;

    if (lex == NULL) return;
    for (i=0; i<lex->size; i++)
		lex_entry_delete(lex->table[i]);
	exfree(lex->table, lex->size * sizeof(Lex_entry *));
    exfree(lex, sizeof(Lexicon));
}
int find_lex(Lexicon *lexicon, char * trg, char * src){
	Lex_entry *lex;
	lex=lexicon_lookup(src, lexicon);
	if(lex==NULL){
		strcpy(trg, src);
		return FALSE;
	}
	strcpy(trg, lex->trg[lex->num-1]);
	return TRUE;
}
int find_lex_ith(Lexicon *lexicon, char * trg, char * src, int i){
	Lex_entry *lex;
	lex=lexicon_lookup(src, lexicon);
	if(lex==NULL){
		strcpy(trg, src);
		return FALSE;
	}
	if (i<lex->num)
		strcpy(trg, lex->trg[i]);
	else
		trg[0] = '\0';
	return TRUE;
}
Lex_entry * lex_entry_create(char * src, char *trg, int line_num) {

	Lex_entry *lex_entry;
	lex_entry = (Lex_entry *)exalloc(sizeof(Lex_entry));
	lex_entry->num=0;
	lex_entry->src = exstrcpy(src);
	lex_entry->trg[lex_entry->num++] = exstrcpy(trg);
	lex_entry->line_number = line_num;
	return lex_entry;
}
Lex_entry * lexicon_add_entry(char * src, char *trg, int line_num, Lexicon * lex) {
	Lex_entry *lex_entry;
	int i;
	if ((lex_entry = lexicon_lookup(src, lex))!=NULL){
		for (i=0; i<lex_entry->num && strcmp(lex_entry->trg[i], trg); i++);
		if (i==lex_entry->num){
			lex_entry->trg[lex_entry->num++] = exstrcpy(trg);
		}
		return lex_entry;
	}

	lex_entry = lex_entry_create (src, trg, line_num);
	//lex_entry = (Lex_entry *)exalloc(sizeof(Lex_entry));
	//lex_entry->num=0;
	//lex_entry->src = exstrcpy(src);
	//lex_entry->trg[lex_entry->num++] = exstrcpy(trg);
	//lex_entry->line_number = line_num;

	lexicon_add(lex_entry, lex);
	return lex_entry;
}
Lex_entry * lex_entry_copy(Lex_entry * src_le) {
	Lex_entry *trg_le;
	int i;
	if (src_le==NULL)
		return NULL;
	trg_le = (Lex_entry *)exalloc(sizeof(Lex_entry));
	trg_le->num=src_le->num;
	trg_le->line_number = src_le->line_number;
	trg_le->src = exstrcpy(src_le->src);
	for (i=0; i<src_le->num; i++){
		trg_le->trg[i] = exstrcpy(src_le->trg[i]);
	}
	return trg_le;
}
Lexicon * read_lexicon(Transfer trans, char * lex_file_name){
	Lexicon *lexicon;
	int line_num=1;
	char c=0;
	char src[MAX_DIS_STR], trg[MAX_DIS_STR];
	int i;
	FILE * fp;
	if ((fp=dictopen(lex_file_name, lex_file_name, "r"))==NULL){
		mapping_error(trans,"Unable to open file",lex_file_name);
		return NULL;
	}
	lexicon=lexicon_create();
	for(c=fgetc(fp); c!=EOF;){
		trans->map_line_num = line_num;
		i=0;
		for(; c!=':' && c!=EOF; c=fgetc(fp)){
			if (!isspace(c))
				src[i++]=c;
		}
		if(c==EOF){
			mapping_error(trans,"expecting a \":\"", src);
			return NULL;
		}
		src[i]='\0';
		i=0;
		for(c=fgetc(fp); c!=';' && c!=EOF; c=fgetc(fp)){
			if (!isspace(c))
				trg[i++]=c;
		}
		if(c==EOF){
			mapping_error(trans,"expecting a \";\"", src);
			return NULL;
		}
		trg[i]='\0';

		c=fgetc(fp);//pass the ";"

		lexicon_add_entry(src, trg, line_num, lexicon);
		for(c=fgetc(fp); isspace(c); c=fgetc(fp));
		line_num++;
	}
	fclose(fp);
	return lexicon;
}



/***********************************************
*	free extrenal memory in use
************************************************/
void exfree_disjuncts(Disjunct *c) {
/* free the list of disjuncts pointed to by c
   (DOES  free the strings)
*/
    Disjunct *c1;
    for (;c != NULL; c = c1) {
	c1 = c->next;
	exfree_connectors(c->left);
	exfree_connectors(c->right);
	exfree((char *)c, sizeof(Disjunct));
    }
}

void free_c_table(C_table ** c_tabel, int size){
	int i;
	C_table * ct, *nct = NULL;

	for(i = 0; i< size; i++){
		ct = c_tabel[i];
		while (ct != NULL){
			nct = ct->next;
			exfree(ct, sizeof(C_table));
			ct = nct;
		}
	}
	exfree(c_tabel, size*sizeof(C_table*));
}

/***********************************************
*	t_word_array utilites
*
************************************************/
T_word_array * init_t_word_array(int num_words){
	T_word_array * t_word_array;
	int i;
	t_word_array = (T_word_array *) exalloc(sizeof(T_word_array));
	t_word_array->elements = (T_word **) exalloc(MAX_LINKS*sizeof(T_word*));
	for(i = 0; i < MAX_LINKS; i++)
		t_word_array->elements[i]= NULL;
	t_word_array->sent_no = 0;
	t_word_array->new_word_t_word_no = t_word_array->last_t_word_no = num_words;
	return t_word_array;
}
void new_t_word(T_word_array *t_word_array, int t_word_no, char * string){

	t_word_array->elements[t_word_no] = (T_word *)exalloc(sizeof(T_word));
	t_word_array->elements[t_word_no]->marked = BAD_WORD;
	t_word_array->elements[t_word_no]->no_word = BAD_WORD;
	t_word_array->elements[t_word_no]->string = exstrcpy(string);
	t_word_array->elements[t_word_no]->left = NULL;
	t_word_array->elements[t_word_no]->right = NULL;
	t_word_array->elements[t_word_no]->f = NULL;
	if (t_word_no==t_word_array->last_t_word_no)
		t_word_array->last_t_word_no++;
}
T_word * copy_t_word(T_word * d) {
	T_word * d1;
    if (d == NULL) return NULL;
	d1 = (T_word *) exalloc(sizeof(Disjunct));
    *d1 = *d;
	d1->string = exstrcpy(d->string);
	d1->f = ex_feature_copy_driver(USE_SOURCE_STRING_SET, d->f);
    d1->left = excopy_connectors(d->left);
    d1->right = excopy_connectors(d->right);
    return d1;
}

void free_t_word(T_word *t_word){
	if (t_word==NULL)
		return;
	exfree_connectors(t_word->left);
	exfree_connectors(t_word->right);
	exfeature_free(t_word->f);
	string_delete(t_word->string);
	exfree(t_word, sizeof(T_word));
}

void free_t_word_array(T_word_array *t_word_array){
	int i;
	if (t_word_array==NULL)
		return;
	for(i = 0; i < t_word_array->last_t_word_no; i++){
		free_t_word(t_word_array->elements[i]);
		t_word_array->elements[i] = NULL;
	}
	exfree(t_word_array->elements, MAX_LINKS*sizeof(T_word *));
	exfree(t_word_array, sizeof(T_word_array));
}
void delete_t_word(T_word *dn){
	//Disables dn, it may be freed by free_t_word in some other routines.
	exfree_connectors(dn->left);
	exfree_connectors(dn->right);
	dn->left = dn->right = NULL;
}
int t_word_exists(T_word *dn){
	return (dn && ((dn->left!=NULL)||(dn->right!=NULL)));
}

int get_t_word_no(T_word_array *t_word_array, char * string, int start, int end){
	int t_word_no = BAD_WORD;
	while (start < end && t_word_no == BAD_WORD){
		if (strcmp(t_word_array->elements[start]->string, string) == 0)
			t_word_no = start;
		else start++;
	}
	if(start==end)
		return -1;
	return t_word_no;
}

/***********************************************
*	Connector utilites
*
************************************************/
Connector * exnew_connector(int word, char * string){
	Connector * c;
	c = init_connector((Connector *) exalloc(sizeof(Connector)));
	c->string = exstrcpy(string);
	c->multi = 0;
	c->label = NORMAL_LABEL;
	c->priority = THIN_priority;
	c->word = word;
	c->next = NULL;
	return c;
}
Connector * excopy_connector(Connector *c){
    Connector *c1;
    if (c == NULL) return NULL;
    c1 = init_connector((Connector *) exalloc(sizeof(Connector)));
    *c1 = *c;
	c1->string = exstrcpy (c->string);
    c1->next = copy_connectors(c->next);
    return c1;
}
Connector * copy_connector_to_sentence(Sentence sent, Connector *c){
    Connector *c1;
    if (c == NULL) return NULL;
    c1 = init_connector((Connector *) xalloc(sizeof(Connector)));
    *c1 = *c;
	c1->string = string_set_add(c->string, sent->string_set);
	c1->next = copy_connector_to_sentence(sent, c->next);
    return c1;

}
int get_next_neighbor(Transfer trans, int word_no, int neighb_no, char dir){
	int new_num;
	Lol_list *lol_list;
	if (dir == '+')
		lol_list = trans->word_lols [word_no]->right;
	else
		lol_list = trans->word_lols [word_no]->left;
	new_num = num_list_get_index (trans->dis_str_array[word_no]->num_list, neighb_no, 0);
	return new_num;
}

int get_next_neighbor_in_name(Transfer trans, int word_no, char* neighb, char dir){
	int i, order_num;
	Lol_list *lol_list;
	char link_name[MAX_LINK_NAME]="", order[MAX_LINK_NAME]="", *pos;
	char new_link_name[MAX_LINK_NAME]="";
	i=0;


	pos=strchr(neighb,'_');
	if (pos!=NULL){
		strncpy(link_name,neighb, pos-neighb);
		link_name[pos-neighb]='\0';
		strcpy(order, pos+1);
	}
	else
		strcpy(link_name, neighb);


	if (*order=='\0')
		order_num = 1;
	else if (!is_number(order)){
		mapping_error(trans, "Bad link-order", order);
		return BAD_WORD;
	}else
		order_num = atoi(order);
	if (dir == '+')
		lol_list = trans->word_lols [word_no]->right;
	else
		lol_list = trans->word_lols [word_no]->left;
	for (i=0; lol_list!=NULL; lol_list = lol_list->next){
		find_lex(trans->links_list, new_link_name, lol_list->name);
		if (new_link_name[strlen(new_link_name)-1]=='%')
			new_link_name[strlen(new_link_name)-1] = '\0';
		if (strcmp(new_link_name, link_name)==0){
			i++;
			if (i==order_num)
				break;
		}
	}
	if (!lol_list){
		return BAD_WORD;
	}
	return lol_list ->lol->word;
}
int multiple_links(Transfer trans, int t_word_no, int word_no){
	Connector * c;
	if (t_word_no == word_no)
		return TRUE;
	for (c= trans->t_word_array->elements[t_word_no]->left; c!=NULL; c= c->next)
		if (c->word == word_no)
			return TRUE;
	for (c= trans->t_word_array->elements[t_word_no]->right; c!=NULL; c= c->next)
		if (c->word == word_no)
			return TRUE;
	return FALSE;
}

Connector * make_connectors(Transfer trans,int  t_word_no, int added_t_word_no, char * connector, char * num_list, char * nested_num_list){
	T_word_array *t_word_array=trans->t_word_array;
	int word_no = BAD_WORD, multi = 0;
	char *s, *t, *pos, dir,  num_str[4];
	char c_name[MAX_LINK_NAME], temp[MAX_LINK_NAME], new_word[MAX_WORD];
	Connector * c, *c1 = NULL;
	s = connector;
	if (*s == ATSIGN)
		{s++; multi = 1;}
	for(t = s; (*t != POINT && *t != OPEN_BRACKET && *t != END_STR); t++);
	strncpy(c_name, s, t-s);
	c_name[t-s] = END_STR;
	s = t;
	if (*s == POINT){
		s++;
		dir = s [0];
		if ((*s=='+')||(*s=='-'))
			s++;
		t = strchr(s, '*');
		if (t){
			strncpy (temp, s, t-s);
			temp [t-s ] = 0;
			s = temp;
		}
		if(*s == '#'){
			s++;
			if (!is_number(s)){
				mapping_error(trans, "Invalid word number",s );
				return NULL;
			}
			word_no = atoi(s);
			word_no = num_list_get_index(nested_num_list, word_no, 0);
			if (word_no==BAD_WORD){
				mapping_error(trans, "Invalid word number",s );
				return NULL;
			}
		}
		else{
			if (!is_number(s)){
				if (conn_name_is_valid(s)){
					word_no = get_next_neighbor_in_name(trans, t_word_no, s,dir);
					if (word_no==BAD_WORD){
						mapping_error(trans, "Invalid word number",s );
						return NULL;
					}
				}
				else{
					mapping_error(trans, "Invalid word  number or name",s );
					return NULL;
				}
			}
			else{
				word_no = atoi(s);
				word_no = num_list_get_index(num_list, word_no, 0);
				if (word_no==BAD_WORD){
					mapping_error(trans, "Invalid word number",s );
					return NULL;
				}
			}
		}
		while ((t!=NULL) && (*t== '*')){
			s = ++t;
			s++;
			dir = t [0];
			t = strchr(s, '*');

			if (t) {
				strncpy (temp, s, t-s);
				temp [t-s ] = 0;
				s = temp;
			}
			if (!is_number(s)){
				if (conn_name_is_valid(s)){
					word_no = get_next_neighbor_in_name(trans, word_no, s,dir);
					if (word_no==BAD_WORD){
						mapping_error(trans, "Invalid word number",s );
						return NULL;
					}
				}
				else{
					mapping_error(trans, "Invalid word number",s );
					return NULL;
				}
			}
			else{
				word_no = get_next_neighbor(trans, word_no, atoi(s), dir);
				if (word_no==BAD_WORD){
					mapping_error(trans, "Invalid word number",s );
					return NULL;
				}
				if (word_no == -1)
					return NULL;
			}

		}

	}
	else if(*s == OPEN_BRACKET){
			if((pos=strchr(s,']'))==NULL){
				mapping_error(trans,"expecting a \"]\"", s);
				return NULL;
			}
			strncpy(new_word, s+1, pos-s-1);
			new_word[pos-s-1]='\0';
			word_no = get_t_word_no(t_word_array, new_word, added_t_word_no, t_word_array->last_t_word_no);
			if (word_no<0){
				word_no = t_word_array->last_t_word_no;
				new_t_word(t_word_array, word_no, new_word);
			}

	}

	if (multiple_links(trans, t_word_no, word_no)){
		mapping_error(trans,"Multiple links on the same word or cycle for disjunct: ", itoa(t_word_no, num_str));
		return NULL;
	}
	c = exnew_connector(word_no, c_name);
	if (multi){
		word_no = atoi(s);
		word_no = num_list_get_index(num_list, word_no, multi++);
		while (word_no != BAD_WORD){
			c1 = exnew_connector(word_no, c_name);
			c1->next = c;
			c = c1;
			word_no = atoi(s);
			word_no = num_list_get_index(num_list, word_no, multi++);
		}
	}
	return c;
}
Connector * append_connector(Connector * list, Connector * ac){
	Connector * c1;
	c1 = list;
	if (c1==NULL){
		return ac;
	}
	while(c1->next != NULL)
		c1 = c1->next;
	c1->next = ac;
	return list;
}
void  unmark_connector(Connector * c, int word){

	c->word = word;
	c->length_limit = UNLIMITED_LEN;
}

int add_disunct_connectors(Transfer trans, int t_word_no,int added_t_word_no, char * con_list, char * num_list, char * nested_num){
	char *s;
	char cname[MAX_LINK_NAME];
	Connector * c,*cl, *ci;
	T_word_array *t_word_array=trans->t_word_array;
	s = con_list;
	s++;
	while(*s != ')'){
		if ((s=get_next_conn(trans, cname, s))==NULL){
			return FALSE;
		}
		c = make_connectors(trans, t_word_no,  added_t_word_no, cname, num_list, nested_num);
		if (c==NULL)
			return FALSE;
		for (ci=c ;ci->next!=NULL; ci = ci->next);
		ci->next = t_word_array->elements[t_word_no]->left;
		t_word_array->elements[t_word_no]->left = c;

	}
	s = s + 2;
	while(!STREMPTY(s) && *s != ')'){
		if ((s=get_next_conn(trans, cname, s))==NULL){
			return FALSE;
		}
		c = make_connectors(trans, t_word_no, added_t_word_no, cname, num_list, nested_num);
		if (c==NULL)
			return FALSE;
		c = reverse(c);
		for (ci=c ;ci->next!=NULL; ci = ci->next);
		if (t_word_array->elements[t_word_no]->right==NULL){
			t_word_array->elements[t_word_no]->right = c;
			cl = ci;
			continue;
		}
		cl->next = c;
		cl = ci;
	}
	return TRUE;
}


/***********************************************
*	Some Functions  to elminate duplicate t_word_array nodes
************************************************/
int t_word_cmp(T_word_array *t_word_array, int i, int j){
	T_word ** elements = t_word_array->elements;
	Connector *ci, *cj;
	int dir;
	int is_equal=TRUE;;
	if (i==j)
		return TRUE;
	if (XOR((elements[i]->marked == BAD_WORD),(elements[j]->marked == BAD_WORD)))
		return FALSE;
	if((elements[i]->marked != BAD_WORD) && (elements[j]->marked != BAD_WORD)){
		if((elements[i]->marked ==j) && (elements[j]->marked == i))
            return TRUE;
		else
			return FALSE;
	}
	if (strcmp(elements[i]->string, elements[j]->string)!=0)
		return FALSE;
	elements[i]->marked = j;
	elements[j]->marked = i;
	for (dir=LEFT; is_equal!=FALSE && dir<=RIGHT; dir++){
		if (dir==LEFT){
			ci=elements[i]->left;
			cj=elements[j]->left;
		}
		else{
			ci=elements[i]->right;
			cj=elements[j]->right;
		}
		while(TRUE){
			if (!ci && !cj){
				is_equal = is_equal && TRUE;
				break;
			}
			if (XOR(ci,cj))
				return FALSE;
			if (strcmp(ci->string, cj->string)!=0)
				return FALSE;
		 	if (t_word_cmp(t_word_array, ci->word, cj->word)==FALSE)
				return FALSE;
			ci=ci->next;
			cj=cj->next;
		}
	}
	return is_equal;

}
void replace_duplicate_t_words(T_word_array *t_word_array, T_word *t_word, int added_t_word_no){
	T_word ** elements = t_word_array->elements;
	Connector *ci, *cj;
	int i, j, dir;
	for (j=t_word_array->new_word_t_word_no; j<t_word_array->last_t_word_no; j++)
		elements[j]->marked = BAD_WORD;

	for (dir=LEFT; dir<=RIGHT; dir++){
		if (dir==LEFT)
			ci=t_word->left;
		else
			ci=t_word->right;
		for(; ci!=NULL; ci=ci->next){
			if((ci->word < added_t_word_no)
				||(elements[ci->word]->marked!=BAD_WORD))
				continue;
			for(i=t_word_array->new_word_t_word_no; i<added_t_word_no; i++){
				if(elements[i]->marked!=BAD_WORD)
					continue;
				if (t_word_cmp(t_word_array, ci->word, i)==TRUE){
					for (cj=ci; cj!=NULL; cj=cj->next){
						if ((elements[cj->word]!=NULL)&& (elements[cj->word]->marked!=BAD_WORD)){
							cj->word = elements[cj->word]->marked;
						}
					}
					for (j=added_t_word_no; j<t_word_array->last_t_word_no; j++)
						if (elements[j]->marked != BAD_WORD){
							delete_t_word(elements[j]);
						}
				}
				else{
					for (j=added_t_word_no; j<t_word_array->last_t_word_no; j++)
						elements[j]->marked = BAD_WORD;
				}
				for (j=t_word_array->new_word_t_word_no; j<added_t_word_no; j++)
					elements[j]->marked = BAD_WORD;

			}
		}

	}
}

char correct_map(char * s, char * m){
	if (strcmp(s, m) == 0)
		return TRUE;
	return FALSE;
}

/***********************************************
*	C_tables utilites
************************************************/
int length_list(Connector * c){
	Connector * c1;
	int count = 0;

	c1 = c;
	while(c1 != NULL){
		count++;
		c1 = c1->next;
	}
	return count;
}

int connector_count(T_word_array *t_word_array){
	int i, left_count = 0, right_count = 0;

	for (i = 0; i<t_word_array->last_t_word_no; i++){
		left_count += length_list(t_word_array->elements[i]->left);
		right_count += length_list(t_word_array->elements[i]->right);
	}
	return MAX(left_count, right_count);
}

void init_c_tables(Transfer trans, int size){
	int i;

	trans->table_size = next_power_of_two_up(size);
	trans->lc_table = (C_table **) exalloc(trans->table_size * sizeof(C_table *));
	trans->rc_table = (C_table **) exalloc(trans->table_size * sizeof(C_table *));

	for (i = 0; i< trans->table_size; i++){
		trans->lc_table[i] = NULL;
		trans->rc_table[i] = NULL;
	}
}
C_table * add_c_table(C_table * ct, int t_word_no, Connector * c){
	C_table * new_node;

	new_node = (C_table *) exalloc(sizeof(C_table));
	new_node->t_word_no = t_word_no;
	new_node->c = c;
	new_node->list_no = -1;
	new_node->next = ct;
	return new_node;
}

int connector_left_link(Transfer trans, int hash_val, int t_word_no, char * con_name){
	int word_no = BAD_WORD;
	C_table * ct;
	Connector * c;

	ct = trans->lc_table[hash_val];
	while (ct != NULL){
		c = ct->c;
		if (c != NULL && c->word == t_word_no && strcmp(c->string, con_name) == 0)
			word_no = ct->t_word_no;
		ct = ct->next;
	}
	return word_no;
}
void fill_connector_tables(Transfer trans){
	int i, h, word_no;
	Connector * c;

	for (i = 0; i<trans->t_word_array->last_t_word_no; i++){
		c = trans->t_word_array->elements[i]->left;
		while (c != NULL){
			h = power_hash(c) & (trans->table_size -1);
			trans->lc_table[h] = add_c_table(trans->lc_table[h], i, c);
			c = c->next;
		}
		c = trans->t_word_array->elements[i]->right;
		while (c != NULL){
			h = power_hash(c) & (trans->table_size -1);
			word_no = c->word;
			if (word_no == BAD_WORD)
				word_no = connector_left_link(trans, h, i, c->string);
			trans->rc_table[h] = add_c_table(trans->rc_table[h], i, c);
			c = c->next;
		}
	}
}
void build_connector_tables(Transfer trans){

	init_c_tables(trans, connector_count(trans->t_word_array));
	fill_connector_tables(trans);
}
/***********************************************
*	map_linkage utilites
************************************************/
void extract_num_list(char * dis_str, char * num_list){
	int pos;
	pos = strchr(dis_str, '(')-dis_str ;
	strncpy(num_list, dis_str, pos);
	num_list[pos]=END_STR;

	strcpy(dis_str, dis_str+pos);
	return;
}
char * map_disjunct(Transfer trans, char * trg_dis_str, int src_t_word_no, int i){
	//dis_map=lexicon_lookup(src_dis_str, trans->dis_map_set);
	//if(dis_map==NULL){
	//	strcpy(trg_dis_str, src_dis_str);
	//	return trg_dis_str;
	//}

	if ((i<trans->dis_str_array[src_t_word_no]->dis_map->num) && (trans->dis_str_array[src_t_word_no]->dis_map->trg[i])){
		if(strcmp(trans->dis_str_array[src_t_word_no]->dis_map->src, EMPTY_DIS)==0){
			strcpy(trg_dis_str, EMPTY_DIS);
			return trg_dis_str;
		}
		else{
			strcpy(trg_dis_str, trans->dis_str_array[src_t_word_no]->dis_map->trg[i]);
			trans->map_line_num = trans->dis_str_array[src_t_word_no]->dis_map->line_number;
		}
	}
	else
		trg_dis_str[0] = '\0';


	return trg_dis_str;
}
void map_links_name(Lexicon *link_map,Lexicon *new_link_map , char * dis_str){
	char new_dis_str[MAX_DIS_STR] = "";
	char connector_name[MAX_CONNECTOR_NAME], at_sign[2] = "";
	char link_name[MAX_CONNECTOR_NAME], new_link_name[MAX_CONNECTOR_NAME];
	char temp_str[MAX_DIS_STR];
	char *pos;
	int len,j, tr, cdelim, recover;

	for (pos=dis_str;*pos!='\0';){
		strcpy_to_delim(temp_str, pos, '(');
		len=strlen(temp_str);
		strcat(new_dis_str, temp_str);
		pos = len+pos;
		for (tr=0; tr<2; tr++){
			len = strlen(new_dis_str);
			new_dis_str[len++] = *pos++;
			new_dis_str[len] = '\0';
			for (; ; ){
				for(j=0; *pos!=')' && *pos!=',' && *pos; pos++, j++)
					connector_name[j]=*pos;
				connector_name[j] = '\0';
				if (connector_name[0]!='\0'){
					at_sign[0] = '\0';
					if (connector_name[0] == '@'){
						strcpy(at_sign, "@");
						strcpy(connector_name, connector_name+1);
					}
					cdelim = get_conn_delim(connector_name);
					strcpy_to_delim(link_name, connector_name, cdelim);
					strcpy_from_delim(temp_str, connector_name, cdelim);
					find_lex(link_map, new_link_name, link_name);
					if ((recover = (new_link_name[strlen(new_link_name)-1]!='%'))==FALSE)
						new_link_name[strlen(new_link_name)-1] = '\0';
					strcat(new_dis_str, at_sign);
					strcat(new_dis_str, new_link_name);
					strcat(new_dis_str, temp_str);
					if (recover && (new_link_map!=NULL) && (strcmp(new_link_name, link_name)!=0)){
						lexicon_add_entry(new_link_name, link_name, -1, new_link_map);
					}
				}
				len = strlen(new_dis_str);
				new_dis_str[len++] = *pos++;
				new_dis_str[len] = '\0';
				if (*(pos-1)==')')
					break;
			}
		}
	}
	strcpy(dis_str, new_dis_str);

}
int map_and_build_dis_str(Transfer trans, int t_word_no, char * string){
	T_word_array *t_word_array;
	char dis_str [MAX_DIS_STR];
	char *s;
	char word[MAX_WORD], individual_dis_str[MAX_DIS], num_list[MAX_NUM_LIST], num_str[4];
	int new_t_word_no, added_t_word_no;
	char nested_num_list[MAX_NUM_LIST]="";
	int i, j;
	int done;
	t_word_array = trans->t_word_array;
	new_t_word_no=t_word_no;
	added_t_word_no = trans->t_word_array->last_t_word_no;
	for (i=trans->dis_str_array[t_word_no]->current_map+1; ;i++){
		done = TRUE;
		//<Re-Entrance>
		free_t_word(t_word_array->elements[t_word_no]);
		t_word_array->elements[t_word_no] = NULL;
		for (j=added_t_word_no; j<trans->t_word_array->last_t_word_no; j++){
			free_t_word(t_word_array->elements[j]);
			t_word_array->elements[j] = NULL;
		}
		new_t_word_no = t_word_no;
		trans->t_word_array->last_t_word_no = added_t_word_no;
		//</Re-Entrance>
 		map_disjunct(trans, dis_str, t_word_no, i);
		if (*dis_str == '\0'){
			done = FALSE;
			break;
		}
		strcpy(num_list, trans->dis_str_array[t_word_no]->num_list);
		map_links_name(trans->dis_str_array[t_word_no]->links_map, NULL, dis_str);
		if(dis_str==NULL){
			mapping_error(trans,"Unable to map disjunct", string);
			done = FALSE;
			continue;
		}
		s = dis_str;
		if((s=get_next_dis(trans, word, individual_dis_str, s))==NULL){
			done = FALSE;
			continue;
		}
		new_t_word(t_word_array, new_t_word_no, string);
		if(add_disunct_connectors(trans, new_t_word_no, added_t_word_no, individual_dis_str, num_list, nested_num_list)==FALSE){
			done = FALSE;
			continue;
		}
		num_list_append(nested_num_list, itoa(new_t_word_no, num_str),-1);
		while((STREMPTY(s))!=TRUE){
			if((s=get_next_dis(trans, word, individual_dis_str, s))==NULL){
				done = FALSE;
				continue;
			}
			if ((new_t_word_no = get_t_word_no(t_word_array, word, added_t_word_no, t_word_array->last_t_word_no))<0){
				mapping_error(trans,"Undefined new word", word);
				done = FALSE;
				continue;
			}


			if (add_disunct_connectors(trans, new_t_word_no, added_t_word_no, individual_dis_str, num_list, nested_num_list)==FALSE)
				done = FALSE;
			num_list_append(nested_num_list, itoa(new_t_word_no, num_str),-1);

		}
		if (done == TRUE)
			break;
		else{
			string_delete(trans->dis_str_array[t_word_no]->dis_map->trg[i]);
			trans->dis_str_array[t_word_no]->dis_map->trg[i]=NULL;
		}

	}
	if (done){
		trans->dis_str_array[t_word_no]->current_map = i;
		replace_duplicate_t_words(t_word_array, t_word_array->elements[t_word_no], added_t_word_no);
	}

	return done;
}
List_o_links * reverse_lol(List_o_links * e){
	List_o_links * head, *x;
	head = NULL;
    while (e != NULL) {
	x = e->next;
	e->next = head;
	head = e;
	e = x;
    }
    return head;
}
int con_cmp(char *src, char* trg){
 for(;*src!='\0' && isupper(*src) && *trg!='\0' && isupper(*trg) && *src==*trg; src++, trg++);
 if (!isupper(*src) && !isupper(*trg)) return 0;
 else return 1;
}
int add_multi_connector(char * dis_str, char * con_name){
	char  last_con[MAX_CONNECTOR_NAME];
	int ret, end;
	ret = end = strlen(dis_str);
	while ((--ret)>=0){
		if (dis_str[ret] == ATSIGN){
			 strncpy(last_con, dis_str+ret+1, end-ret-1);
			 last_con[end-ret-1]='\0';
			if (con_cmp(last_con, con_name) == 0)
				return 1;
			return 0;
		}else if(dis_str[ret] == VIRGULE ||dis_str[ret] == L_PAR)
				return 0;
		else if (dis_str[ret] == POINT)
			end = ret;
	}
	return 0;
}
int add_connector_str(char * dis_str, int multi, char * con_name, int num){
	int end = -1;
	char num_str[4];

	if (multi)
		end = add_multi_connector(dis_str, con_name);
	if(end == 1)	return end;

	if (!STREMPTY(dis_str))
		strcat(dis_str, ",");
	if(!end)
			strcat(dis_str, "@");
	sprintf(dis_str,"%s%s.%s", dis_str, con_name, itoa(num, num_str));

	return end;
}
void  build_lols (Transfer trans, Linkage linkage){
	List_o_links * lol, *lol_i;
	Link * ppla;
	Sublinkage *sublinkage=&(linkage->sublinkage[linkage->current]);
	List_o_links *word_lol;
	int word_no;
	Lol_list *left=NULL, *right=NULL, *list_i, *pre_node, *lol_node;
	ppla = sublinkage->link;
	trans->word_lols = (Word_lols ** ) exalloc (linkage->num_words * sizeof (Word_lols *));
	for(word_no = 0; word_no < trans->num_words; word_no++){
		left = right = NULL;
		word_lol = sublinkage->pp_data.word_links[word_no];
		lol_i=lol = word_lol;
		for(lol_i=word_lol; lol_i!=NULL;lol_i=lol_i->next ){
			if (lol_i->word < word_no){
				for (list_i=left, pre_node=NULL; list_i!=NULL && list_i->lol->word > lol_i->word; pre_node = list_i, list_i=list_i->next);
				lol_node = (Lol_list *)exalloc(sizeof(Lol_list));
				lol_node->lol = lol_i;
				lol_node->name = exstrcpy(ppla[lol_i->link]->name);
				lol_node->next = list_i;
				if (pre_node!=NULL)
					pre_node->next = lol_node;
				else
					left = lol_node;
			}
			else{
				for (list_i=right, pre_node=NULL; list_i!=NULL && list_i->lol->word > lol_i->word; pre_node = list_i, list_i=list_i->next);
				lol_node = (Lol_list *)exalloc(sizeof(Lol_list));
				lol_node->lol = lol_i;
				lol_node->name = exstrcpy(ppla[lol_i->link]->name);
				lol_node->next = list_i;
				if (pre_node!=NULL)
					pre_node->next = lol_node;
				else
					right = lol_node;
			}
		}
		trans->word_lols [word_no] = (Word_lols * ) exalloc (sizeof (Word_lols ));
		trans->word_lols[word_no]->left = left;
		trans->word_lols[word_no]->right = right;
	}
}
void exfree_lols (Transfer trans){
	Lol_list *left=NULL, *right=NULL, *list_i, *lol_node;
	int i;
	for (i=0; i<trans->num_words; i++ ){
		left = trans->word_lols[i]->left;
		for (list_i=left; list_i!=NULL;){
			lol_node = list_i;
			list_i = list_i->next;
			string_delete(lol_node->name);
			exfree(lol_node, sizeof(Lol_list));
		}
		right = trans->word_lols[i]->right;
		for (list_i=right; list_i!=NULL;){
			lol_node = list_i;
			list_i = list_i->next;
			string_delete(lol_node->name);
			exfree(lol_node, sizeof(Lol_list));
		}
		exfree (trans->word_lols[i], sizeof (Word_lols));
	}
	exfree (trans->word_lols, trans->num_words * sizeof (Word_lols *));

}
char * word_dis_str(Transfer trans, char * trg_dis_str, Link * ppla, int word_no){
	List_o_links *lol_i;
	int multi, link, num = 1;
	char  dis_str1[MAX_DIS_STR]="", num_list[MAX_DIS_STR]="",dis_str2[MAX_DIS_STR]="", num_str[4]="";
	char link_name[MAX_CONNECTOR_NAME];
	Lol_list *left=NULL, *right=NULL, *list_i;

	left =  trans->word_lols[word_no]->left;
	right =  trans->word_lols[word_no]->right;
	num_list[0] = END_STR;
	for (list_i=left; list_i!=NULL; list_i = list_i->next){
		lol_i = list_i ->lol;
		link = lol_i->link;
		find_lex(trans->links_list, link_name, ppla[link]->name);
		if(strcmp(link_name, "NONE")!=0){
			multi = add_connector_str(dis_str1, ppla[link]->rc->multi, ppla[link]->name, num);
			num_list_append(num_list, itoa(ppla[link]->l, num_str), multi);
			if (multi != 1)	num++;
		}

	}

	for (list_i=right; list_i!=NULL; list_i = list_i->next){
		lol_i = list_i ->lol;
		link = lol_i->link;
		find_lex(trans->links_list, link_name, ppla[link]->name);
		if(strcmp(link_name, "NONE")!=0){
			multi = add_connector_str(dis_str2, ppla[link]->lc->multi, ppla[link]->name, num);
			num_list_append(num_list, itoa(ppla[link]->r, num_str), multi);
			if (multi != 1)	num++;
		}
	}

	sprintf(trg_dis_str, "%s(%s)(%s)", num_list, dis_str1, dis_str2);
	return trg_dis_str;
}
int extract_dis_strs(Transfer trans, Linkage linkage){
	Link * ppla;
	int i;
	char dis_str[MAX_DIS_STR];
	Lex_entry *lex_entry;
	Sublinkage *sublinkage=&(linkage->sublinkage[linkage->current]);
	ppla = sublinkage->link;

	for(i = 0; i < trans->num_words; i++){
		word_dis_str(trans, dis_str,ppla, i);
		extract_num_list(dis_str, trans->dis_str_array[i]->num_list);
		map_links_name(trans->links_list, trans->dis_str_array[i]->links_map, dis_str);
		lex_entry = lexicon_lookup(dis_str, trans->dis_map_set);

		//First methof to handle unfound disjuncts
		//if (lex_entry==NULL){
		//	itoa(i, num_str, 10);
		//	mapping_error(trans,"Unable to map disjunct, No",  num_str);
		//	return FALSE;
		//}
		//Second methof to handle unfound disjuncts
		if (lex_entry==NULL)
			trans->dis_str_array[i]->dis_map = lex_entry_create (dis_str, dis_str, -1);
		else
			trans->dis_str_array[i]->dis_map = lex_entry_copy(lex_entry);
		translate_word(trans->lexicon, trans->dis_str_array[i]->target_word, linkage->word[i], linkage->feature_array[i]);
	}
	return TRUE;
}
Connector * remove_link(Sentence sent, int index, Connector * c1){
	Connector * c;
	char found = FALSE;
	c = c1;

	while (c != NULL && !found)
		if (c->word == index)
			found = TRUE;
		else c = c->next;
	while (c != NULL){
		c->length_limit = 2;
		c = c->next;
	}
	return c1;
}
char LEFT_WALL(Connector * c){
	char leftWall = TRUE;
	Connector * c1 = c;

	while (c1 != NULL){
		if (c1->length_limit != 2)
			leftWall = FALSE;
		c1 = c1->next;
	}
	return leftWall;
}
void unmark_t_words(T_word_array *t_word_array	){
	int i;
	for (i=0; i<t_word_array->last_t_word_no; i++){
		t_word_array->elements[i]->marked = FALSE;
		t_word_array->elements[i]->no_word = BAD_WORD;
	}
}
void reset_undefined_words(Transfer trans){
	int i;
	for (i=0; i<trans->list_size; i++){
		trans->lc_list[i]->c->word = BAD_WORD;
		trans->rc_list[i]->c->word = BAD_WORD;
	}
}
int find_first_t_word(T_word_array *t_word_array, int t_word_no){
	int first_t_word;
	Connector *flc, *frc;
	T_word *t_word = t_word_array->elements[t_word_no];
	t_word->marked = TRUE;
	if (t_word_exists(t_word)==FALSE)
		return -1;
	flc = t_word->left;
	frc = t_word->right;
	first_t_word = -1;
	if ((frc!=NULL) && (t_word_array->elements[frc->word]->marked==FALSE))
		first_t_word = find_first_t_word(t_word_array, frc->word);
	if (first_t_word !=-1 )
		return first_t_word;
	if (flc == NULL){
		first_t_word = t_word_no;
		return first_t_word;
	}
	if (t_word_array->elements[flc->word]->marked == FALSE)
			first_t_word = find_first_t_word(t_word_array, flc->word);
	return first_t_word;
}
int find_first_t_word_driver(Transfer trans){
	int first_t_word;
	int starting_t_word;
	for (starting_t_word=0;starting_t_word < trans->t_word_array->last_t_word_no
		&& !t_word_exists(trans->t_word_array->elements[starting_t_word]) ; starting_t_word++);
	if(starting_t_word >=trans->t_word_array->last_t_word_no)
		return -1;
	unmark_t_words(trans->t_word_array);
	first_t_word = find_first_t_word(trans->t_word_array, starting_t_word);
	return first_t_word;

}
void number_t_words(Transfer trans,  int t_word_no){
	T_word * t_word;
	T_word_array * t_word_array;
	Connector *c;

	t_word_array = trans->t_word_array;
	t_word = t_word_array->elements[t_word_no];

	t_word->marked = TRUE;
	for (c = t_word->left; c!=NULL; c=c->next){
		if (t_word_array->elements[c->word]->marked!=TRUE)
			number_t_words(trans, c->word);
	}
	t_word->no_word = t_word_array->sent_no++;
	for (c = t_word->right; c!=NULL; c=c->next){
		if (t_word_array->elements[c->word]->marked==FALSE)
			number_t_words(trans, c->word);
	}
}
void reverse_right_connectors(Transfer trans){
	int i;
	T_word ** elements = trans->t_word_array->elements;
	for (i=0; i<trans->t_word_array->last_t_word_no; i++){
		elements[i]->right = reverse(elements[i]->right);
	}

}
int number_t_words_driver(Transfer trans){
	int first_t_word;
	if ((first_t_word= find_first_t_word_driver(trans))<0)
		return FALSE;
	unmark_t_words(trans->t_word_array);
	reverse_right_connectors (trans);
	trans->t_word_array->sent_no = 0;
	number_t_words(trans, first_t_word);
	reverse_right_connectors (trans);
	return TRUE;
}
int reorder_t_words_and_connectors(Transfer trans){
	int i, j;
	int valid = TRUE;
	T_word ** elements;
	Connector *c;
	if (number_t_words_driver(trans)==FALSE)
		return FALSE;
	elements = trans->ordered_t_word_array->elements;
	for (i=0; i<trans->t_word_array->last_t_word_no; i++){
		j = trans->t_word_array->elements[i]->no_word;
		if (j!=BAD_WORD){
			elements[j] = copy_t_word (trans->t_word_array->elements[i]);
			trans->ordered_t_word_array->last_t_word_no++;
			for (c = elements[j]->left; c!=NULL; c = c->next){
				c->word = trans->t_word_array->elements[c->word]->no_word;
			}
			for (c = elements[j]->right; c!=NULL; c = c->next){
				c->word = trans->t_word_array->elements[c->word]->no_word;
			}
		}
		else{
			if ((trans->t_word_array->elements[i]->left!=NULL)
				||(trans->t_word_array->elements[i]->right!=NULL))
				valid = FALSE;

		}
	}


	return valid;
}
int t_words_connected (Transfer trans, int lw, int rw, Connector *lwrc, Connector *rwlc){
	if ((lwrc==NULL) || (rwlc ==NULL))
		return FALSE;
	return 	(lwrc->word ==rw) && (rwlc->word == lw) && (strcmp (lwrc->string, rwlc->string)==0);
}
int validate_linkage(Transfer trans, int lw, int rw, Connector *lwrc, Connector *rwlc){
	int w;
	int valid = TRUE;
	T_word **t_words = trans->ordered_t_word_array->elements;
	if (lw == rw-1){
		if ((lwrc==NULL) && (rwlc==NULL))
			return TRUE;
		else
			return FALSE;
	}
	if (lwrc!=NULL){
		w = lwrc->word;
		if ((w<=lw)||(w>=rw))
			return FALSE;
		if (t_words_connected (trans, lw, w, lwrc, t_words[w]->left)==TRUE)
			valid = valid && validate_linkage (trans, lw, w, lwrc->next, t_words[w]->left->next);
		else
			return FALSE;
		if (valid == TRUE){
			if (t_words_connected(trans, w, rw, t_words[w]->right, rwlc))
				valid = valid && validate_linkage (trans, w, rw, t_words[w]->right->next, rwlc->next);
			else
				valid = valid && validate_linkage (trans, w, rw, t_words[w]->right, rwlc);
		}
	}
	else if (rwlc!=NULL){
		w = rwlc->word;
		if ((w<=lw)||(w>=rw))
			return FALSE;
		if (t_words_connected (trans, w, rw, t_words[w]->right, rwlc)==TRUE)
			valid = valid && validate_linkage (trans, w, rw, t_words[w]->right->next, rwlc->next);
		else
			return FALSE;
		if (valid == TRUE)
			valid = valid && validate_linkage (trans, lw, w, lwrc, t_words[w]->left);

	}
	else
		return FALSE;
	return valid;
}
int validate_linkage_driver(Transfer trans){
	T_word** t_words;
	int n ;
	t_words = trans->ordered_t_word_array->elements;
	n = trans->ordered_t_word_array ->last_t_word_no;
	if ((t_words[0]->left!=NULL) || (t_words[n-1]->right!=NULL)){
		mapping_error(trans, "start end word not validated", "");
		return FALSE;
	}
	if (t_words_connected(trans, 0, n-1, t_words[0]->right, t_words[n-1]->left)){
		if (validate_linkage (trans, 0, n-1, t_words[0]->right->next, t_words[n-1]->left->next)==FALSE){
			mapping_error(trans, "Linkage not validated", "");
			return FALSE;
		}
	}
	else{
		if (validate_linkage (trans, 0, n-1, t_words[0]->right, t_words[n-1]->left)==FALSE){
			mapping_error(trans, "Linkage not validated", "");
			return FALSE;
		}
	}
	return TRUE;

}
Disjunct * get_word_disjunct(Transfer trans,  Sentence sent, int t_word_no){
	Disjunct * dis;
	dis = (Disjunct *)xalloc(sizeof(Disjunct));
	dis->left = NULL;
	dis->right = NULL;
	dis->left = copy_connector_to_sentence( sent, trans->ordered_t_word_array->elements[t_word_no]->left);
	dis->right = copy_connector_to_sentence( sent, trans->ordered_t_word_array->elements[t_word_no]->right);
	dis->cost = 0;
	dis->marked = FALSE;
	dis->next = NULL;
	dis->string =  string_set_add(trans->ordered_t_word_array->elements[t_word_no]->string, sent->string_set);
	dis->feature = feature_copy_driver(sent->string_set, trans->ordered_t_word_array->elements[t_word_no]->f);
	return dis;
}
X_node * build_xnode(Transfer trans, Sentence sent, int t_word_no){
	X_node *x;
	x = (X_node *) xalloc(sizeof(X_node));
	x->exp = NULL;
	x->exp = NULL;
	x->string = sent->word[t_word_no].d->string;
	x->feature = sent->word[t_word_no].d->feature;;
	x->next=NULL;
	return x;
}
void add_sentence_disjuncts(Transfer trans, Sentence sent){
	int sent_word, length=0;
	int i ;
	for (i=0; i<trans->ordered_t_word_array->last_t_word_no; i++){
		sent_word = trans->ordered_t_word_array->elements[i]->no_word;
		sent->word[i].d = get_word_disjunct(trans, sent, i);
		sent->word[i].x = build_xnode(trans, sent, i);
		strcpy(sent->word[i].string, sent->word[i].d->string);
		sent->word[sent_word].firstupper = 0;
		length++;

	}
	sent->length = length;
}
int MIN_sent_word(T_word_array *t_word_array){
	int i, min = BAD_WORD;

	for(i = 0; i < t_word_array->last_t_word_no; i++)
		min = MIN(t_word_array->elements[i]->no_word, min);
	return min;
}
Sentence my_sentence_create(Dictionary dict) {
    Sentence sent;

    free_lookup_list();

    sent = (Sentence) xalloc(sizeof(struct Sentence_s));
    sent->dict = dict;
    sent->length = 0;
    sent->num_linkages_found = 0;
    sent->num_linkages_alloced = 0;
    sent->num_linkages_post_processed = 0;
    sent->num_valid_linkages = 0;
    sent->link_info = NULL;
    sent->deletable = NULL;
    sent->effective_dist = NULL;
    sent->num_valid_linkages = 0;
    sent->null_count = 0;
    sent->parse_info = NULL;
    sent->string_set = string_set_create();


	return sent;
}
void sentence_generate(Transfer trans){
	int temp;
	trans->sent[trans->target_no++]=my_sentence_create(trans->target_dict);
	add_sentence_disjuncts(trans, trans->sent[trans->target_no-1] );
	trans->sent[trans->target_no-1]->q_pruned_rules = FALSE; /* for post processing */
	trans->sent[trans->target_no-1]->is_conjunction = (char *) xalloc(sizeof(char)*trans->sent[trans->target_no-1]->length);
	set_is_conjunction(trans->sent[trans->target_no-1]);
	initialize_conjunction_tables(trans->sent[trans->target_no-1]);
	temp = my_sentence_parse(trans->sent[trans->target_no-1], trans->opts);
	if (temp==0)
		printf("d");
}
int build_linkage(Transfer trans){
	int	ret=TRUE;
	trans->ordered_t_word_array = init_t_word_array(0);
	if (reorder_t_words_and_connectors (trans) ==FALSE){
		mapping_error(trans, "Ordering  failed", "");
		ret = FALSE;
	}
	if (ret){
		if (validate_linkage_driver(trans)==TRUE){
			sentence_generate(trans);
			ret = TRUE;
		}
	}
	free_t_word_array (trans->ordered_t_word_array);
	trans->ordered_t_word_array = NULL;
	return ret;
}

//Second pararmeter? I don't remember,
Linkage trans_linkage_create(Transfer trans, int i, Sentence sent, Parse_Options opts){
	Linkage linkage;
	linkage = linkage_create(i, sent, opts);
	morphological_synthesizer(linkage, trans->target_dict, trans->lexicon);
	return linkage;

}

Transfer transfer_create(char* map_file_name, char* links_list_file, char* lexicon, char* trg_dict_name){
	Transfer trans;
	trans = (Transfer)exalloc(sizeof(struct Transfer_s));
	trans->links_list=NULL;
	strcpy(trans->lex_file_name, lexicon);
	if((trans->lexicon = read_lexicon(trans, lexicon))==NULL){
		return NULL;
	}
	strcpy(trans->map_file_name, map_file_name);
 	if((trans->dis_map_set = read_lexicon(trans, map_file_name))==NULL){
		return NULL;
	}

	strcpy(trans->links_list_file, links_list_file);
	if((trans->links_list = read_lexicon(trans, links_list_file))==NULL){
		return NULL;
	}


	//trans->match_head = NULL;
	trans->lc_list = trans->rc_list = NULL;
	trans->list_size = 0;
	trans->uncertain_no = 0;
	trans->target_no = 0;
	trans->t_word_array=NULL;
	trans->ordered_t_word_array=NULL;
	trans->dis_str_array = NULL;
	trans->lc_table=trans->rc_table=NULL;
	trans->table_size=0;
	trans->target_dict  = dictionary_create(trg_dict_name, NULL, NULL, NULL, NULL);
	if (trans->target_dict == NULL)
		return NULL;
	trans->string_set = string_set_create();
	trans->opts  = parse_options_create();
	parse_options_set_unify_features(trans->opts,FALSE);
	parse_options_set_display_walls (trans->opts, FALSE);
	trans->num_linkages = 0;
	trans->num_words = 0;
	trans->word_lols = NULL;
	return trans;

};
Dis_str_array ** init_dis_str_array(Transfer trans){

	Dis_str_array **dis_str_array ;
	int i;
	dis_str_array = (Dis_str_array **) exalloc(trans->num_words * sizeof(Dis_str_array *));
	for (i=0; i< trans->num_words; i++){
		dis_str_array[i] = (Dis_str_array *) exalloc(sizeof(Dis_str_array));
		dis_str_array[i]->links_map = lexicon_create();
		dis_str_array[i]->dis_map = NULL;
		dis_str_array[i]->current_map = -1;
	}
	return dis_str_array;
}
void free_dis_str_array(Dis_str_array ** dis_str_array, int num){
	int i;
	if (dis_str_array == NULL)
		return;
	for (i=0; i< num; i++){
		if (dis_str_array[i]!=NULL){
			lexicon_delete( dis_str_array[i]->links_map);
			lex_entry_delete (dis_str_array[i]->dis_map);
			exfree(dis_str_array[i], sizeof(Dis_str_array ));
		}
	}
	exfree(dis_str_array, num*sizeof(Dis_str_array *));

}
void delete_sentences(Transfer trans){
	int i;
	for (i=0; i<trans->target_no; i++){
		sentence_delete (trans->sent[i]);
	}
	trans->target_no = 0;
}
void prepare_to_transfer(Transfer trans,  Linkage linkage){


	free_c_table(trans->rc_table, trans->table_size);
	free_c_table(trans->lc_table, trans->table_size);
	free_t_word_array(trans->t_word_array);
	free_dis_str_array(trans->dis_str_array, trans->num_words);
	exfree(trans->lc_list, trans->list_size*sizeof(C_table *));
	exfree(trans->rc_list, trans->list_size*sizeof(C_table *));
	//match_list_delete(&(trans->match_head));
	delete_sentences(trans);
    //parse_options_delete(trans->opts);
	//dictionary_delete(trans->target_dict);
	//string_set_delete(trans->string_set);
	//lexicon_delete(trans->dis_map_set);
	//lexicon_delete(trans->lexicon);
	//lexicon_delete(trans->links_list);
	exfree_lols (trans);
	//exfree(trans, sizeof(struct Transfer_s));

	//transfer_create
	//trans->match_head = NULL;
	trans->lc_list = trans->rc_list = NULL;
	trans->list_size = 0;
	trans->uncertain_no = 0;
	trans->target_no = 0;
	trans->t_word_array=NULL;
	trans->dis_str_array = NULL;
	trans->lc_table=trans->rc_table=NULL;
	trans->table_size=0;
	//trans->target_dict  = dictionary_create(dict_param->dict_name, dict_param->pp_name, dict_param->cons_name, dict_param->affix_name, dict_param->morpho_dict_name);
	//if (trans->target_dict == NULL)
	//	return NULL;
	//trans->string_set = string_set_create();
	//trans->opts  = parse_options_create();
	//trans->opts->unify_features = FALSE;
	//trans->num_linkages = 0;
	//trans->num_words = 0;
	trans->word_lols = NULL;

	trans->t_word_array = init_t_word_array(linkage->num_words);
	trans->num_words = linkage->num_words;
	trans->dis_str_array = init_dis_str_array(trans);
	maperrmsg[0] = '\0';


}
void transfer_delete(Transfer trans){
	free_c_table(trans->rc_table, trans->table_size);
	free_c_table(trans->lc_table, trans->table_size);
	free_t_word_array(trans->t_word_array);
	free_dis_str_array(trans->dis_str_array, trans->num_words);
	exfree(trans->lc_list, trans->list_size*sizeof(C_table *));
	exfree(trans->rc_list, trans->list_size*sizeof(C_table *));
	//match_list_delete(&(trans->match_head));
//	string_list_ex_free(trans->links_list);
//	string_list_ex_free(trans->sub_sensitive_links_list);
	delete_sentences(trans);
    parse_options_delete(trans->opts);
	dictionary_delete(trans->target_dict);
	string_set_delete(trans->string_set);
	lexicon_delete(trans->dis_map_set);
	lexicon_delete(trans->lexicon);
	lexicon_delete(trans->links_list);
	exfree_lols (trans);
	exfree(trans, sizeof(struct Transfer_s));

}

void cleanups_for_transfer(Transfer trans){
	/*
	free_c_table(trans->rc_table, trans->table_size);
	free_c_table(trans->lc_table, trans->table_size);
	free_t_word_array(trans->t_word_array);
	free_dis_str_array(trans->dis_str_array, trans->num_words);
	exfree(trans->lc_list, trans->list_size*sizeof(C_table *));
	exfree(trans->rc_list, trans->list_size*sizeof(C_table *));
	match_list_delete(&(trans->match_head));
//	string_list_ex_free(trans->links_list);
//	string_list_ex_free(trans->sub_sensitive_links_list);
	delete_sentences(trans);
    parse_options_delete(trans->opts);
	dictionary_delete(trans->target_dict);
	string_set_delete(trans->string_set);
	lexicon_delete(trans->dis_map_set);
	lexicon_delete(trans->lexicon);
	lexicon_delete(trans->links_list);
	exfree_lols (trans);
	exfree(trans, sizeof(struct Transfer_s));
	*/
	// Prepare to re-parse
	free_c_table(trans->rc_table, trans->table_size);
	free_c_table(trans->lc_table, trans->table_size);
	trans->table_size = 0;
	exfree(trans->lc_list, trans->list_size*sizeof(C_table *));
	trans->list_size = 0;
	exfree(trans->rc_list, trans->list_size*sizeof(C_table *));
	//match_list_delete(&(trans->match_head));

	//free_t_word_array(trans->t_word_array);
	//trans->t_word_array=NULL;
	//trans->t_word_array = init_t_word_array(trans->num_words);
	//trans->t_word_array->new_word_t_word_no = trans->t_word_array->last_t_word_no =NULL;

	//	trans->num_words = linkage->num_words;
	//free_dis_str_array(trans->dis_str_array, trans->num_words);
	//trans->dis_str_array = NULL;
	//trans->dis_str_array = init_dis_str_array(trans);
	//delete_sentences (trans);

}

int transfer_linkage(Transfer trans, Linkage linkage, int word_no){
	int j;
	int mapped;
	int added_t_word_no;
	int ret = FALSE;
	if (word_no < trans->num_words){
		trans->dis_str_array[word_no]->current_map = -1;
		added_t_word_no = trans->t_word_array->last_t_word_no;
		while (TRUE){
			mapped = map_and_build_dis_str(trans, word_no, trans->dis_str_array[word_no]->target_word);
			if (mapped==FALSE){
				if (trans->dis_str_array[word_no]->current_map == -1)
					return FAIL;
				else
					break;
			}
			trans->t_word_array->elements[word_no]->f = ex_feature_copy_driver(trans->string_set, linkage->feature_array[word_no]);
			ret = transfer_linkage(trans, linkage, word_no+1);
			/* Important Rollback*/
			for (j=added_t_word_no; j<trans->t_word_array->last_t_word_no; j++){
				free_t_word(trans->t_word_array->elements[j]);
			 	trans->t_word_array->elements[j] = NULL;
			}
			trans->t_word_array->last_t_word_no = added_t_word_no;
			/* Important Rollback*/
			if (ret==FAIL)
				return FAIL;
		}
	}
	else{
		cleanups_for_transfer(trans);
		build_connector_tables(trans);
		/*
		if (init_lists(trans)==FALSE)
			return FALSE;

		if(trans->match_head!=NULL){
			for(i=0; i<trans->uncertain_no; i++){
				set_undefined_words(trans, i);
				ret = ret || build_linkage(trans);
				reset_undefined_words(trans);
			}
		}
		else*/
		ret = build_linkage(trans);

		//printf("\nCurrent Status:%d", current_status);
		//if (current_status==62)
		//	printf("Critical Point");
		return ret;
	}
	return ret; //Shouldn't reach here

}
int transfer_linkage_driver(Transfer trans, Linkage linkage){
	prepare_to_transfer(trans, linkage);
	build_lols (trans, linkage );
	if (extract_dis_strs(trans, linkage)==FALSE)
		return 0;
	transfer_linkage(trans, linkage, 0);

	return trans->target_no;

}

/***********************************************
*	Edited system functions
************************************************/
int my_sentence_parse(Sentence sent, Parse_Options opts) {
    int nl;

	verbosity = opts->verbosity;
	resources_reset_space(opts->resources);

    if (resources_exhausted(opts->resources)) {
	sent->num_valid_linkages = 0;
	return 0;
    }

    print_time(opts, "Finished expression pruning");
    my_prepare_to_parse(sent, opts);

    init_fast_matcher(sent);
    init_table(sent);

    free_parse_set(sent);
    init_x_table(sent);

    for (nl = opts->min_null_count;
	 (nl<=opts->max_null_count) && (!resources_exhausted(opts->resources)); ++nl) {
	sent->null_count = nl;
	sent->num_linkages_found = parse(sent, sent->null_count, opts);
	print_time(opts, "Counted parses");
	post_process_linkages(sent, opts);
	if (sent->num_valid_linkages > 0) break;
    }

    free_table(sent);
    free_fast_matcher(sent);
    print_time(opts, "Finished parse");

    return sent->num_valid_linkages;
}

/*********************************************************************************
*	This functions were used in the cases that a word in the disjunct array didn't
*   have any subscript, so the only way wto habdle this ambiguous cases were to
*   find out all of the possible matches, but they are not in use any more as we
*   prefered to ignore and avoid this case for now
************************************************/
/*
Match_list * match_list_add(Transfer trans, Match_list **head, Match_list *current){
	Match_list *ml, *ml_i;
	int list_size;
	int i;
	list_size = trans->list_size;
	ml = (Match_list *)exalloc(sizeof(Match_list));
	ml->list_size = list_size;
	ml->left = (C_table *)exalloc(list_size*sizeof(C_table));
	ml->right = (C_table *)exalloc(list_size*sizeof(C_table));
	ml->next = NULL;
	for(i=0; i<list_size; i++){
		ml->left[i] =(current==NULL) ? current->left[i]:-1;
	}
	for(i=0; i<list_size; i++){
		ml->right[i] = (current==NULL) ? current->right[i]:-1;;
	}
	//Now add this new node to the list
	ml_i = *head;
	if(ml_i==NULL){
		*head = ml;
		return;
	}
	for(; ml_i->next!=NULL; ml_i=ml_i->next);
	ml_i->next = ml;
	return ml;
}
void match_node_delete(Match_list **head, Match_list *node){
	Match_list *ml_i, *prev_ml=NULL;

	for(ml_i=*head; ml_i!=node; prev_ml=ml_i, ml_i = ml_i->next);
	if (ml_i = *head){
		(*head)=(*head)->next;
	}
	else if(ml_i->next!=NULL)
		prev_ml->next=ml_i->next;
	else
		prev_ml->next=NULL;

	exfree(ml_i->left,node->list_size*sizeof(int));
	exfree(ml_i->right,node->list_size*sizeof(int));
	exfree(ml_i,sizeof(Match_list));
}
void match_node_delete_no(Transfer trans, int node_num){
	int i;
	Match_list *node;
	for(i=0, node=trans->match_head; i<trans->uncertain_no; i++){
		node = node->next;
	}
	match_node_delete(&(trans->match_head), node);
}

void match_list_delete(Match_list **head){
	Match_list *node;
	while(*head!=NULL){
		match_node_delete(head, *head);
	}


}


void create_match_list(Transfer trans, Match_list *node){
	int h, next_unmatch,i=0;
	int cnd1,cnd2,cnd3;
	Match_list *new_node;
	C_table * ct1, *ct2;
	Connector *c;
	next_unmatch = node->last_left_match;
	if (next_unmatch==node->list_size){
		for(i=0; i=node->list_size; i++){
			if (node->right[i]==-1){
				match_list_delete(&(trans->match_head));
				return;
			}
		}
		return;
	}
	ct1 = trans->lc_list[node->left[next_unmatch]];//tbt
	h = power_hash(ct1->c)& (trans->table_size -1);
	for (ct2=trans->rc_table[h]; ct2!=NULL; ct2=ct2->next){
		cnd1=(ct2->list_no != -1);
		cnd2=(node->right[ct2->list_no]==BAD_WORD);
		cnd3 = ct1->t_word_no!=ct2->t_word_no;
		if (cnd1 && cnd2 && cnd3){
			new_node = match_list_add(trans, &(trans->match_head), node);
			new_node->left[next_unmatch]= ct2->list_no;
			new_node->right[ct2->list_no] = next_unmatch;
			new_node->last_left_match++;
			create_match_list(trans, new_node);
		}
	}
	match_list_delete(&(trans->match_head));

}
int find_uncertain_no(Match_list *ml){
	Match_list *node;
	int i;
	for (i=0, node = ml; node!=NULL; node=node->next, i++);
	return i;

}
int init_lists(Transfer trans){
	int i, lj, rj, lc_size=0, rc_size=0;
	C_table *ct;
	for (i=0;i<trans->table_size; i++){
		for(ct=trans->lc_table[i]; ct!=NULL; ct=ct->next){
			if (ct->c->word==BAD_WORD)
				lc_size++;
		}
		for(ct=trans->rc_table[i]; ct!=NULL; ct=ct->next){
			if (ct->c->word==BAD_WORD)
				rc_size++;
		}
	}
	if (lc_size!=rc_size){
		return FALSE;
	}
	trans->list_size = lc_size;
	if(trans->list_size==0)
		return TRUE;

	trans->lc_list = (C_table **) exalloc(trans->list_size * sizeof(C_table *));
	trans->rc_list = (C_table **) exalloc(trans->list_size * sizeof(C_table *));
	for (i=0, lj=0, rj=0;i<trans->table_size; i++){
		for(ct=trans->lc_table[i]; ct!=NULL; ct=ct->next){
			if (ct->c->word==BAD_WORD){
				ct->list_no = lj;
				trans->lc_list[lj] = ct;
				lj++;
			}
		}
		for(ct=trans->rc_table[i]; ct!=NULL; ct=ct->next){
			if (ct->c->word==BAD_WORD){
				ct->list_no = rj;
				trans->rc_list[lj] = ct;
				rj++;
			}
		}
	}
	match_list_add(trans, &(trans->match_head), NULL);
	create_match_list(trans,trans->match_head);
	trans->uncertain_no = find_uncertain_no(trans->match_head);
}
void set_undefined_words(Transfer trans, int uncertain_no){
	int i;
	Match_list *node;
	for(i=0, node=trans->match_head; i<uncertain_no; i++){
		node = node->next;
	}
	for (i=0; i<node->list_size; i++){
		trans->lc_list[i]->c->word = node->left[i];
		trans->rc_list[i]->c->word = node->right[i];
	}
}
*/
