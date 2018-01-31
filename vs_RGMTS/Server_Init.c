#include "rgmts.h"
Keys* Server_Init(int document_num, int words_num, int dimension, int extend, int extend_num, int key_len)
{
	Keys *k_temp;
	int d_extend = extend_num*key_len / 2;
	int arr_used[2] = {0, 2};
	srand((unsigned int)time(0));
	k_temp = Keys_Init(document_num, words_num, dimension, extend, extend_num, key_len, d_extend, arr_used);
	return k_temp;
}

Index_Group* Get_Index(Keys *k_temp, int words_num, int document_num, int dimension, int new_b, int extend, int extend_num, int key_len,int d_extend, int kind, int is_show, int topnum, int ma)
{
    Index_Group *index_g = (Index_Group*) Malloc(sizeof(Index_Group));
    if(k_temp==NULL || k_temp->s==NULL || k_temp->matrix==NULL || k_temp->matrix_EX==NULL){
        Error_Print("error: k_temp is empty in Get_Index\n", 1);
    }
    index_g->m_index_head = (struct m_index *) get_EGMRS_index(words_num, document_num, dimension,new_b);
    index_g = get_GMTS_and_RGMTS_index(index_g, words_num, document_num, dimension, new_b, extend, extend_num, key_len, d_extend, k_temp->s, k_temp->matrix, k_temp->matrix_EX, is_show, topnum, ma);
    return index_g;
}



