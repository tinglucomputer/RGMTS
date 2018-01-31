#include "head.h"

void
get_m_index_from_file(struct m_index *temp,char *file_name, int words_num, int document_num)
{
    int id,i,j=0;
    double *index;
    FILE *f;

    f=Fopen(file_name,"r");

    while(j<words_num){
        if(fscanf(f, "id:%d",&id)>0){
            temp[j].id =id;
            fscanf(f,"\n");
            fscanf(f, "%s",sBuffer);
            index = (double *)Malloc(sizeof(double)*document_num);
            for(i=0;i<document_num;i++){
                fscanf(f, "%lf", &(index[i]));
            }
            temp[j].index = index;
            fscanf(f,"\n");
            j++;
        }else{
            printf("error in: func get_m_index_from_file -> get string\n ");
            exit(4);
        }
    }
    fclose(f);
}

struct m_index *
get_EGMRS_index(int words_num, int document_num, int dimension, int new_b)
{
    char encrypted_file_name[50];
    struct m_index *m_head=NULL;
    m_head = (struct m_index *)Malloc(words_num*sizeof(struct m_index));

    sprintf(encrypted_file_name, "./index/index_w_%d_f_%d_d_%d",words_num,document_num,dimension);

    //printf("get encrypt my index from file:\n");
    get_m_index_from_file(m_head,encrypted_file_name,words_num,document_num);
    //printf("finish\n\n");

    return m_head;
}

