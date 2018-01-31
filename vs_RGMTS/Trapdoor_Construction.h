#ifndef  TRAPDOOR_CONSTRUCTION
#define TRAPDOOR_CONSTRUCTION

Trapdoor_list *
TrapDoor_Construction(Keys* my_keys, Trapdoor_list *trapdoor_ret, int *lists, int numbers, int words_num, int document_num, int dimension, int topnum, int extend, int extend_num, int key_len, int d_extend);
Trapdoor_list * Trapdoor_Malloc(int words_num, int dimension);
void Free_Trapdoor(Trapdoor_list *trapdoor_ret);

#endif // ! TRAPDOOR_CONSTRUCTION
