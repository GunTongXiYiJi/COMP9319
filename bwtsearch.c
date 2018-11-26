#include<stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<string.h>

#include"bwtsearch.h"

// argv[1]  delimiter
// argv[2]	bwt file   path 
// argv[3]	index folder path
// argv[4]	-m -n -a -i 
// argv[5]	"search pattern" 

int main(int argc, char* argv[]){

	int debug = 0;
	char c,max_char;
	int size,i,j,k,count,occ_exists,par_index;
	/* m1: 1 megabytes *
	 * partitions: (size of file) / m1 *
	 * remainder: (size of file) % m1 *
	 * par_index: index of partitions *
	 * max_char: last char in c_table */
	int m1 = (1<<20)/20;
	// int m1 = 5;
	int partitions;
	int remainder;
	FILE *fp;

/* ***********************build c_table & write occ_tables start******************** */

	/* handle \n delimiter */
	if(argv[1][1] == 'n'){
		argv[1][0] = 10;
	}
/* ********************check if occ file exists********************* */
	/* parse bwt file name */
	// char *bwt_name = strrchr(argv[2],'/');
	char *bwt_name = argv[2];
	if(strstr(argv[2],"/")){
		bwt_name = strrchr(argv[2],'/');
	}
	/* build occ file name */
	int bwt_name_len = strlen(bwt_name);
	char occ_name[5 + bwt_name_len];
	par_index = 1;
	sprintf(occ_name,"%s.%d",bwt_name,par_index);
	/* build occ file path */
	int occ_name_len = strlen(occ_name);
	int temp_len = strlen(argv[3]);
	char occ_path[temp_len + occ_name_len + 2];
	sprintf(occ_path,"%s%s",argv[3],occ_name);

	if(debug == 1){
		printf("occ_name is %s\n",occ_name);
		printf("occ_path is %s\n",occ_path);
	}

	if ( access(occ_path,0) != 0 ){
		/* occ does not exist */
		occ_exists = 0;
	}else{
		/* occ exists */
		occ_exists = 1;
	}

	/* open bwt file */
	if((fp = fopen(argv[2],"r"))== NULL){
		printf("file not exists");
	}
	size = filesize(fp);
	partitions = size / m1;

	/* if file less than 1MB */
	if(partitions == 0){
		occ_exists = 1;
	}

/* ********************check if occ file exists end********************* */

	/* initialize occ_table & c_table */
	int *occ_table = malloc(sizeof(int) * 128);//512Byte
	assert(occ_table != NULL);

	int *c_table = malloc(sizeof(int) * 128);//512Byte
	assert(c_table != NULL);

	for(i = 0 ;i < 128; i ++ ){
		c_table[i] = 0;
		occ_table[i] = 0;
	}

	/* count occurence and build occ_table */
	count = 0;
	while(c = fgetc(fp)){
		if(feof(fp)){
			break;
		}
		if(c == argv[1][0]){
			occ_table[0] ++;
		}else{
			occ_table[c] ++;	
		}

		if(occ_exists == 0){
			count ++;

			if(count % m1 == 0){
				par_index = count / m1;
				sprintf(occ_name,"%s.%d",bwt_name,par_index);
				sprintf(occ_path,"%s%s",argv[3],occ_name);

				FILE *fp2 = fopen(occ_path,"w");
				for(i = 0 ;i < 128; i ++){
					if(occ_table[i] > 0){
						fprintf(fp2,"%d,%d\n",i,occ_table[i]);
					}
				}
				fclose(fp2);
			}
		}
	}
	/* write the last occ file */
	if(count % m1 != 0 && occ_exists == 0){
		par_index = count / m1 + 1;
		sprintf(occ_name,"%s.%d",bwt_name,par_index);
		sprintf(occ_path,"%s%s",argv[3],occ_name);

		FILE *fp2 = fopen(occ_path,"w");
		for(i = 0 ;i < 128; i ++){
			if(occ_table[i] > 0){
				fprintf(fp2,"%d,%d\n",i,occ_table[i]);
			}
		}
		fclose(fp2);
	}

	/* build c_table */
	for(i = 0; i < 128; i++){
		if(occ_table[i] != 0){
			for(j = 0; j < i; j++){
				c_table[i] += occ_table[j];
			}
			max_char = i;
		}
	}

	if(debug == 1){
		for(i=0;i<128;i++){
			if(c_table[i] > 0 || i == 0 ){
				printf("c_table[%c] %d,occ_table[%c] %d\n",i,c_table[i],argv[1][0],occ_table[i]);	
			}
		}
	}
	fclose(fp);

/* **************************** search start************************** */
	int result[2];

	if(argv[4][1] == 'm' || argv[4][1] == 'n' || argv[4][1] == 'a'){
		
		m_search(argv[2],argv[3],argv[5],c_table,max_char,size,result,bwt_name,occ_name_len,temp_len);	
	}

	if(argv[4][1] == 'n'){

		printf("%d\n",n_search(result,c_table,argv[1][0],argv[2],argv[3],argv[5],bwt_name,occ_name_len,temp_len));
	}

	if(argv[4][1] == 'm'){
		printf("%d\n",result[1] - result[0] + 1);	
	}

	if(argv[4][1]=='a'){

		char * aux_path = malloc(sizeof(char) * 50);
		assert(aux_path != NULL);

		for(i = 0 ;i < 50; i ++ ){
			aux_path[i] = 0;
		}

    	sprintf(aux_path,"%s.aux",argv[2]);

    	FILE *aux_file = fopen(aux_path,"r");

    	int delimiter_count = occ_table[0];

    	int *delimiter_position = malloc(sizeof(int) * delimiter_count);
    	assert(delimiter_position != NULL);
    	memset(delimiter_position, 0, sizeof(int) * delimiter_count);

    	int *result_ = malloc(sizeof(int) * 5000);
    	assert(delimiter_position != NULL);
    	memset(result_, 0, sizeof(int) * 5000);

    	int n1 = 0;
    	i = 0;
    	while(fscanf(aux_file,"%d",&n1) != EOF){
  			delimiter_position[i] = n1;
  			i ++;
 		}

 		fclose(aux_file);

		/* -a search takes the result of m_search */
		int First = result[0];
		int Last = result[1];
		int ret = Last - First + 1;
		/* pF: F(suffix array) pointer *
		 * char_L: L(bwt) char */
		int i,pF,off_set;
		char char_L;

		/* if m_search has 0 result,do nothing */
		if( ret > 0 ){

			FILE *bwt = fopen(argv[2],"r");

			j=0;
			for(i = First; i <= Last; i ++){

				pF = i;

				while(1){

					char_L = bwt_seek(pF,bwt);
					off_set = occ(char_L,pF - 1,bwt, bwt_name,occ_name_len,temp_len, argv[3]);
					
					if(char_L == argv[1][0]){

						if(delimiter_position[off_set] == occ_table[0]){
							result_[j] = 1;
						}else{
							result_[j] = delimiter_position[off_set] + 1;
						}

						j++;
						break;
					}

					pF = c_table[char_L] + off_set + 1;
					if(char_L == argv[5][0] && pF <= Last && pF >= First){
						break;
					}
				}
			}
			quickSort_int(result_,0,j-1);

			for(i=0;i<j;i++){
				printf("%d\n",result_[i]);
			}

			free(result_);
			free(delimiter_position);
			free(aux_path);
			fclose(bwt);
		}
	}

	if(argv[4][1]=='i'){

		char char_L;
		int off_set;

		char * aux_path = malloc(sizeof(char) * 50);
		assert(aux_path != NULL);

		for(i = 0 ;i < 50; i ++ ){
			aux_path[i] = 0;
		}
    	sprintf(aux_path,"%s.aux",argv[2]);
    	FILE *aux_file = fopen(aux_path,"r");
    	int delimiter_count = occ_table[0];

    	int *delimiter_position = malloc(sizeof(int)*delimiter_count);
    	assert(delimiter_position != NULL);
    	memset(delimiter_position, 0, sizeof(int) * delimiter_count);

    	char *result_=malloc(sizeof(char) * 5000 * 5000);
    	assert(result_ != NULL);
    	memset(result_,0,sizeof(char) * 5000 * 5000);

    	int n1 = 0;
    	i = 0;
    	while(fscanf(aux_file,"%d",&n1) != EOF){
  			delimiter_position[i] = n1;
  			i ++;
 		}

 		char ch[] = " ";
 		int pF=0;
 		int end_delimiter = 0;
 		char *substr = strtok(argv[5],ch);
 		FILE *bwt = fopen(argv[2],"r");
 		i=0;
 		while (substr != NULL) {
 			if(i == 0){
 				end_delimiter = atoi(substr);
 				if(end_delimiter == 1){
 					end_delimiter = occ_table[0];
 				}
 				else{
 					end_delimiter = end_delimiter-1;
 				}
 				
 			}else{
 				pF = atoi(substr);
 			}  
 			i++;                            
            substr = strtok(NULL,ch);
        }   
   
        i = 0;
        
        while(1){
        	char_L = bwt_seek(pF,bwt);
        	off_set = occ(char_L, pF - 1,bwt, bwt_name,occ_name_len,temp_len, argv[3]);

        	if(char_L == argv[1][0]){

 	       		int temp = delimiter_position[off_set];

 	       		if(temp == end_delimiter){       			
        			break;
        		}
        		result_[i] = '\n';
        		i++;
        		pF = temp;
        	}
        	else{

        		result_[i] = char_L;
        		i++;
        		pF = c_table[char_L] + off_set + 1;	
        	}	
		}

		for(j = i-1; j >=0; j--){
			printf("%c",result_[j]);
		}
		printf("\n");

 		fclose(aux_file);
 		fclose(bwt);
 		free(delimiter_position);
 		free(result_);
 		free(aux_path);
	}
	free(c_table);
	free(occ_table);

	return 0;
}




/* ***********************Helper functions start******************** */

/* return size of a file */
int filesize(FILE *fp){
	int n;
	fpos_t fpos;
	
	/* get current position */ 
	fgetpos(fp, &fpos); 
	/* move file pointer to end of file */ 
	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	/* restore to original position */
	fsetpos(fp,&fpos); 
	
	return n;
}

void m_search(char *bwt_path, char *temp_folder_path,char *search_term,int *c_table, char max_char,int size,int *result,char *bwt_name,int occ_name_len, int temp_len){
	
	int First = 0;
	int Last = 0;
	int p,i,j;
	char c;
	char *P = search_term;

	p = strlen(P);
	i = p - 1;
	c = P[p - 1];
	First = c_table[c] + 1;
	if(First == 1){
		result[0] = First;
		result[1] = Last;
		return;
	}

	if(c != max_char){
		for(j = c + 1; j < 128; j++){
			if(c_table[j] != 0){
				Last = c_table[j];
				break;
			}
		}
	}else{
		Last = size;
	}
	FILE *bwt = fopen(bwt_path,"r");
	
	while( First <= Last && i >=1 ){
		c = P[i -1];
		First = c_table[c] + occ(c, First - 1, bwt, bwt_name,occ_name_len,temp_len, temp_folder_path) + 1;
		Last = c_table[c] + occ(c, Last, bwt, bwt_name,occ_name_len,temp_len, temp_folder_path);
		i = i - 1;
	}

	fclose(bwt);
	result[0] = First;
	result[1] = Last;
}

int occ(char c,int v,FILE *bwt, char *bwt_name,int occ_name_len, int temp_len,char *temp_folder_path){
	
	int m1 = (1<<20)/20;
	int n1,n2 = 0;
	int found = 0;
	int par_index = v / m1;
	int remainder = v % m1;

	if(par_index != 0){

		/* build occ file name */
		char occ_name[occ_name_len];
		sprintf(occ_name,"%s.%d",bwt_name,par_index);
		/* build occ file path */
		char occ_path[temp_len + occ_name_len + 2];
		sprintf(occ_path,"%s%s",temp_folder_path,occ_name);
		// printf("occ_path is %s\n",occ_path);
		FILE *fp = fopen(occ_path,"r");
		assert(fp != NULL);
		
		while(fscanf(fp,"%d,%d",&n1,&n2) != EOF){

			if(n1 == c){
				found = 1;
				break;
			}
		}
		if(found == 0){
			n2 = 0;
		}
		fclose(fp);
	}

	int i = 0;
	int count = 0;
	char c_bwt;

	fseek(bwt, par_index * m1, SEEK_SET);

	while(i < remainder){
		c_bwt = fgetc(bwt);
		if(c_bwt == c){
			count ++;
		}
		i ++;
	}

	rewind(bwt);

	return count + n2;
}

/* find a specific char in bwt file */
char bwt_seek(int pF,FILE *bwt){

	fseek(bwt, pF-1, SEEK_SET);
	char c = fgetc(bwt);
	rewind(bwt);

	return c;
}

int n_search(int *result,int *c_table,char delimiter,char *bwt_path,char *temp_folder_path,char *search_term,char *bwt_name,int occ_name_len, int temp_len){
	
	/* n_search takes the result of m_search */
	int First = result[0];
	int Last = result[1];
	int ret = Last - First + 1;

	/* pF: F(suffix array) pointer *
	 * char_L: L(bwt) char */
	int i,pF,off_set;
	char char_L;

	/* if m_search has 1 or 0 result */
	if( ret < 2 ){
		return ret;
	}

	FILE *bwt = fopen(bwt_path,"r");

	for(i = First; i <= Last; i ++){

		pF = i;
		while(1){

			char_L = bwt_seek(pF,bwt);

			if(char_L == delimiter){
				break;
			}
			off_set = occ(char_L,pF - 1,bwt, bwt_name,occ_name_len,temp_len, temp_folder_path);
			pF = c_table[char_L] + off_set + 1;
			
			if(char_L == search_term[0] && pF <= Last && pF >= First){
				ret --;
			}
		}
	}
	fclose(bwt);
	return ret;
}

void quickSort_int(int* arr,int startPos, int endPos){
	int i, j; 
	int key; 
	key = arr[startPos]; 
	i = startPos; 
	j = endPos; 
	while (i<j){
		while (arr[j] >= key && i<j)--j; 
		arr[i] = arr[j]; 
		while (arr[i] <= key && i<j)++i; 
		arr[j] = arr[i];
	}
	arr[i] = key; 
	if (i - 1>startPos) quickSort_int(arr, startPos, i - 1); 
	if (endPos>i + 1) quickSort_int(arr, i + 1, endPos);
}


/* ***********************Helper functions end******************** */
