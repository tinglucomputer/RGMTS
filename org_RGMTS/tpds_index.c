#include "head.h"

extern int encrypt_num;
double *D_1,*D_2;

struct Node *
get_tpds_index_from_file(char *index_file_name, int dimension, int mark, int *all)
{
    FILE *f=Fopen(index_file_name,"r");
    int id,i,j=0,s=0,n=1,_all;
    struct Node *temp;

    if(fscanf(f,"all:%d",&_all)<1){
        printf("error in: func get_tpds_index_from_file -> get all\n");
        exit(1);
    }
    fscanf(f,"\n");
    temp = (struct Node *)Malloc(_all*sizeof(struct Node));
    for(j=0;j<_all;j++) {
        temp[j].index = (double *)Malloc(sizeof(double)*dimension);
        temp[j].index_1 = (double *)Malloc(sizeof(double)*dimension);
        temp[j].index_2 = (double *)Malloc(sizeof(double)*dimension);
    }
    j=0;
    while(j<_all){
        if(fscanf(f, "id:%d",&id)>0){
            fscanf(f,"\n");
            fscanf(f, "%s", sBuffer);
            for(i=0;i<dimension;i++){
                fscanf(f, "%lf", &(temp[j].index[i]));
            }
            fscanf(f,"\n");
            if(mark){
                fscanf(f, "%s",sBuffer);
                for(i=0;i<dimension;i++){
                    fscanf(f, "%lf", &(temp[j].index_1[i]));
                }
                fscanf(f,"\n");
                fscanf(f, "%s",sBuffer);
                for(i=0;i<dimension;i++){
                    fscanf(f, "%lf", &(temp[j].index_2[i]));
                }
                fscanf(f,"\n");
            }
            Init_node(&temp[j],NULL,NULL,NULL,id,NULL,NULL);
        }else{
            printf("error in: func get_tpds_index_from_file -> get id j=%d\n ",j);
            exit(4);
        }
        j++;
    }
    while(s<_all){
        if(temp[s].id>0){
            s++;
            continue;
        }
        if(s==n) n++;
        if(n<_all) temp[s].left = &temp[n];
        else break;
        n++;
        if(n<_all) temp[s].right = &temp[n];
        else break;
        n++;
        s++;
    }
    (*all) = _all;
    return &temp[0];
}

void
encrypt_tpds(struct Node *head, int words_num, int *s, double ***matrix,int first_matrix, int second_matrix)
{
    int i;
    double *value_1,*value_2;

    value_1 = head->index_1;
    value_2 = head->index_2;
    for(i=0;i<words_num;i++){
        if(s[i]==first_matrix){
            D_1[i]=D_2[i]=head->index[i];

        }else{
            D_1[i] = 2.5-(double)(rand()%50)/10.0;
            D_2[i] = head->index[i] - D_1[i];
        }
    }

    for(i = 0; i < words_num; i++){
        value_1[i]=calculate(D_1, first_matrix, 0, i, words_num,matrix);
        value_2[i]=calculate(D_2, second_matrix,0, i, words_num,matrix);
    }
    head->index_1=value_1;
    head->index_2=value_2;
}

void
encrypt_tpds_index(struct Node *head, int words_num, int *s, double ***matrix)
{
    if(head==NULL) return;
    encrypt_num++;
    if(encrypt_num%100==0) printf("-- %d\n",encrypt_num);
    encrypt_tpds(head,words_num,s,matrix,0,2);
    encrypt_tpds_index(head->left, words_num, s, matrix);
    encrypt_tpds_index(head->right, words_num, s, matrix);
}

void
sava_tpds_index_to_file(char *file_name, struct Node *head,int node_num,int words_num, int all)
{
    if(head==NULL) return ;
    FILE *f=Fopen(file_name,"w");
    pnode queue[node_num],temp[node_num],*t1,*t2,*t3;
    int qnum=0,tnum=0,i;

    fprintf(f,"all:%d\n",all);

    t1=queue;
    t2=temp;
    t1[qnum++]=head;
    while(qnum>0){
        for(i=0;i<qnum;i++){
            save_node(f,words_num,t1[i]->id, t1[i]->index, t1[i]->index_1, t1[i]->index_2);
            if(t1[i]->left!=NULL) t2[tnum++]=t1[i]->left;
            if(t1[i]->right!=NULL) t2[tnum++]=t1[i]->right;
        }
        t3=t1;
        t1=t2;
        t2=t3;
        qnum=tnum;
        tnum=0;
    }
    fclose(f);
}
struct Node *
tpds_index_building(int words_num, int document_num, int new_b, int *s)
{
    struct Node *head;
    char en_file_name[50],un_file_name[50];
    extern double ***matrix;
    extern int tpds_node_num;

    sprintf(en_file_name,"./matrix/matrix_%d_%d",words_num,1);
    //printf("get tpds matrix from file:\n");
    matrix = get_matrix(en_file_name,words_num,1);
    //printf("get end\n");

    sprintf(un_file_name,"./index/index_tpds2015_%d_%d.pickle_ue",document_num,words_num);
    sprintf(en_file_name,"./index/index_tpds2015_%d_%d.pickle",document_num,words_num);

    if(new_b==1 || access(en_file_name,0)!=0){
        D_1 = (double *)Malloc(sizeof(double)*words_num);
        D_2 = (double *)Malloc(sizeof(double)*words_num);

        time_start("building tpds index");
        //printf("get unencrypt tpds index from file:\n");
        head = get_tpds_index_from_file(un_file_name, words_num,0,&tpds_node_num);
        //printf("encrypt tpds2015 index ...\n");
        encrypt_tpds_index(head, words_num, s, matrix);
        //printf("sava encrypted index to %s\n",en_file_name);
        sava_tpds_index_to_file(en_file_name, head,tpds_node_num,words_num,tpds_node_num);
        time_end("building tpds index");
    }else{
        printf("get encrypt tpds index from file:\n");
        head = get_tpds_index_from_file(en_file_name, words_num,1,&tpds_node_num);
        printf("get end\n\n");
    }
    return head;
}

