/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

Linkage trans_linkage_create(Transfer trans, int i, Sentence sent, Parse_Options opts);
Transfer transfer_create(char* map_file_name, char* links_list_file, char* lexicon, char* trg_dict_name);
char *strappend(char *s, char*t);
int transfer_linkage_driver(Transfer trans, Linkage linkage);
int get_t_word_no(T_word_array *t_word_array, char * string, int start, int end);
int my_sentence_parse(Sentence sent, Parse_Options opts);
char* strcpy_to_delim(char *trg, char * src, int c);
char* strcpy_from_delim(char *trg, char * src, int c);
int find_lex(Lexicon *lexicon, char * trg, char * src);
int  get_conn_delim(char *connector_name);
char *  itoa ( int value, char * str);
void transfer_delete(Transfer trans);
