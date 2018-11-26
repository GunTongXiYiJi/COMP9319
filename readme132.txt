------------------------------------------------------------------
<<small>>
bucket_sorted_s //small字符二维数组
bucket_array
bucket_num_s 2	//len(bucket_array)small字符种类总数
sb_num[i] 1 4	//small字符各类别数量
s_array: array of all the small chars' index */
s_num 			//small字符总数量

-----------------------------------------------------------------
<<总text>>
bucket_sorted_txt
bucket_num_txt 5 
txt_num[i]		//text字符各类别数量
bucket_array[j] $ I M P S

----------------------------------------------------------------
M I S S I S S I P P I  $
0 1 2 3 4 5 6 7 8 9 10 11
suffix array:
11 10 7 4 1 0 9 8 6 3 5 2
bwt:
I  P  S S M $ P I S S I I
-----------------------------------------------------------------
b a n a n a i n p a j  a  m  a  s  $
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
suffix array:
15 5 9 11 3 1 13 0 6 10 12 4 2 7 8 14
bwt:
s n p j n b m $ a a a a a i n a
------------------------------------------------------------------
f i r s t | s e c o  n  d  |  t  h  i  r  d  |  f  o  r  t  h  | 
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
small:
0 1 2 3   5     8         12    14 15       18 19 20 21        24
sorted small:
5 12 18 24 8 -1 -1 -1 0 19 -1 14 15 1 -1 -1 20 -1 2 21 -1 3 -1 -1 -1
suffix array:
5 12 18 24 8 11 17 7 0 19 23 14 15 1 10 9 20 16 2 21 6 3 4 22 13
bwt:
t d d h e n r s | | t t h f o c f i i o | r s r | 
| | | | c d d e f f h h i i n o o r r r s s t t t 
------------------------------------------------------------------
r i $ r o $ r i $
0 1 2 3 4 5 6 7 8
suffix array:
2 5 8 1 7 4 0 6 3     8 5 2 temp_arr
bwt:
                 AUX  3 2 1
1 2 3 4 5 6 7 8 9
i o i r r r $ $ $
1 4 7 0 6 3 8 5 2
sf:
$ $ $ i i o r r r     2 5 8 temp_arr_sort
2 5 8 1 7 4 0 6 3
------------------------------------------------------------------
34 	char *txt = malloc(size);

141 int *s_array = malloc(sizeof(int)*s_num);

162 for(i = 0; i < bucket_num_s; i ++){
		bucket_sorted_s[i] = malloc(sizeof(int) * sb_num[i]);
		assert(bucket_sorted_s[i] != NULL);
	}

292 for(i = 0; i < bucket_num_txt; i ++){
		bucket_sorted_txt[i] = malloc(sizeof(int) * txt_num[i]);
		assert(bucket_sorted_txt[i] != NULL);
	}


324	for(i = 0; i < bucket_num_s; i ++){
		free(bucket_sorted_s[i]);
	}

359 free(s_array);

390 free(txt);
	for(i = 0; i < bucket_num_txt; i ++){
		free(bucket_sorted_txt[i]);
	}