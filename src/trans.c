/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/
#define FIRST_STRUCT TRUE
#define NORMAL_PARSE 0
#define TRANSLATE  1
#include "link-includes.h"

void printhelp(){
    printf("\n");
    printf("Usage: UniLink [OPTIONS...]");
    printf("\n\nUniLink is a Machine translation System based on Unification  Link Grammar");
    printf(" , an extention to Link Grammar\n");
    printf(" Options: \n");
    printf("-t :  Translate (Default action is parsing)\n");
    printf("-du:  Disable Unification\n");
    printf("-a :  All linkages are generated\n");
    printf("-o :  Output to file \n");
    printf("      There should be a directory named \"out\" in the current directory \n");

}

void setParameters(int argc, char* argv[], int* action, int* unify_features, int* all_linkages, int* out_to_file){
    int i;
    for (i=1; i<argc; i++){
        if (strcmp(argv[i],"-t")==0)
            *action=TRANSLATE;
        else if(strcmp(argv[i],"-du")==0)
            *unify_features=FALSE;
        else if(strcmp(argv[i],"-a")==0)
            *all_linkages=TRUE;
        else if(strcmp(argv[i],"-o")==0)
            *out_to_file=TRUE;
        else if(strcmp(argv[i],"-h")==0){
            printhelp();
            exit(0);
        }
    }
}
void read_input_sentences(char  input_string[][200]){
    int i=0;
    char  instr[200];
	FILE *in = fopen ("in.txt", "r+" );
	printf("Reading inout from in.txt...\n");
	if (in==NULL){
        printf("\nNo such file gound in the current directory\n");
        strcpy(input_string[0], "end");
	}
	while(fgets (instr, 200, in)){
	    	if ((*instr!='%')&&(*instr!='\0')&&(*instr!='\n'))
                strcpy(input_string[i++], instr);

	}
    strcpy(input_string[i], "end");

}
void extract_sent(Linkage linkage, char* output_string){
	int i;
	*output_string='\0';
	for (i=0; i<linkage->num_words; i++){
		strcat(output_string, linkage->word[i]);
		strcat(output_string, " ");
	}

}

/*******************************************************************************************
*   This functions translats a list of sentences in the input_string array,
*   terminates when it finds "end" in the array
*   Inputs: input_string: Array of sentences
*           all_linkages: if true, translate all possible linkages of the sentence
*           out_to_file : if true, outputs the results to seperate files in the out directory,
*                       the out files names would be something like :
                        src-x-y: Contains a linkage for the source sentence, x is the index
                                 of sentence in the array and y is the index of linkage
                        trg-x-y-z: Contains a linklage for the target sentence, x is the index
                                   of sentence in the array, y is the index of linkage and z
                                   is the zth translation of that linkage x-y(a linkage in the
                                   source language may have more than one correspondent target
*                                  linkage)
*******************************************************************************************/
void translate(char  input_string[][200], int all_linkages, int out_to_file){
    Dictionary    dict;
    Parse_Options opts;
    Sentence      sent;
    Linkage       src_linkage, trg_linkage;
	Transfer trans;
    char *        diagram;
	FILE *		  fp;
    int           i,  j, num_src_linkages, num_trg_linkages;
    //char        filename[30], txfilename[30], filenum[4];
    char          txfilename[30];
    char          output_string[200];
	int n;

	opts  = parse_options_create();
	parse_options_set_verbosity (opts, FALSE);
	parse_options_set_display_walls(opts, TRUE);
	parse_options_set_display_postscript(opts, TRUE);

    dict  = dictionary_create("4.0.dict", "4.0.knowledge", NULL, NULL, "morphology/morphemes.dict");
	if (!dict){
		fprintf(stderr, "%s\n", lperrmsg);
	    parse_options_delete(opts);
		printf("size : %lld", space_in_use);
		exit(0);
	}
	trans=transfer_create("translation/mapfile.txt", "translation/links_list.txt","translation/lexicon.txt", "target/trg.dict" );
	if (!trans){
		fprintf(stderr, "%s\n", maperrmsg);
		fprintf(stderr, "%s\n", lperrmsg);
		printf("size : %lld", space_in_use);
		exit(0);
	}

	for (n=0; strcmp (input_string[n],"end")!=0; n++ ){

		sent = sentence_create(input_string[n], dict);
		if (!sent){
			fprintf(stderr, "%s\n", lperrmsg);
			parse_options_delete(opts);
			printf("size : %lld", space_in_use);
			exit(0);
		}
		printf ("\n************************************\n%d-%s\n************************************\n", n+1, input_string[n]);
		num_src_linkages = sentence_parse(sent, opts);

		num_src_linkages = (all_linkages) ? num_src_linkages: (num_src_linkages!=0)*1;

		for (i=0; i<num_src_linkages;i++) {
			src_linkage = linkage_create(i, sent, opts);
			diagram = linkage_print_diagram(src_linkage);

			printf ("\nLinkage No. %d-%d\n\n", n+1, i+1);
			printf("%s\n", diagram);
			if (out_to_file){
				sprintf(txfilename,"out/src-%d-%d.txt", n+1, i+1);
				fp=fopen(txfilename,"w+");
				if (fp==NULL){
                    fprintf(stderr, "%s%s\n", "Unable to open ", txfilename);
                    printf("size : %lld", space_in_use);
                    exit(0);
				}
				fprintf(fp,"%s",diagram);
				fclose(fp);
			}
			string_delete(diagram);

			num_trg_linkages = transfer_linkage_driver(trans, src_linkage);
			if(num_trg_linkages==0){
				fprintf(stderr, "%s\n", maperrmsg);
			}
			else{
				parse_options_set_display_walls (trans->opts, TRUE);
				for (j=0; j<num_trg_linkages; j++){
					//second parameter should always be 0 !?
					trg_linkage=trans_linkage_create(trans, 0, trans->sent[j], trans->opts);
					diagram = linkage_print_diagram(trg_linkage);

					printf ("\nTranslation No. %d_%d_%d\n\n", n+1,i+1,j+1);
                    printf("Translation: %s\n", output_string);
					printf("%s\n", diagram);
					if (out_to_file){
                        sprintf(txfilename,"out/trg-%d-%d-%d.txt", n+1, i+1, j+1);
						fp=fopen(txfilename,"w+");
                        if (fp==NULL){
                            fprintf(stderr, "%s%s\n", "Unable to open ", txfilename);
                            printf("size : %lld", space_in_use);
                            exit(0);
                        }
                        extract_sent (trg_linkage, output_string);
                        fprintf(fp, "Translation:%s\n", output_string);
						fprintf(fp,"%s",diagram);
						fclose(fp);
					}
					string_delete(diagram);
					linkage_delete(trg_linkage);
				}
			}
			linkage_delete(src_linkage);
		}
		fprintf(stderr, "%s\n", lperrmsg);
		sentence_delete(sent);

	}


	transfer_delete(trans);
    dictionary_delete(dict);
    parse_options_delete(opts);

}
/*******************************************************************************************
*   This functions parses a list of sentences and outputs the results to seperate files in 
*   the out directory,
*   The files are indexed as out_x_y, where x is the index of the sentence in the array and y
*   is the index of the linkage
*******************************************************************************************/
void normal_parse(char input_string[][200], int unify_features, int all_linkages, int out_to_file){

    Dictionary    dict;
    Parse_Options opts;
	int n;
	Sentence      sent;
    Linkage       linkage;
    char *        diagram;
    int           i, num_linkages;
    char          txfilename[30];
	FILE *		  fp;


	opts  = parse_options_create();
	parse_options_set_display_walls(opts, TRUE);
	parse_options_set_display_postscript(opts, TRUE);
	parse_options_set_unify_features(opts, unify_features);

    dict  = dictionary_create("4.0.dict", "4.0.knowledge", NULL, NULL, "morphology/morphemes.dict");
	if (!dict){
		fprintf(stderr, "%s\n", lperrmsg);
	    parse_options_delete(opts);
		printf("size : %lld", space_in_use);
		exit(0);
	}

	//setting opts->unify_features to TRUE enables unification
	for (n=0; strcmp (input_string[n],"end")!=0; n++ ){
		sent = sentence_create(input_string[n], dict);
		if (!sent){
			fprintf(stderr, "%s\n", lperrmsg);
			dictionary_delete(dict);
			parse_options_delete(opts);
			exit(0);
		}
		printf ("\n************************************\n%d-%s\n************************************\n", n+1, input_string[n]);
		num_linkages = sentence_parse(sent, opts);
		num_linkages = (all_linkages) ? num_linkages : (num_linkages!=0)*1 ;
		//Echos all linkages to screen, ps file and text file in ./out directory
		for (i=0; i<num_linkages; ++i) {
			linkage = linkage_create(i , sent, opts);
			diagram = linkage_print_diagram(linkage);

			printf ("\n Linkage No. %d-%d\n\n", n+1, i+1);
			printf("%s\n", diagram);
			if (out_to_file){
				sprintf(txfilename,"out/src-%d-%d.txt", n+1, i+1);
				fp=fopen(txfilename,"w+");
                if (fp==NULL){
                    fprintf(stderr, "%s%s\n", "Unable to open ", txfilename);
                    printf("size : %lld", space_in_use);
                    exit(0);
                }
				fprintf(fp,"%s",diagram);
				fclose(fp);
			}
			string_delete(diagram);
			linkage_delete(linkage);
 		}
		sentence_delete(sent);
	}

    dictionary_delete(dict);
    parse_options_delete(opts);

}


int main(int argc, char* argv[]) {
	int action = NORMAL_PARSE;
	int unify_features = TRUE;
	int all_linkages = FALSE;
	int out_to_file = FALSE;
    setParameters(argc, argv, &action, &unify_features, &all_linkages, &out_to_file);
	//Test examples
	//data1

	char  input_string[200][200];
	read_input_sentences(input_string);

	if (action==TRANSLATE)
        translate(input_string, all_linkages, out_to_file);
    else
        normal_parse(input_string, unify_features, all_linkages, out_to_file  );


	printf("\n space in use : %lld", space_in_use);
	printf("\n external space in use : %lld", external_space_in_use);

    return 0;
}
