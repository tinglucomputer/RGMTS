
#include "rgmts.h"
static struct words_search_tree * create_words_search_node()
{
    struct words_search_tree *temp;
    temp = (struct words_search_tree *)Malloc(sizeof(struct words_search_tree));
    temp->child=NULL;
    temp->next =NULL;
    temp->isend=0;
    temp->id =-1;
    return temp;
}

static void
insert_to_tree(struct words_search_tree * search_tree,char *words, int id)
{
    /**
        tree 用于快速查找一个单词的id
    **/
    int lens,i;
    struct words_search_tree *temp =NULL, *next=NULL;
    temp = search_tree;
    next = search_tree->child;
    lens = strlen(words);
    for(i=0; i<lens; i++)
    {
        while(next!=NULL)
        {
            if(next->value == words[i]) break;
            else next=next->next;
        }
        if(next==NULL)
        {
            next = create_words_search_node();
            next ->value = words[i];
            next ->next = temp->child;
            temp->child = next;
        }
        if(i+1==lens)
        {
            next ->id = id;
            next ->isend = 1;
        }
        else
        {
            temp = next;
            next = temp->child;
        }
    }

}

static struct words *
get_client_index(int words_num, int document_num,struct words_search_tree *search_tree)
{
    /**
        从文件中获得client_index
        client_index用于保存单词与单词id的对应关系
        如单词 word -> 1
    **/
    char file_name[50];
    struct words *head;
    int i;
    FILE *f;
    sprintf(file_name,"./index/client_index_%d_%d",words_num,document_num);
    head = (struct words *)Malloc(words_num*sizeof(struct words));
    f=Fopen(file_name,"r");
    for (i = 0; i < words_num; i++)
    {
        fscanf(f,"%s",head[i].word);
        insert_to_tree(search_tree,head[i].word,i);
    }
    return head;
}

static void
get_client_list(struct words *head,int words_num, int document_num)
{
    /**
       从文件中获得 client_list
       client_list用于保存单词id与单词idf之间的关系
       如： id： 1-> 0.12345
       表示 id为1的单词的idf为 0.12345
    **/
    char file_name[50];
    int i;
    FILE *f;
    sprintf(file_name,"./index/client_list_%d_%d",words_num,document_num);
    f=Fopen(file_name,"r");
    for (i = 0; i < words_num; i++)
    {
        fscanf(f,"%lf",&head[i].idf);
    }
}


Keys * Get_Matrix(int matrix_dimension_ex, int matrix_dimension, int total, int *arr_used)
{
	Keys *client_key = (Keys *)Malloc(sizeof(Keys));
	char matrix_file_path_pre[50];
	sprintf(matrix_file_path_pre, "./matrix/matrix_%d_%d", matrix_dimension, total);
	client_key->matrix = get_matrix(matrix_file_path_pre, matrix_dimension, total, arr_used);
	sprintf(matrix_file_path_pre, "./matrix/matrix_%d_%d", matrix_dimension_ex, total);
	client_key->matrix_EX = get_matrix(matrix_file_path_pre, matrix_dimension_ex, total, arr_used);
	return  client_key;
}

Keys * Keys_Init(int document_num, int words_num, int dimension, int extend, int extend_num, int key_len, int d_extend, int *arr_used)
{
	Keys *k_temp;
	if ((k_temp = Get_Matrix(dimension + key_len*extend_num + extend, dimension + key_len*extend_num, words_num / dimension, arr_used)) == NULL)
	{
		Error_Print("Matrix is empty", 1);
	}

	if (k_temp->matrix == NULL || k_temp->matrix_EX == NULL) {
		Error_Print("Matrix is empty 1", 1);
	}
	char key_s_path[50];
	sprintf(key_s_path, "./index/s_%d", words_num);
	if ((k_temp->s = create_s(key_s_path, words_num)) == NULL)
	{
		Error_Print("Key S is empty\n", 2);
	}
	struct words *words_list = NULL;
	struct words_search_tree *search_tree = NULL;
	search_tree = create_words_search_node();
	words_list = get_client_index(words_num, document_num, search_tree);
	get_client_list(words_list, words_num, document_num);

	k_temp->words_list = words_list;

	return k_temp;
}
