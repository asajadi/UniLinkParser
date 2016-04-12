/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/
#include "link-includes.h"

//Don't Delete it!
//#define ENG_PERS

void strcpy_end(char *word, char* mor){
	char t1[MAX_WORD], t2[MAX_WORD];
	strcpy_to_delim(t1, word, '.');
	strcpy_from_delim(t2, word, '.');
	sprintf(word, "%s%s%s",t1,mor,t2);
}
void strcpy_beg(char *word, char* mor){
	char t1[MAX_WORD];
	strcpy(t1,word);
	sprintf(word, "%s%s",mor,t1);
}

# if defined ENG_PERS
void morpho_adjust_noun(char *word, Feature* f1, Feature *f2){
	Feature *att_v;
	char det[MAX_WORD]="";
	char def[MAX_WORD]="";
	att_v = find_feature_path_string(f1, "root head agr");
	if (att_v==NULL)
		return;
	if ((strcmp(att_v->attribute, "1p")==0) || (strcmp(att_v->attribute, "2p")==0)
		|| (strcmp(att_v->attribute, "3p")==0))
	{
		strcpy_end(word, "ha");
	}
	att_v = find_feature_path_string(f1, "root det def");
	if (att_v==NULL)
		return;
	strcpy(def, att_v->attribute);
	att_v = find_feature_path_string(f1, "root det sem");
	if (att_v==NULL)
		return;
	strcpy(det, att_v->attribute);
	if (strcmp(det, "a")==0){
		if (strcmp(def, "+")!=0)
 		  strcpy_beg(word, "yk ");
		else
 		 strcpy_beg(word, "An ");
	}
	else if (strcmp(det, "the")==0){
		if (strcmp(def, "+")==0)
 		  strcpy_beg(word, "An ");
	}
	else if (strcmp(det, "my")==0){
		if (word[strlen(word)-1]=='a')
			strcpy_end(word, "y");
		strcpy_end(word, "m");

	}

}
void get_past_form(char *word, Feature* f1){
	char t1[MAX_WORD];
	Feature *att_v;
	att_v = find_feature_path_string(f1, "root pform");
	if (att_v==NULL)
		return;
	if ((strcmp(att_v->attribute, "t")==0) || (strcmp(att_v->attribute, "d")==0)
		|| (strcmp(att_v->attribute, "yd")==0) || (strcmp(att_v->attribute, "ad")==0))
	{
		strcpy_end(word,att_v->attribute);
		return;
	}
	strcpy(word, att_v->attribute);
	return;
}
void translate_word(Lexicon *lexicon, char * trg, char * src, Feature *f){
	Feature *att_v;
	char temp[MAX_WORD];
	char temp_word[MAX_WORD];
	strcpy(temp, src);
	att_v = find_feature_path_string(f, "root particle");
	if (att_v!=NULL){
		if (strcmp(att_v->attribute, "?")!=0){
			strcpy_end(temp, "-");
			strcpy_end(temp, att_v->attribute);
		}
	}
	if	(find_lex(lexicon, trg,  temp))
		return;
	strcpy_to_delim(temp, temp, '.');
	find_lex(lexicon, trg,  temp);
}


void set_person_number(char *word, Feature *f1, char *tense, char *perfect, char *mood){
	char t1[MAX_WORD];
	Feature *att_v;
	char agr[MAX_FEATURE_ATT]="";

	att_v = find_feature_path_string(f1, "agr");
	if (att_v!=NULL)
		strcpy(agr, att_v->attribute);
	if ((strcmp(perfect,"+")==0) && (strcmp(tense,"present")==0) && (strcmp(mood,"sub")==0))
		strcpy_end(word, "-ba^s");
	if ((strcmp(perfect,"+")==0) && (strcmp(tense,"past")!=0) && (strcmp(mood,"sub")!=0)){
		if (strcmp(agr,"1s")==0){
			strcpy_end(word, "-am");
			return;
		}
		if (strcmp(agr,"2s")==0){
			strcpy_end(word, "-ay");
			return;
		}
		if ((strcmp(agr,"3s")==0)){
			strcpy_end(word, "-ast");
			return;
		}
		if (strcmp(agr,"1p")==0){
			strcpy_end(word, "-aym");
			return;
		}
		if (strcmp(agr,"2p")==0){
			strcpy_end(word, "-ayd");
			return;
		}
		if (strcmp(agr,"3p")==0){
			strcpy_end(word, "-and");
			return;
		}

	}
	else{
		if (strcmp(agr,"1s")==0){
			strcpy_end(word, "m");
			return;
		}
		if (strcmp(agr,"2s")==0){
			strcpy_end(word, "y");
			return;
		}
		if ((strcmp(agr,"3s")==0)&&(strcmp(tense,"past")!=0)){
			strcpy_end(word, "d");
			return;
		}
		if (strcmp(agr,"1p")==0){
			strcpy_end(word, "ym");
			return;
		}
		if (strcmp(agr,"2p")==0){
			strcpy_end(word, "yd");
			return;
		}
		if (strcmp(agr,"3p")==0){
			strcpy_end(word, "nd");
			return;
		}

	}
}


void morpho_adjust_verb(Dictionary dict, Lexicon *lex, char *word, Feature* f1, Feature *f2){
	char voice_stem[MAX_WORD]="", t1[MAX_WORD], t2[MAX_WORD];
	char tense[MAX_FEATURE_ATT]="", gerund[MAX_FEATURE_ATT]="", perfect[MAX_FEATURE_ATT]="";
	char mood[MAX_FEATURE_ATT]="", simple_form[MAX_FEATURE_ATT]="", att[MAX_FEATURE_ATT];
	char aux[MAX_FEATURE_ATT]="", negative[MAX_FEATURE_ATT]="";
	char present_morpheme[3];
	int has_aux = FALSE;
	Feature *att_v, *sub_f1;
	Dict_node *dn;
	att_v = find_eature_path_string(f1, "root head vform aspect voice");
	if (strcmp(att_v->attribute,"+")==0){
		get_past_form(word, f2);
		strcpy_end(word,"h ");
		strcpy(voice_stem, word);
		strcpy(word, "^sv");
		dn = dictionary_lookup(dict, word);
		f2 = (dn!=NULL) ? dn->f : NULL;
	}

	att_v = find_feature_path_string(f1, "root verb");
	if ((att_v!=NULL)&&(strcmp(att_v->attribute,"?")!=0))
		f1 = att_v;
//	else    Temperory 1000
//		f1 = find_feature_path_string(f1, "root head");

	att_v = find_feature_path_string(f1, "negative");
	if(att_v!=NULL)
		strcpy(negative, att_v->attribute);
	att_v = find_feature_path_string(f1, "aux");
	if(att_v!=NULL)
		strcpy(aux, att_v->attribute);
	att_v = find_feature_path_string(f1, "vform tense");
	if(att_v!=NULL)
		strcpy(tense, att_v->attribute);
	att_v = find_feature_path_string(f1, "vform aspect gerund");
	if(att_v!=NULL)
		strcpy(gerund, att_v->attribute);
	att_v = find_feature_path_string(f1, "vform aspect perfect");
	if(att_v!=NULL)
		strcpy(perfect, att_v->attribute);
	att_v = find_feature_path_string(f1, "vform aspect sub-imp");
	if(att_v!=NULL)
		strcpy(mood, att_v->attribute);
	att_v = find_feature_path_string(f2, "root simple-form");
	if(att_v!=NULL)
		strcpy(simple_form, att_v->attribute);

	if ((aux[0]!='\0') && (strcmp(aux,"will")!=0)){
		translate_word(lex, aux, aux, NULL);
		dn = dictionary_lookup(dict, aux);
		if (dn!=NULL)
			att_v = find_feature_path_string(dn->f, "root pesrson_number");
		if ((att_v!= NULL) && (strcmp(att_v->attribute, "+")==0)){
			if (strcmp(tense, "past")==0){
				strcpy_beg(aux, "my");
				get_past_form(aux, dn->f);
				set_person_number(aux, f1, "past", "-", "-");
			}
			else {
				if (strcmp(mood, "sub")==0)
					strcpy_beg(aux, "b");
				else
					strcpy_beg(aux, "my");
				set_person_number(aux, f1, tense, "-", "-");
			}
		}
		strcpy(mood, "sub");
		strcpy(gerund, "-");
	}
	if (strcmp(mood, "sub")==0)
		strcpy(present_morpheme, "b");
	else
		strcpy(present_morpheme, "my");
	if (strcmp(tense, "future")==0){
		if (strcmp(gerund, "+")==0){
			if (strcmp(perfect, "+")==0){
				get_past_form(word, f2);
				strcpy_end(word,"h");
				set_person_number(word, f1,  tense, perfect, mood );
			}
			else{
				strcpy_beg(word, "my");
				set_person_number(word, f1,  tense, perfect, mood );
			}
		}
		else{
			if (strcmp(perfect, "+")==0){
				get_past_form(word, f2);
				strcpy_end(word,"h");
				set_person_number(word, f1,  tense, perfect, mood );
			}
			else{
				get_past_form(word, f2);
				strcpy(t1, "xah");
				set_person_number(t1, f1,  tense, perfect, mood );
				strcpy_beg(word, " ");
				strcpy_beg(word, t1);
			}
		}
	}

	else if (strcmp(tense, "present")==0){
		if (strcmp(mood, "imp")!=0){
			if (strcmp(gerund, "+")==0){
				if (strcmp(perfect, "+")==0){
					get_past_form(word, f2);
					strcpy_beg(word, "my");
					strcpy_end(word, "h");
					set_person_number(word, f1,  tense, perfect, mood );
				}
				else{
					strcpy_beg(word, "my");
					set_person_number(word, f1,  tense, perfect, mood );
				}
			}
			else{
				if (strcmp(perfect, "+")==0){
					get_past_form(word, f2);
					strcpy_end(word, "h");
					set_person_number(word, f1,  tense, perfect, mood );
				}
				else{
					if (strcmp(simple_form,"+")!=0){
						strcpy_beg(word, present_morpheme);
						set_person_number(word, f1,  tense, perfect, mood );
					}
				}
			}
		}

	}

	else if (strcmp(tense, "past")==0){
		get_past_form(word, f2);
		if (strcmp(gerund, "+")==0){
			if (strcmp(perfect, "+")==0){
				strcpy_beg(word, "my");
				strcpy_end(word, "h-bvd");
				set_person_number(word, f1,  tense, perfect, mood );
			}
			else {
				strcpy_beg(word, "my");
				set_person_number(word, f1,  tense, perfect, mood );
			}
		}
		else{
			if (strcmp(perfect, "+")==0){
				strcpy_end(word, "h-bvd");
				set_person_number(word, f1,  tense, perfect, mood );
			}
			else {
				set_person_number(word, f1,  tense, perfect, mood );
			}
		}
	}
	else{
		set_person_number(word, f1,  tense, perfect, mood );
	}

	if (strcmp(negative, "+")==0){
		if (aux[0]!='\0')
			strcpy_beg(aux, "n");
		else
			strcpy_beg(word, "n");

	}
	strcpy_beg(word, voice_stem);
	if (aux[0]!='\0'){
		strcpy_beg(word, " ");
		strcpy_beg(word, aux);
	}

}

void morphological_adjuster(Dictionary dict, Lexicon *lex, char *word, Feature* f1, Feature *f2){
	Feature *att_v;
	//att_v = find_feature_path_string(f1, "root sem");
	//if ((att_v!=NULL) && (strcmp(att_v->attribute,"?")!=0)){
	//	strcpy(word, att_v->attribute);
	//}
	att_v = find_feature_path_string(f1, "root cat");
	if (att_v==NULL)
		return;
	if (strcmp(att_v->attribute, "n")==0){
		morpho_adjust_noun(word, f1, f2);
		return;
	}
	if (strcmp(att_v->attribute, "v")==0){
			morpho_adjust_verb(dict, lex, word, f1, f2);
			return;
		}

}

void morphological_synthesizer(Linkage linkage, Dictionary trg_dict, Lexicon *lex){
	int i;
	Dict_node *dn;
	Feature *trg_f = NULL;
	char word[MAX_WORD];
	for (i=0; i<linkage->num_words; i++){
		if (strcmp(linkage->word[i], "v")==0)
			continue;    //As and can take any feature
		dn = dictionary_lookup(trg_dict, linkage->word[i]);
		//if (dn==NULL)
		//	continue;
	    if (dn!=NULL)
			trg_f = dn->f;
		strcpy(word, linkage->word[i]);
		morphological_adjuster(trg_dict, lex, word, linkage->feature_array[i], trg_f);
		string_delete(linkage->word[i]);
		linkage->word[i] = (char *) exalloc(strlen(word)+1);
		strcpy(linkage->word[i], word);
	}
}

# else
void set_person_number_for_noun(char *word, Feature *f1){
	Feature *att_v;

	char person[MAX_FEATURE_ATT]="", num[MAX_FEATURE_ATT]="";

	att_v = find_feature_path_string(f1, "root ez_pp");
	if ((att_v!=NULL) && (strcmp (att_v->attribute, "ez")==0))
		return;

	att_v = find_feature_path_string(f1, "root pos person");
	if (att_v!=NULL)
		strcpy(person, att_v->attribute);
	else
		return;
	att_v = find_feature_path_string(f1, "root pos num");
	if (att_v!=NULL)
		strcpy(num, att_v->attribute);
	else
		return;
	if ((strcmp(person,"-")==0)||(strcmp(num,"-")==0))
		return;
	if (word[strlen(word)-1]=='a')
		strcpy_end(word,"y");
	else if(word[strlen(word)-1]=='h')
		strcpy_end(word,"a");

	if ((strcmp(person,"1")==0)&&((strcmp(num,"s")==0))){
		strcpy_end(word, "m");
		return;
	}
	if ((strcmp(person,"2")==0)&&((strcmp(num,"s")==0))){
		strcpy_end(word, "t");
		return;
	}
	if ((strcmp(person,"3")==0)&&((strcmp(num,"s")==0))){
		strcpy_end(word, "^s");
		return;
	}
	if ((strcmp(person,"1")==0)&&((strcmp(num,"p")==0))){
		strcpy_end(word, "man");
		return;
	}
	if ((strcmp(person,"2")==0)&&((strcmp(num,"p")==0))){
		strcpy_end(word, "tan");
		return;
	}
	if ((strcmp(person,"3")==0)&&((strcmp(num,"p")==0))){
		strcpy_end(word, "^san");
		return;
	}


}

void morpho_adjust_noun(char *word, Feature* f1, Feature *f2){
	Feature *att_v;
	char rc[MAX_FEATURE_ATT]="", hp[MAX_FEATURE_ATT]="";
	att_v = find_feature_path_string(f1, "root agr num");
	if (att_v==NULL)
		return;
	if ((strcmp(att_v->attribute, "p")==0) || (strcmp(att_v->attribute, "sp")==0)){
		att_v = find_feature_path_string(f1, "root rc");
		if (att_v!=NULL)
			strcpy(rc, att_v->attribute);
		att_v = find_feature_path_string(f1, "root hp");
		if (att_v!=NULL)
			strcpy(hp, att_v->attribute);
		if ( (strcmp(rc, "+")!=0) && (strcmp(hp, "+")!=0))
			strcpy_end(word, "ha");
	}


	att_v = find_feature_path_string(f1, "root  indef");
	if (att_v==NULL)
		return;

	if ((strcmp(att_v->attribute, "e")==0)||(strcmp(att_v->attribute, "ek")==0)){
		if ((word[strlen(word)-1]=='a') || (word[strlen(word)-1]=='h'))
			strcpy_end(word, "yy");
		else
			strcpy_end(word, "y");
	}

	att_v = find_feature_path_string(f1, "root  ez_pp");
	if ((att_v!=NULL)&& (strcmp(att_v->attribute, "ez")==0)){
		if ((word[strlen(word)-1]=='a') || (word[strlen(word)-1]=='h'))
			strcpy_end(word, "y");
	}

	if ((strcmp(att_v->attribute, "e")==0)||(strcmp(att_v->attribute, "ek")==0)){
		if ((word[strlen(word)-1]=='a') || (word[strlen(word)-1]=='h'))
			strcpy_end(word, "y");
		else
			strcpy_end(word, "y");
	}

	set_person_number_for_noun (word, f1);
}
void get_past_form(char *word, Feature* f1){
	Feature *att_v;
	att_v = find_feature_path_string(f1, "root pform");
	if (att_v==NULL)
		return;
	if ((strcmp(att_v->attribute, "t")==0) || (strcmp(att_v->attribute, "d")==0)
		|| (strcmp(att_v->attribute, "yd")==0) || (strcmp(att_v->attribute, "ad")==0))
	{
		strcpy_end(word,att_v->attribute);
		return;
	}
	strcpy(word, att_v->attribute);
	return;
}
void translate_word(Lexicon *lexicon, char * trg, char * src, Feature *f){
	char temp[MAX_WORD];
	if	(find_lex(lexicon, trg,  src))
		return;
	strcpy_to_delim(temp, src, '.');
	find_lex(lexicon, trg,  temp);
}


void set_person_number_for_verb(char *word, Feature *f1, char *tense, char *perfect, char *mood){
	Feature *att_v;
	char person[MAX_FEATURE_ATT]="", num[MAX_FEATURE_ATT]="";
	if (strcmp(mood,"a")==0)
		return;
	att_v = find_feature_path_string(f1, "root agr person");
	if (att_v!=NULL)
		strcpy(person, att_v->attribute);
	att_v = find_feature_path_string(f1, "root agr num");
	if (att_v!=NULL)
		strcpy(num, att_v->attribute);
	if ((strcmp(perfect,"+")==0) && (strcmp(tense,"past")==0) && (strcmp(mood,"e")==0)){
		if ((strcmp(person,"1")==0)&&((strcmp(num,"s")==0))){
			strcpy_end(word, "-am");
			return;
		}
		else if ((strcmp(person,"2")==0)&&((strcmp(num,"s")==0))){
			strcpy_end(word, "-ay");
			return;
		}
		else if ((strcmp(person,"3")==0)&&((strcmp(num,"s")==0))){
			strcpy_end(word, "-ast");
			return;
		}
		else if ((strcmp(person,"1")==0)&&((strcmp(num,"p")==0))){
			strcpy_end(word, "-aym");
			return;
		}
		else if ((strcmp(person,"2")==0)&&((strcmp(num,"p")==0))){
			strcpy_end(word, "-ayd");
			return;
		}
		else if ((strcmp(person,"3")==0)&&((strcmp(num,"p")==0))){
			strcpy_end(word, "-and");
			return;
		}

	}
	else{
		if ((strcmp(person,"1")==0)&&((strcmp(num,"s")==0))){
			strcpy_end(word, "m");
			return;
		}
		else if ((strcmp(person,"2")==0)&&((strcmp(num,"s")==0))){
			strcpy_end(word, "y");
			return;
		}
		else if ((strcmp(person,"3")==0)&&((strcmp(num,"s")==0)) && (strcmp(tense,"past")!=0)){
			strcpy_end(word, "d");
			return;
		}
		else if ((strcmp(person,"1")==0)&&((strcmp(num,"p")==0))){
			strcpy_end(word, "ym");
			return;
		}
		else if ((strcmp(person,"2")==0)&&((strcmp(num,"p")==0))){
			strcpy_end(word, "yd");
			return;
		}
		else if ((strcmp(person,"3")==0)&&((strcmp(num,"p")==0))){
			strcpy_end(word, "nd");
			return;
		}

	}
}


void morpho_adjust_verb(Dictionary dict, Lexicon *lex, char *word, Feature* f1, Feature *f2){
	char tense[MAX_FEATURE_ATT]="", gerund[MAX_FEATURE_ATT]="", perfect[MAX_FEATURE_ATT]="";
	char negative[MAX_FEATURE_ATT]="", mood[MAX_FEATURE_ATT]="", far[MAX_FEATURE_ATT]="";
	char voice[MAX_FEATURE_ATT]="", voice_stem[MAX_WORD]="";
	Feature *att_v;
	Dict_node *dn;

	//reding features
	att_v = find_feature_path_string(f1, "root vform tense");
	if(att_v!=NULL)
		strcpy(tense, att_v->attribute);
	att_v = find_feature_path_string(f1, "root vform negative");
	if(att_v!=NULL)
		strcpy(negative, att_v->attribute);
	att_v = find_feature_path_string(f1, "root vform aspect mood");
	if(att_v!=NULL)
		strcpy(mood, att_v->attribute);
	att_v = find_feature_path_string(f1, "root vform aspect far");
	if(att_v!=NULL)
		strcpy(far, att_v->attribute);
	att_v = find_feature_path_string(f1, "root vform aspect gerund");
	if(att_v!=NULL)
		strcpy(gerund, att_v->attribute);
	att_v = find_feature_path_string(f1, "root vform aspect perfect");
	if(att_v!=NULL)
		strcpy(perfect, att_v->attribute);
	att_v = find_feature_path_string(f1, "root voice");
	if(att_v!=NULL)
		strcpy(voice, att_v->attribute);
	//if (att_v!=NULL)
	//	if (strcmp(att_v->attribute,"+")==0){
	//		iv_f1 = find_feature_path_string(f1, "root  vform iverb");
	//		att_v = find_feature_path_string(f2, "root  vform iverb sem");
	//		strcpy(iverb, att_v->attribute);
	//		find_lex(lex, iverb_fa,  iverb);
	//		dn = dictionary_lookup(dict, iverb_fa);
	//		if (dn!=NULL)
	//			iv_f2 = dn->f;
	//		morpho_adjust_verb(dict, lex, iverb_fa, iv_f1, iv_f2);
	//	}

	//state machine
	if (strcmp(voice,"+")==0){
		get_past_form(word, f2);
		strcpy_end(word,"h ");
		strcpy(voice_stem, word);
		strcpy(word, "^sv");
		dn = dictionary_lookup(dict, word);
		f2 = (dn!=NULL) ? dn->f : NULL;
	}

	if (strcmp(tense, "past")==0){
		if (strcmp(mood, "e")==0){
			//1.1
			if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				get_past_form(word, f2);
			}
			//1.2
			else if ((strcmp(gerund,"+")==0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				get_past_form(word, f2);
				strcpy_beg (word, "my");
			}
			//1.3
			else if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")==0)&&(strcmp(far,"+")!=0)){
				get_past_form(word, f2);
				strcpy_end (word, "h");
			}
			//1.4
			else if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")==0)){
				get_past_form(word, f2);
				strcpy_end (word, "h-bvd");
			}
		}
		else if (strcmp(mood, "s")==0){
			//1.5
			//?
			if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				get_past_form(word, f2);
				strcpy_beg (word, "my");
			}
			//1.6
			else if ((strcmp(gerund,"+")==0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				att_v = find_feature_path_string(f1, "root  vform iv");

				if ((strcmp(att_v->attribute,"c")==0)&&(att_v->next==NULL))
					strcpy_beg (word, "b");
				else{
					get_past_form(word, f2);
					strcpy_beg (word, "my");
				}
			}
			//1.7
			else if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")==0)&&(strcmp(far,"+")!=0)){
				get_past_form(word, f2);
				strcpy_end (word, "h-ba^s");
			}
			//1.8
			else if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")==0)){
				get_past_form(word, f2);
				strcpy_end (word, "h-ba^s");
			}
		}

	}//past
	else if (strcmp(tense, "present")==0){
		if (strcmp(mood, "e")==0){
			//2.1
			if ((strcmp(gerund,"+")==0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				strcpy_beg(word, "my");
			}
		}
		else if (strcmp(mood, "s")==0){
			//2.2
			if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				strcpy_beg (word, "be");
			}
		}
		else if (strcmp(mood, "a")==0){
			 if ((strcmp(gerund,"+")!=0)&&(strcmp(perfect,"+")!=0)&&(strcmp(far,"+")!=0)){
				strcpy_beg (word, "b");
			}
		}
	}
	set_person_number_for_verb(word, f1,  tense, perfect, mood );
	if (*voice_stem !='\0'){
		strcpy_beg(word, "-");
		strcpy_beg(word, voice_stem);
	}

}

void morpho_adjust_objaux(Dictionary dict, Lexicon *lex, char *word, Feature* f1, Feature *f2){
	Feature *att_v;
	char person[MAX_FEATURE_ATT]="", num[MAX_FEATURE_ATT]="";

	att_v = find_feature_path_string(f1, "root agr person");
	if (att_v!=NULL)
		strcpy(person, att_v->attribute);
	att_v = find_feature_path_string(f1, "root agr num");
	if (att_v!=NULL)
		strcpy(num, att_v->attribute);
	if ((strcmp(person,"1")==0)&&((strcmp(num,"s")==0))){
		strcpy_end(word, "m");
		return;
	}
	else if ((strcmp(person,"2")==0)&&((strcmp(num,"s")==0))){
		strcpy_end(word, "y");
		return;
	}
	else if ((strcmp(person,"3")==0)&&((strcmp(num,"s")==0))){
		strcpy_end(word, "-ast");
		return;
	}
	else if ((strcmp(person,"1")==0)&&((strcmp(num,"p")==0))){
		strcpy_end(word, "ym");
		return;
	}
	else if ((strcmp(person,"2")==0)&&((strcmp(num,"p")==0))){
		strcpy_end(word, "yd");
		return;
	}
	else if ((strcmp(person,"3")==0)&&((strcmp(num,"p")==0))){
		strcpy_end(word, "nd");
		return;
	}

}
void morpho_adjust_adjective(Dictionary dict, Lexicon *lex, char *word, Feature* f1, Feature *f2){
		Feature *att_v;
	att_v = find_feature_path_string(f1, "root super");
	if ((att_v!=NULL) && (strcmp(att_v->attribute,"+")==0)){
		strcpy_end(word, "trin");
		return;
	}
	set_person_number_for_noun (word, f1);
}
void morphological_adjuster(Dictionary dict, Lexicon *lex, char *word, Feature* f1, Feature *f2){
	Feature *att_v;
	att_v = find_feature_path_string(f1, "root cat");
	if (att_v==NULL)
		return;
	if (strcmp(att_v->attribute, "n")==0){
		morpho_adjust_noun(word, f1, f2);
		return;
	}
	if (strcmp(att_v->attribute, "v")==0){
		morpho_adjust_verb(dict, lex, word, f1, f2);
		return;
		}
	if (strcmp(att_v->attribute, "objaux")==0){
		morpho_adjust_objaux(dict, lex, word, f1, f2);
		return;
		}
	if (strcmp(att_v->attribute, "a")==0){
		morpho_adjust_adjective(dict, lex, word, f1, f2);
		return;
		}

}

void morphological_synthesizer(Linkage linkage, Dictionary trg_dict, Lexicon *lex){
	int i;
	Dict_node *dn;
	Feature *trg_f = NULL;
	char word[MAX_WORD];
	for (i=0; i<linkage->num_words; i++){
		if (strcmp(linkage->word[i], "v")==0)
			continue;    //As and can take any feature
		dn = dictionary_lookup(trg_dict, linkage->word[i]);
		//if (dn==NULL)
		//	continue;
	    if (dn!=NULL)
			trg_f = dn->f;
		strcpy(word, linkage->word[i]);
		morphological_adjuster(trg_dict, lex, word, linkage->feature_array[i], trg_f);
		string_delete(linkage->word[i]);
		linkage->word[i] = (char *) exalloc(strlen(word)+1);
		strcpy(linkage->word[i], word);
	}
}
#endif
