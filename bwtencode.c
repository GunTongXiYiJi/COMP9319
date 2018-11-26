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
	printf("in main\n");
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
	fclose(fp);
/* ***********************acquire text content end******************** */

/* ***********************write S file start******************** */
	printf("write S file start\n");
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
	printf("bucket sort S array start\n");
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
	
	if(debug == 1){/* debug ignore */
		for(i = 0; i < bucket_num_s; i++){
			for(j = 0; j < sb_num[i]; j++){
				printf("bucket_sorted_s[%d][%d] is %d\n",i,j,bucket_sorted_s[i][j]);////////////
			}
		}
	}

	fclose(fp2); 	
/* ***********************bucket sort S array end******************** */

/* ***********************bucket txt start******************** */
	printf("bucket txt start\n");
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
			if(bucket_array[i] == txt[j]){
				flag = -1;
				break;
			}
		}
		if(flag > 0){
			bucket_array[i] = txt[j];
			bucket_num_txt ++;
		}
		j++;
	}

	/* sort bucket_array in lexcial order*/
	quickSort(&bucket_array[0],0,bucket_num_txt-1);	
	printf("11111111111111111111\n");
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
			}else{
				j ++;
			}
			j ++;
		}
	}else{
		for(i = 0; i < bucket_num_txt; i ++){
			bucket_txt_array[i] = bucket_array[i];
		}
	}

	if(debug == 0){/* debug ignore */
		
		for(i = 0; i < bucket_num_txt; i ++){
			printf("bucket_txt_array  %c\n",bucket_txt_array[i]);
		}
		
		for(i = 0; i < bucket_num_txt; i ++){
			printf("bucket txt %c\n",bucket_array[i]);
		}
	}
	printf("2222222222222222222222\n");
	/* txt_num: number of all chars in text respectively **/
	int txt_num[bucket_num_txt];
	
	for(i = 0; i < bucket_num_txt; i++){
		sum = 0;
		for(j = 0; j < size; j++){
			if(txt[j] == bucket_txt_array[i]){
				sum ++;
			}
		}
		txt_num[i] =  sum;
	}
	printf("333333333333333333333\n");
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
	printf("4444444444444444444\n");
	for(i = 0; i < bucket_num_s; i++){
		for(j = sb_num[i] - 1; j >= 0; j--){
			for(k = 0; k < bucket_num_txt; k ++){
				if(txt[bucket_sorted_s[i][j]] == bucket_txt_array[k]){
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
	printf("555555555555555555555\n");
	for(i = 0; i < bucket_num_s; i ++){
		free(bucket_sorted_s[i]);
	}

	/* record the next insert pos,saves searching time */
	for(i = 0; i < bucket_num_txt; i++){
		pos[i] = 0;
	}
	printf("pos33 is %d,bucket_num_txt is %d\n",pos[33],bucket_num_txt);
	/* v: now smalls are in the bucket_sorted_txt, *
	 * traverse the smalls to insert the small - 1 index */
	int v,m,q,contin = 0;
	printf("666666666666666666666\n");
	/* build temp_aux_path */
	char temp_aux[] = "/temp.aux";
	char temp_aux_path[strlen(argv[2]) + strlen(temp_aux)];
	sprintf(temp_aux_path,"%s%s",argv[2],temp_aux);
	printf("fffffffffffffffffffffffff\n");
	FILE *ta = fopen(temp_aux_path,"w");
	int aux_len = 0;
	printf("txt_num[0] is %d\n",txt_num[0]);
	for(i = 0; i < bucket_num_txt; i++){
		printf("i is %d\n",i);
		for(j = 0; j < txt_num[i]; j++){
			// // if(j%5000 == 0){
			// 	printf("j is %d\n",j);
			// // }
			if(j == 339){
				printf("bucket_sorted_txt[i][j] is %d\n",bucket_sorted_txt[i][j]);
			}
			v = bucket_sorted_txt[i][j] - 1;
			if(j == 339){
				printf("v is %d\n",v);
				printf("txtv is %c,%d\n",txt[v],txt[v]);
				printf("kkkkkkkkkkkkkkkkkkkkkkk\n");
			}
			if(v < 0){
				fprintf(ta,"%d\n",size - 1);
				aux_len ++;
			}else if(txt[v] == argv[1][0]){
				fprintf(ta,"%d\n",v);
				aux_len ++;
			}
			if(j == 339){
				printf("v is %d\n",v);
				printf("jjjjjjjjjjjjjjjjjjjjjjjj\n");
			}
			if(v < 0){
				continue;
			}
			/* if v is s char,it's already in*/
			if(j == 339){
				printf("v is %d\n",v);
				printf("LLLLLLLLLLLLLLL\n");
			}
			contin = 0;
			if(contin == 1){
				continue;
			}
			/* find the row index to insert */
			for(k = 0; k < bucket_num_txt; k ++){
				if(txt[v] == bucket_txt_array[k]){
					printf("txt[v]  is %c,bucket_txt_array[k] is %c,k is %d\n",txt[v],bucket_txt_array[k],k);
					break;
				}
			}
			if(j == 339){
				printf("v is %d\n",v);
				printf("MMMMMMMMMMMMMMMM\n");
				printf("k is %d,pos[k] is %d\n",k,pos[k]);
			}
			printf("pos33 is %d\n",pos[33]);
			if(bucket_sorted_txt[k][pos[k]] == -1){
				bucket_sorted_txt[k][pos[k]] = v;
				pos[k] ++;		
			}
			if(j == 339){
				printf("PPPPPPPPPPPPPPPPPPPP\n");
			}
		}
	}
	printf("gggggggggggggggggggggggg\n");
	free(s_array);
	fclose(ta);
	printf("777777777777777777777\n");
	if(debug ==1 ){/* debug ignore */
		for(i = 0; i < bucket_num_txt; i++){
			for(j = 0; j < txt_num[i]; j++){
				printf("bucket_sorted_txt[%d][%d] is %c\n",i,j,txt[bucket_sorted_txt[i][j]-1]);////////////
			}
		}
		printf("\n");
		for(i = 0; i < bucket_num_txt; i++){ 
			for(j = 0; j < txt_num[i]; j++){
				printf("bucket_sorted_txt[%d][%d] is %d\n",i,j,bucket_sorted_txt[i][j]);////////////
			}
		}
	}
printf("88888888888888888888\n");
	/* Write bwt result to file */
	FILE *fp3 = fopen(argv[4],"w");
	for(i = 0; i < bucket_num_txt; i++){ 
		for(j = 0; j < txt_num[i]; j++){
			if(bucket_sorted_txt[i][j] > 0){
				fputc(txt[bucket_sorted_txt[i][j]-1],fp3);		
			}else{
				fputc(argv[1][0],fp3);
			}
		
		}
	}
	fclose(fp3);
/* ***********************bucket txt end******************** */
	
	free(txt);
	for(i = 0; i < bucket_num_txt; i ++){
		free(bucket_sorted_txt[i]);
	}
	remove(s_filepath);
printf("9999999999999999999999999\n");
	/* Write aux file */
	char *bwt_path = argv[4];
	char aux_path[5 + strlen(bwt_path)];
	sprintf(aux_path,"%s.aux",bwt_path);
	// printf("ccccccccccccccccccccccccc\n");
	printf("temp_aux_path is %s\n",temp_aux_path);
	ta = fopen(temp_aux_path,"r");
	assert(ta != NULL);
	// printf("eeeeeeeeeeeeeeeeeeeee\n");
	int n1;
	// printf("fffffffffffffffffffff\n");
	printf("aux_len is %d\n",aux_len);
	// int temp_arr[aux_len];
	// printf("gggggggggggggggggggggggg\n");
	// int temp_arr_sort[aux_len];
	int *temp_arr = malloc(sizeof(int) * aux_len);
	int *temp_arr_sort = malloc(sizeof(int) * aux_len);
	// printf("dddddddddddddddddddddddddd\n");
	i = 0;
	while(fscanf(ta,"%d",&n1) != EOF){
		temp_arr[i] = n1;
		temp_arr_sort[i] = n1;
		i ++;
	}
	fclose(ta);
	printf("aaaaaaaaaaaaaaaaaaaaaaaa\n");
	quickSort_int(&temp_arr_sort[0],0,aux_len-1);
	FILE *aux = fopen(aux_path,"w");
	// printf("aux_path is %s\n", aux_path);
	// printf("len is %d\n", len);
	for(i = 0;i < aux_len;i ++){
		for(j = 0;j < aux_len;j++){
			// printf("temp_arr[i] is %d,temp_arr_sort[j] is %d\n",temp_arr[i],temp_arr_sort[j]);
			if(temp_arr[i] == temp_arr_sort[j]){
				break;
			}
		}
		fprintf(aux,"%d\n",j + 1);
	}
	fclose(aux);
	printf("bbbbbbbbbbbbbbbbbbbbbbbbbb\n");
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
	free(temp_arr);
	free(temp_arr_sort);
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

/* ***********************Helper functions end******************** */
