#ifndef BASIC_DEFINE_H_INCLUDED
#define BASIC_DEFINE_H_INCLUDED
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

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
    char words[40];
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

#endif // BASIC_DEFINE_H_INCLUDED
