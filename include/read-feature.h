/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

#define SPECIAL_CHARS "(){};[]&|:<>,="
void feature_error(Dictionary dict,char *fmt, ...);
int  feature_conn_read(Dictionary dict, char * dict_path_name);
int  feature_templates_read(Dictionary dict, char * dict_path_name);
Feature * find_feature_path_string(Feature *f, char *path);
Feature * ex_feature_copy_driver(String_set *ss, Feature *f);
Feature * feature_copy_driver(String_set *ss, Feature *f);
char * print_features(Linkage linkage);
Feature * feature_read_driver(Dictionary dict);
void make_file_name(char*trg, char* src, char* aux);
void feature_list_free(Feature_list * fl);
void unify_sentence(Sentence sent, Parse_Options opts);
int unify_linkage(Sentence sent);
void initialize_feature_array(Parse_info *pi);
void free_feature_array(Parse_info *pi);
void compute_feature_array(Parse_info *pi);
void feature_free(Feature * f);
void feature_equ_free(Feature_equ *fe);
int conn_name_is_valid(char * s);
void exfeature_free(Feature * f);
int feature_is_equal(Feature * f1 , Feature * f2);
void feature_list_add( Feature_list **f1, Feature * f2);
void print_a_feature(Feature *f, char *fp);
int  unify_features(Feature * f1 , Feature * f2, Feature **unified_f);
int string_is_equal(char * s, int c);
int read_general_word_file_features(Dictionary dict, Dict_node * dn);
void concat_subs(Dictionary dict,  Dict_node *dn);
int is_special(char * s);
