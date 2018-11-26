int filesize(FILE *fp);
int occ(char c,int v,FILE *bwt, char *bwt_name,int occ_name_len, int temp_len,char *temp_folder_path);
void m_search(char *bwt_path, char *temp_folder_path,char *search_term,int *c_table, char max_char,int size,int *result,char *bwt_name,int occ_name_len, int temp_len);
int n_search(int *result,int *c_table,char delimiter,char *bwt_path,char *temp_folder_path,char *search_term,char *bwt_name,int occ_name_len, int temp_len);
char bwt_seek(int pF,FILE *bwt);
void quickSort_int(int* arr,int startPos, int endPos);
