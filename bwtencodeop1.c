#include<stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<string.h>

#include"bwtencode.h"

// argv[1] delimiter
// argv[2]	temp folder   path 
// argv[3]	original text path
// argv[4]	bwt file      path

int main(int argc, char* argv[]){
	int debug = 0;
	char c;
	char s_filename[] = "/s.txt"; //file to store the "S" characters
	int size,j,k,i=0;
	FILE *fp;
	
/* ***********************acquire text content start******************** */

	/* handle \n delimiter */
	if(argv[1][1] == 'n'){
		argv[1][0] = 10;
	} 
	
	if((fp = fopen(argv[3],"rb"))== NULL){
		printf("file not exists");
	}
	
	/* acquire size of string */
	size = filesize(fp);

	char *txt = malloc(size);
	assert(txt != NULL);

	/* put string into buffer */
	while(c = fgetc(fp)){
		if(feof(fp)){
			break;
		}
		txt[i] = c;
		i ++;
	}
	
/* ***********************acquire text content end******************** */

/* ***********************write S file start******************** */
	char s_filepath[strlen(argv[2]) + strlen(s_filename)];
	int b = '\n';

	/* build temp file path */
	strcpy(s_filepath,argv[2]);
	strcat(s_filepath,s_filename);
	
	FILE *fp1 = fopen(s_filepath,"w");
	for(i = 0; i < size - 1; i ++){
		int mlen = size - i - 1;
		if(strcmp2(&txt[i], &txt[i + 1],argv[1],mlen,2) < 0){
			fprintf(fp1,"%d",i);
			fputc('\n',fp1);
		}
	}
	fprintf(fp1,"%d",i);
	fclose(fp1);

/* ***********************write S file end******************** */

/* ***********************bucket sort S array start******************** */
	
    /* s_num: total number of s chars         *
	 * bucket_num_s: variety of s chars       *
	 * bucket_array: non-reoccuring s chars   *
	 * bucket_s_array: non-reoccuring s chars *
	 * with delimiter moved to the front      */
	char bucket_array[128];
	char bucket_s_array[128];
	int n,flag,bucket_num_s = 0, s_num = 0;

	/* open s.txt which contains index of s chars */
	FILE *fp2 = fopen(s_filepath,"r");
	while(fscanf(fp2,"%d",&n) != EOF){
		flag = 1;
		for(i = 0; i < bucket_num_s; i ++){
			if(bucket_array[i] == txt[n]){
				flag = -1;
				break;
			}
		}
		if(flag > 0){
			bucket_array[i] = txt[n];
			bucket_num_s ++;
		}
		s_num ++;
	}

	/* sort bucket_array in lexcial order*/
	quickSort(&bucket_array[0],0,bucket_num_s-1);
	/* move whatever is the delimiter to the front */
	if(bucket_array[0] != argv[1][0]){
		for(i = 0; i < bucket_num_s; i ++){
			if(bucket_array[i] == argv[1][0]){
				break;
			}
		}

		int pos = i;
		j = 0;
		bucket_s_array[0] = argv[1][0];

		for(i = 1; i < bucket_num_s; i ++){
			if(bucket_array[j] != argv[1][0]){
				bucket_s_array[i] = bucket_array[j];
			}
			j ++;
		}
	}else{
		for(i = 0; i < bucket_num_s; i ++){
			bucket_s_array[i] = bucket_array[i];
		}
	}

	if(debug == 1){/* debug ignore */
		for(i = 0; i < bucket_num_s; i ++){
			printf("bucket_s_array  %c\n",bucket_s_array[i]);
		}

		for(i = 0; i < bucket_num_s; i ++){
			printf("bucket_array (small) %c\n",bucket_array[i]);
		}
	}
	
	
	/* build the bucket_sorted_s : double array,  *
	 * records the sorted small chars' index      *
	 * sb_num: number of small chars respectively *
	 * s_array: array of all the small chars' index */
	int sum;
	int sb_num[bucket_num_s];
	int *bucket_sorted_s[bucket_num_s];
	int *s_array = malloc(sizeof(int)*s_num);
	assert(s_array != NULL);
	
	rewind(fp2);
	
	i = 0;
 	while(fscanf(fp2,"%d",&n) != EOF){
 		s_array[i] = n;
 		i ++;
 	}
	
	for(i = 0; i < bucket_num_s; i++){
		sum = 0;
		for(j = 0; j < s_num; j++){
			if( txt[s_array[j]] == bucket_s_array[i]){
				sum ++;
			}
		}
		sb_num[i] = sum;
	}

	for(i = 0; i < bucket_num_s; i ++){
		bucket_sorted_s[i] = malloc(sizeof(int) * sb_num[i]);
		assert(bucket_sorted_s[i] != NULL);
	}
	for(i = 0; i < bucket_num_s; i++){
		for(j = 0; j < sb_num[i]; j++){
			bucket_sorted_s[i][j] = -1;
		}
	}

	/* record the next insert pos,saves searching time */
	int position[bucket_num_s];
	for(i = 0; i < bucket_num_s; i++){
		position[i] = 0;
	}

	/* insert all the s chars into bucket_sorted_s according to *
	 * their order in bucket_s_array and position in position[] */
	for(i = 0; i < s_num; i++){
		for(j = 0; j < bucket_num_s; j++){
			if(txt[s_array[i]] == bucket_s_array[j]){
				bucket_sorted_s[j][position[j]] = s_array[i];
				position[j] ++;		
			}
		}
	}

	for(i = 1; i < bucket_num_s; i++){
		quickSort_suffix(&bucket_sorted_s[i][0],0,sb_num[i]-1,txt,argv[1],size);
	}
	
	// free(txt);
	if(debug == 1){/* debug ignore */
		for(i = 0; i < bucket_num_s; i++){
			for(j = 0; j < sb_num[i]; j++){
				printf("bucket_sorted_s[%d][%d] is %d\n",i,j,bucket_sorted_s[i][j]);////////////
			}
		}
	}

	fclose(fp2); 	
/* ***********************bucket sort S array end******************** */
printf("bucket sort S array end\n");
/* ***********************bucket txt start******************** */

	/* size: original file size                 *
	 * bucket_num_txt: variety of text chars    *
	 * bucket_array: non-reoccuring txt chars   *
	 * bucket_txt_array: non-reoccuring txt chars *
	 * sorted with delimiter moved to the front */
	int bucket_num_txt = 0;
	char bucket_txt_array[128];
	j = 0;

	/* build bucket_array */
	while(j < size){
		flag = 1;
		for(i = 0; i < bucket_num_txt; i ++){
			if(bucket_array[i] == txt_seek(j,fp)){
				flag = -1;
				break;
			}
		}
		if(flag > 0){
			bucket_array[i] = txt_seek(j,fp);
			bucket_num_txt ++;
		}
		j++;
	}
	printf("111111111111111111111\n");
	/* sort bucket_array in lexcial order*/
	quickSort(&bucket_array[0],0,bucket_num_txt-1);	

	/* move whatever is the delimiter to the front */
	if(bucket_array[0] != argv[1][0]){
		for(i = 0; i < bucket_num_txt; i ++){
			if(bucket_array[i] == argv[1][0]){
				break;
			}
		}

		int pos = i;
		j = 0;
		bucket_txt_array[0] = argv[1][0];

		for(i = 1; i < bucket_num_txt; i ++){
			if(bucket_array[j] != argv[1][0]){
				bucket_txt_array[i] = bucket_array[j];
			}
			j ++;
		}
	}else{
		for(i = 0; i < bucket_num_txt; i ++){
			bucket_txt_array[i] = bucket_array[i];
		}
	}

	if(debug == 1){/* debug ignore */
		
		for(i = 0; i < bucket_num_txt; i ++){
			printf("bucket_txt_array  %c\n",bucket_txt_array[i]);
		}
		
		for(i = 0; i < bucket_num_txt; i ++){
			printf("bucket txt %c\n",bucket_array[i]);
		}
	}
printf("22222222222222222222222222\n");
	/* txt_num: number of all chars in text respectively **/
	int txt_num[bucket_num_txt];
	
	for(i = 0; i < bucket_num_txt; i++){
		sum = 0;
		for(j = 0; j < size; j++){
			if(txt_seek(j,fp) == bucket_txt_array[i]){
				sum ++;
			}
		}
		txt_num[i] =  sum;
	}

	/* bucket_sorted_txt: double array of final sorted suffix index */ 
	int *bucket_sorted_txt[bucket_num_txt];
	int pos[bucket_num_txt];
	/* insert small chars from bucket_sorted_s    *
	 * into bucket_sorted_txt in reverse order of *
	 * bucket_sorted_txt, pos is the insert position */
	for(i = 0; i < bucket_num_txt; i ++){
		pos[i] = txt_num[i] - 1;
	}

	for(i = 0; i < bucket_num_txt; i ++){
		bucket_sorted_txt[i] = malloc(sizeof(int) * txt_num[i]);
		assert(bucket_sorted_txt[i] != NULL);
	}

	for(i = 0; i < bucket_num_txt; i++){
		for(j = 0; j < txt_num[i]; j++){
			bucket_sorted_txt[i][j] = -1;
		}
	}

	for(i = 0; i < bucket_num_s; i++){
		for(j = sb_num[i] - 1; j >= 0; j--){
			for(k = 0; k < bucket_num_txt; k ++){
				printf("txt_seek is %c,txt[] is %c\n",txt_seek(bucket_sorted_s[i][j],fp),txt[bucket_sorted_s[i][j]]);
				if(txt_seek(bucket_sorted_s[i][j],fp) == bucket_txt_array[k]){
					break;
				}
			}
			bucket_sorted_txt[k][pos[k]] = bucket_sorted_s[i][j];
			pos[k]--;
		}
	}
	
	if(debug == 1){/* debug ignore */
		for(i = 0; i < bucket_num_txt; i++){
			for(j = 0; j < txt_num[i]; j++){
				printf("bucket_sorted_txt[%d][%d] is %d\n",i,j,bucket_sorted_txt[i][j]);////////////
			}
		}
	}
	
	for(i = 0; i < bucket_num_s; i ++){
		free(bucket_sorted_s[i]);
	}

	/* record the next insert pos,saves searching time */
	for(i = 0; i < bucket_num_txt; i++){
		pos[i] = 0;
	}
	
	/* v: now smalls are in the bucket_sorted_txt, *
	 * traverse the smalls to insert the small - 1 index */
	int v,m,q,contin = 0;
printf("3333333333333333333333333333333\n");
	for(i = 0; i < bucket_num_txt; i++){
		for(j = 0; j < txt_num[i]; j++){
			v = bucket_sorted_txt[i][j] - 1;
			if(v < 0){
				continue;
			}
			/* if v is s char,it's already in*/
			contin = 0;
			if(contin == 1){
				continue;
			}
			/* find the row index to insert */
			for(k = 0; k < bucket_num_txt; k ++){
				printf("txt_seek(v,fp) is %c\n",txt_seek(v,fp));
				printf("txt[v] is %c\n",txt[v]);
				if(txt_seek(v,fp) == bucket_txt_array[k]){
					break;
				}
			}
			printf("bucket_sorted_txt[k][pos[k]] is %d\n",bucket_sorted_txt[k][pos[k]]);
			printf("5555555555555555555555\n");
			if(bucket_sorted_txt[k][pos[k]] == -1){
				bucket_sorted_txt[k][pos[k]] = v;
				pos[k] ++;		
			}
		}
	}
	printf("4444444444444444444444444444\n");
	free(s_array);
	
	if(debug ==1 ){/* debug ignore */
		for(i = 0; i < bucket_num_txt; i++){
			for(j = 0; j < txt_num[i]; j++){
				printf("bucket_sorted_txt[%d][%d] is %c\n",i,j,txt_seek(bucket_sorted_txt[i][j]-1,fp));////////////
			}
		}
		printf("\n");
		for(i = 0; i < bucket_num_txt; i++){ 
			for(j = 0; j < txt_num[i]; j++){
				printf("bucket_sorted_txt[%d][%d] is %d\n",i,j,bucket_sorted_txt[i][j]);////////////
			}
		}
	}

	/* Write bwt result to file */
	FILE *fp3 = fopen(argv[4],"w");
	for(i = 0; i < bucket_num_txt; i++){ 
		for(j = 0; j < txt_num[i]; j++){
			if(bucket_sorted_txt[i][j] > 0){
				fputc(txt_seek(bucket_sorted_txt[i][j]-1,fp),fp3);		
			}else{
				fputc(argv[1][0],fp3);
			}
		
		}
	}
	fclose(fp3);
/* ***********************bucket txt end******************** */
	fclose(fp);
	for(i = 0; i < bucket_num_txt; i ++){
		free(bucket_sorted_txt[i]);
	}
	remove(s_filepath);
	free(txt);
	/* Write aux file */
	// FILE *fp4;
	// if((fp4 = fopen(argv[4],"r"))== NULL){
	// 	printf("file not exists");
	// }

	// char *bwt = malloc(size);
	// assert(bwt != NULL);
	// i = 0;

	// /* put string into buffer */
	// while(c = fgetc(fp4)){
	// 	if(feof(fp4)){
	// 		break;
	// 	}
	// 	bwt[i] = c;
	// 	i ++;
	// }
	// fclose(fp4);

	return 0;
}

/************************Helper functions start******************** */

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

void quickSort(char* arr,int startPos, int endPos){
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
	if (i - 1>startPos) quickSort(arr, startPos, i - 1); 
	if (endPos>i + 1) quickSort(arr, i + 1, endPos);
}

void quickSort_suffix(int* arr,int startPos, int endPos,
	char *txt,char *d,int size){

	int i, j; 
	int key; 
	key = arr[startPos]; 
	i = startPos; 
	j = endPos;

	int mlen,sh;

	while (i<j){

		if(arr[j] > key){
        		sh = 2;
        		mlen = size - key - 1;
    	}else{
    		sh = 1;
    		mlen = size - arr[j] - 1;
    	}
		while (strcmp2(&txt[arr[j]],&txt[key],d,mlen,sh)>=0 && i<j){
			--j;
			if(arr[j] > key){
	        		sh = 2;
	        		mlen = size - key - 1;
	    	}else{
	    		sh = 1;
	    		mlen = size - arr[j] - 1;
	    	}			

		}
		arr[i] = arr[j];

		if(arr[i] > key){
        		sh = 2;
        		mlen = size - key - 1;
    	}else{
    		sh = 1;
    		mlen = size - arr[i] - 1;
    	}

    	while (strcmp2(&txt[arr[i]],&txt[key],d,mlen,sh)<=0 && i<j){
			++i;
			if(arr[i] > key){
        		sh = 2;
        		mlen = size - key - 1;
	    	}else{
	    		sh = 1;
	    		mlen = size - arr[i] - 1;
	    	}
		}
		arr[j] = arr[i];
	}
	arr[i] = key; 
	if (i - 1>startPos){
		quickSort_suffix(arr, startPos, i - 1,&txt[0],d,size);
	} 
	if (endPos>i + 1){
		quickSort_suffix(arr, i + 1, endPos,&txt[0],d,size);
	}

}

int strcmp2(char *s1,char *s2,char *d,int len,int sh){
	char c1,c2;
	int i = 0;
	int ret = 0;

	/* sh for shorter,pre set the return value */
	if(sh == 2){
		ret = 1;
	}
	else{
		ret=-1;
	}

	for(i = 0; i < len; i++){
		c1 = s1[i];
		c2 = s2[i];

		if(c1 == c2 && c2 == d[0]){
			return ret;
		}else if(c2 == d[0]){
			return 1;
		}else if(c1 == d[0]){
			return -1;	
		}else if(c1 > c2){
			return 1;
		}else if(c1 < c2){
			return -1;
		}
	}
	return ret;
}

char txt_seek(int index, FILE *txt){
	if(index < 0){
		printf("fffffffkkkkkkkk\n");
	}
	if(index > 0){
		fseek(txt, index, SEEK_SET);
	}

	
	char c = fgetc(txt);
	rewind(txt);
	return c;
}
/* ***********************Helper functions end******************** */
