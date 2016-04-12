/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

#define MAX_LINK_NAME 20
#define VIRGULE ','
#define L_PAR '('
#define R_PAR ')'
#define OPEN_BRACKET '['
#define CLOSE_BRACKET ']'
#define POINT '.'
#define ATSIGN '@'
#define END_STR '\0'
#define MAX_CONNECTOR_NAME 20
#define MAX_DIS_STR 2000
#define MAX_DIS 200
#define MAX_NUM_LIST (MAX_SENTENCE*3+MAX_SENTENCE+1)
#define MAX_NESTED_WORDS 10
#define LEFT 0
#define RIGHT 1
#define FAIL -1
#define EMPTY_DIS "()()"
#define MAX_MAPS 20
#define MAX_TARGET_NO 50

#define XOR(A,B) ( (!(A) && (B)) || ((A) && !(B)) )//A.S


typedef struct Lex_entry_struct Lex_entry;
struct Lex_entry_struct{
	char * src;
	char * trg[MAX_MAPS];
	int num;
	int line_number;
};
typedef struct Lexicon_struct Lexicon;
struct Lexicon_struct{
    int size;       /* the current size of the table */
    int count;      /* number of things currently in the table */
	Lex_entry ** table;  /* the table itself */
};

typedef struct c_table_struct C_table;
struct c_table_struct {
    Connector * c;
	int t_word_no;
	int list_no;
	//char marked;	///add
	C_table * next;
};


typedef struct T_word_s T_word;
struct T_word_s{
	int no_word;
	int marked;
	char * string;
	Feature *f;
	Connector * left, * right;
};

typedef struct T_word_array_s  T_word_array;
struct T_word_array_s{
	T_word ** elements;
	int last_t_word_no;
	int	new_word_t_word_no;
	int sent_no;
};

//typedef struct String_list_ex_struct String_list;
//struct String_list_ex_struct {
//	char * string;
//	String_list * next;
//};
/*
typedef struct Match_list_struct Match_list;
struct Match_list_struct{
	int *left;
	int *right;
	int list_size;
	int last_left_match;
	Match_list *next;
};*/
typedef struct  Lol_list_s Lol_list;
struct Lol_list_s{
	List_o_links *lol;
	char * name;
	Lol_list *next;
};
typedef struct  Word_lols_s Word_lols;
struct Word_lols_s{
	Lol_list *left, *right;
};

typedef struct  Dis_str_array_s Dis_str_array;
struct Dis_str_array_s{
	Lex_entry *dis_map;
	char  num_list [MAX_NUM_LIST];
	char target_word[MAX_WORD];
	Lexicon *links_map;
	int current_map;
	//int last_t_word_no;
	//int added_t_word_no;
	//char * nested_num_list;
};

typedef struct Transfer_s * Transfer;
struct Transfer_s{
	Dictionary target_dict;
	String_set *string_set;
	Sentence sent[MAX_TARGET_NO];
    Parse_Options opts;
	int num_linkages;
	int num_words;
//	Match_list *match_head;
	char lex_file_name[MAX_PATH_NAME];
	Lexicon *lexicon;
	char map_file_name[MAX_PATH_NAME];
	Lexicon *dis_map_set;
	char links_list_file[MAX_PATH_NAME];
//	String_list * links_list;
	Lexicon * links_list;
//	String_list * sub_sensitive_links_list;
	int map_line_num;
	T_word_array *t_word_array;
	T_word_array *ordered_t_word_array;
	Dis_str_array ** dis_str_array;
	int table_size ;
	C_table ** lc_table, ** rc_table;
	C_table ** lc_list, ** rc_list;
	int list_size;
	int uncertain_no;
	int target_no;
	Word_lols ** word_lols;

};

typedef struct Automap_s Automap;
struct Automap_s{
	Transfer transfer;
	Dictionary src_dict;
	Sentence src_sent, trg_sent;

};

extern char maperrmsg[1024];
