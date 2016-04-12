/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

#include "link-includes.h"
#define PRINT_DIAGRAM 0
//Don't Delete it!
//#define ORIGINAL_ANALYSIS

void free_decomposed_word(Decomposed_word *decomposed_word)
{

 	if (decomposed_word==NULL)
	{
		return;
	}
	else
	{
    	free_decomposed_word(decomposed_word->next);
		xfree(decomposed_word, sizeof(Decomposed_word));

	}


}
void free_morpheme_list(Morpheme *morpheme_list)
{

 	if (morpheme_list==NULL)
	{
		return;
	}
	else
	{
    	free_morpheme_list(morpheme_list->next);
		feature_list_free(morpheme_list->f_list);
		xfree(morpheme_list, sizeof(Morpheme));

	}


}
void decompose(char* in,char* wr,Decomposed_word **morph_list, Dictionary    dict){

	char *o;
	char tempstr[MDWL];
	int i;
	Decomposed_word  *morph_node;
 	o=(char *) xalloc(MAX_WORD*sizeof(char));
	strcpy(o,"");
 	for (i=0;wr[i]!='\0';i++){
		o=strncat(o,wr+i,1);
		if (dictionary_lookup(dict, o))
		{
          strcpy(tempstr,in);
		  strcat(in," ");
          strcat(in,o);
		  decompose(in, wr+i+1,morph_list, dict);
          strcpy(in,tempstr);
		}
	}
	if ((wr[0]=='\0')&&(in[0]))	{

		morph_node=(Decomposed_word *) xalloc(sizeof(Decomposed_word));
		strcpy(morph_node->word,in);
		morph_node->next = (*morph_list);
		if(*morph_list!=NULL){
			(*morph_list)->prev=morph_node;
		}
		(*morph_list)=morph_node;

	}
	xfree(o,MAX_WORD*sizeof(char));
}
void test_decom_list(Decomposed_word *decom_list)
{
	 Decomposed_word *        node;
	 for (node=decom_list;node!=NULL;node=node->next)
		 printf("\n%s", node->word);

}

Decomposed_word * decompose_driver(char* in_word, Dictionary    dict){
   	char  in[MDWL]=HEAD_WORD;
	Decomposed_word *morph_list=NULL;
	decompose(in, in_word, &morph_list, dict);
	return morph_list;
}
int find_stem(Linkage linkage){
	Feature *f;
	int pos=-1;
	int i;
	for(i=1; i<linkage->num_words; i++){
		if(linkage->feature_array[i]==NULL)
			continue;
		if ((f=find_feature_path_string(linkage->feature_array[i], "root stem"))!=NULL
			&& (strcmp(f->attribute,"+")==0)){
				if (pos==-1){
					pos=i;
				}
				else{
					pos=-1;
					break;
				}
			}
	}
	return pos;
}
void morpheme_list_add(Morpho_structures ms, Morpheme **morpheme_list, char * new_word, Feature *new_f){
	Morpheme *node;
	Feature_list *fln;
	Feature *temp_f;
	Morpheme *temp_morpheme;
	for (node=*morpheme_list; node!=NULL; node=node->next){
		if (strcmp(node->word, new_word)==0){
			break;
		}
	}
	if (node!=NULL){
		for (fln=node->f_list; fln!=NULL; fln=fln->next)
		{
			if (feature_is_equal(fln->f,new_f))
				break;
		}
		if (fln!=NULL){
			return;
		}
		else{
			temp_f=feature_copy_driver(USE_SOURCE_STRING_SET, new_f);
			feature_list_add(&(node->f_list), temp_f);
		}
	}
	else{
		temp_morpheme = (Morpheme *)xalloc (sizeof (Morpheme));
		temp_morpheme->word = string_set_add(new_word, ms->dict->string_set);
		temp_morpheme->f_list=NULL;
		temp_f=feature_copy_driver(USE_SOURCE_STRING_SET, new_f);
		feature_list_add(&(temp_morpheme->f_list), temp_f);
		temp_morpheme->next = *morpheme_list;
		*morpheme_list = temp_morpheme;
	}

}
Morpheme * analyse_decomposed_word(Morpho_structures ms, Decomposed_word *decomposed_word){
	Sentence sent;
	Linkage linkage;
	Decomposed_word *node;
	Morpheme *morpheme_list=NULL;
	int num_linkages , i;
	int stem_pos;
	char * diagram;
//	ms->opts->unify_features = FALSE;
	for(node=decomposed_word; node!=NULL; node=node->next){
		sent=sentence_create(node->word, ms->dict);
		if(!sent){
			continue;
		}
		num_linkages = sentence_parse(sent, ms->opts);
		for (i=0; i<num_linkages; i++){
			linkage = linkage_create(i, sent, ms->opts);
			if (PRINT_DIAGRAM){
				linkage = linkage_create(i, sent, ms->opts);
				printf("%s\n", diagram = linkage_print_diagram(linkage));
				string_delete(diagram);
			}
			if ((stem_pos = find_stem(linkage))>=0){
				morpheme_list_add(ms, &morpheme_list,linkage->word[stem_pos], linkage->feature_array[0]);
			}
			linkage_delete(linkage);
		}
		sentence_delete(sent);
	}
	return morpheme_list;
}
Morpheme * morpho_analyse_list_maker(char* in_word, Morpho_structures ms){
		Decomposed_word * decomposed_word;
		Morpheme *morpheme_list=NULL;

		decomposed_word= decompose_driver(in_word, ms->dict);
		//test_decom_list(decomposed_word);//test
		morpheme_list = analyse_decomposed_word(ms, decomposed_word);
		free_decomposed_word(decomposed_word);
		return morpheme_list;
}



//**********************************
int phonological_validate(char *word)
{
	int i, len=strlen(word);
	for (i=0;i<len;i++)
		if(word[i]==' ')
		{
			if ((word[i-1]=='h') & (word[i+1]=='a'))
				return FALSE;
			if ((word[i-1]=='a') & (word[i+1]=='h'))
				return FALSE;
			if ((word[i-1]=='h') & (word[i+1]=='i'))
				return FALSE;
		}
return TRUE;
}
void delete_morpho_structures(Morpho_structures morpho_structures){
	dictionary_delete(morpho_structures->dict);
//	free_morpho_list(morpho_structures->ecomposed_word);
	parse_options_delete(morpho_structures->opts);
	xfree(morpho_structures, sizeof(struct Morpho_structures_s));
}
# if !defined ORIGINAL_ANALYSIS
X_node* morpheme_dict_node_maker(Dictionary dict, Morpheme *morpheme_list){
	Morpheme *morph;
	Feature_list *f_list;
	Dict_node *dn, *dnx;
	Feature *f1, *f2,*f3=NULL;
    X_node * x, * y;
	int n;
	int unifiable;
	char word[MAX_WORD];
	char temp[MAX_LINE]="";
	Feature * subcat;
	x=NULL;
	for(morph=morpheme_list; morph!=NULL; morph=morph->next){
		n=0;
		for(f_list=morph->f_list; f_list!=NULL; f_list=f_list->next){
			if ((subcat = find_feature_path_string(f_list->f, "root subcat"))==NULL)
				continue;
			dn = dictionary_lookup(dict, subcat->attribute);
			for (dnx=dn; dnx!=NULL; dnx=dnx->right){
				f1= feature_copy_driver(USE_SOURCE_STRING_SET, dnx->f);
				f2= feature_copy_driver(dict->string_set, f_list->f);
				if (PRINT_DIAGRAM){
					temp[0] = '\0';
					print_a_feature(f1,temp);
					printf("dn_feature : \n %s",temp);
					temp[0] = '\0';
					print_a_feature(f2,temp);
					printf("\nhead_feature : \n %s",temp);
				}
				unifiable=unify_features(f1,f2,&f3);
				if(unifiable==TRUE){
					y = (X_node *) xalloc(sizeof(X_node));
					y->next = x;
					x = y;
					x->exp = copy_Exp(dnx->exp);
					sprintf(word, "%s_%d", morph->word, n);
					x->string = string_set_add(word, dict->string_set);
	//					x->string = morph->word;
					x->feature = f3;
					n++;
				}
				else{
					feature_free(f3);
				}
			}
		}
	}

return x;
}
#else
X_node* morpheme_dict_node_maker(Dictionary dict, Morpheme *morpheme_list){
	Morpheme *morph;
	Feature_list *f_list;
	Dict_node *dn, *dnx;
	Feature *f1, *f2,*f3=NULL;
    X_node * x, * y;
	int n;
	int unifiable;
	char word[MAX_WORD];

	x=NULL;
	for(morph=morpheme_list; morph!=NULL; morph=morph->next){
		dn=dictionary_lookup(dict, morph->word);
		for(dnx=dn; dnx!=NULL; dnx=dnx->right){
			n=0;
			for(f_list=morph->f_list; f_list!=NULL; f_list=f_list->next){
				f1= feature_copy_driver(USE_SOURCE_DICT, dn->f);
				f2= feature_copy_driver(dict, f_list->f);
				unifiable=unify_features(f1,f2,&f3);
				if(unifiable==TRUE){
					y = (X_node *) xalloc(sizeof(X_node));
					y->next = x;
					x = y;
					x->exp = copy_Exp(dnx->exp);
					sprintf(word, "%s_%d", dn->string, n);
					x->string = string_set_add(word, dict->string_set);
//					x->string = morph->word;
					x->feature = f3;
					n++;
				}
				else{
					feature_free(f3);
				}
			}
		}
	}

return x;
}

#endif
X_node *morpho_analyze(Dictionary dict,char* word){
	X_node *x=NULL;
	Morpheme *morpheme_list=NULL;
	morpheme_list = morpho_analyse_list_maker(word, dict->morpho_structures);
	x=morpheme_dict_node_maker(dict, morpheme_list);
	free_morpheme_list(morpheme_list);
	return x;
}

int boolean_morpheme_dict_node_maker(Dictionary dict, Morpheme *morpheme_list){
	Morpheme *morph;
	Feature_list *f_list;
	Dict_node *dn, *dnx;
	Feature *f1, *f2,*f3=NULL;
	int unifiable=FALSE;

	for(morph=morpheme_list; morph!=NULL; morph=morph->next){
		dn=dictionary_lookup(dict, morph->word);
		for(dnx=dn; dnx!=NULL; dnx=dnx->right){
			for(f_list=morph->f_list; f_list!=NULL; f_list=f_list->next){
				f1= feature_copy_driver(USE_SOURCE_STRING_SET, dn->f);
				f2= feature_copy_driver(dict->string_set, f_list->f);
				unifiable=unify_features(f1,f2,&f3);
				feature_free(f3);
				if (unifiable==TRUE){
					break;
				}
			}
		}
	}

return unifiable;
}

int boolean_morpho_analyze(Dictionary dict,char* word){
	int lookup;
	Morpheme *morpheme_list=NULL;
	morpheme_list = morpho_analyse_list_maker(word, dict->morpho_structures);
	lookup=boolean_morpheme_dict_node_maker(dict, morpheme_list);
	free_morpheme_list(morpheme_list);
	return lookup;
}
