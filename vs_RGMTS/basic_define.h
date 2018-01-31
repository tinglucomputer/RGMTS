#ifndef BASIC_DEFINE_H_INCLUDED
#define BASIC_DEFINE_H_INCLUDED
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
	
#define false 0
#define true 1

typedef struct Keys {
	int *s;
	double ***matrix;
	double ***matrix_EX;
	struct words *words_list;
}Keys;

typedef struct Node{
    double *index;
    double *index_1;
    double *index_2;
    int    id;
    struct Node *left;
    struct Node *right;
}Node;

typedef struct fgmrsii{
  int    id;
  int    list_index;
    int    left;
    int    right;
    double *index;
    double *index_1;
    double *index_2;
}fgmrsii;

typedef struct m_index{
    int id;
    double *index;
    double *index_1;
    double *index_2;
}m_index;

typedef struct words{
    char word[40];
    double idf;
    int id;
}words;

typedef struct words_search_tree{
    struct words_search_tree *child;
    struct words_search_tree *next;
    char value;
    int isend;
    int id;
}words_search_tree;

typedef struct result{
    double value;
    int documents[50];
    int used;
    struct result *next;
    struct result *up;
}result;

typedef struct fquery{
    int id;
    int words;
    double *query;
    double *D_1;
    double *D_2;
    int trun;
    struct fquery *next;
    int wordlists[10];
    int wordlists_used;
}fquery;

typedef struct mstacks{
    int id;
    double value;
}mstacks;

typedef struct merges{
    int values[10][2];
    int used;
    int mark;
    int id;
    int group;
    struct merges *next;
}merges;

typedef struct topval{
    int document_id;
    double value;
}topval;

char sBuffer[4096];
typedef struct Node * pnode;
typedef struct fgmrsii * pfgmrsii;
typedef struct fquery * pfquery;

typedef struct Trapdoor_list {
	struct fquery *RGMTS_trapdoor;
	struct fquery *LIST_trapdoor;
	struct fquery *query_RGMTS;
	struct fquery *query_GMTS;
	struct fquery *query_LIST;
}Trapdoor_list;

typedef struct Index_Group{
    struct m_index *m_index_head;
    struct m_index *RGMTS_documents_lists;
    pfgmrsii *RGMTS_head;
	int **extend_files;
	int **documents_temp;
}Index_Group;

#endif // BASIC_DEFINE_H_INCLUDED
