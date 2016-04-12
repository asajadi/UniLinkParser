/*********************************************************************************
*	Armin Sajadi, 2004
*		
*********************************************************************************/

#include "link-includes.h"
#include "morpho_structures.h"
void parse_file(char* in_file,    Dictionary    dict);
int main() {

    Dictionary    dict;
	wchar_t       in_file[50];
    FILE *fp1;
	initial_psalpha();
	dict  = dictionary_create("farsi.dict", NULL, NULL, NULL);
    if (!dict)
	{
		fprintf(stderr, "%s\n", lperrmsg);
		printf("size : %d", space_in_use);
		getch();
		exit(0);
	}
	while(1)
	{
		printf("\nInput>");
 		wscanf(L"%s",in_file);
		if (!wcscmp(in_file,L"quit"))
			break;
		if (!in_file[0])
			continue;
		fp1=_wfopen(in_file,L"rb");

		if(fp1==NULL)
		{
			printf("Could not open the file!");
			continue;
		}
		else
			fclose(fp1);
		parse_file(in_file, dict);
//		test_decom_list(decom_list);

	}
	dictionary_delete(dict);
    return 0;
}
void parse_file(char* in_file,    Dictionary    dict)
{
	char          wr[MWL];
	FILE *fp1;
	wchar_t* file_name="out.txt";
    kaleme decom_list,last;
	fp1=fopen(file_name,"wb");
	fclose(fp1);
	fp1=_wfopen(in_file,L"rb");
	if(fp1==NULL)
	{
		printf("Could not open the file!");
		getch();
		exit(0);
	}
	getwc(fp1);
	while(TRUE)
	{
		if feof(fp1)
			break;
		fwscanf(fp1,L"%s",inword);
		decom_list=morpho_analyse(inword, dict,1);
  	    output_file();	
    	if(decom_list)
		{
	      free_decom_list(decom_list);
		}
	}
	fclose(fp1);

}
