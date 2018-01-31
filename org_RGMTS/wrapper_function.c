#include "head.h"

void *
Malloc(int size)
{
    void *temp;
    if((temp=malloc(size))==NULL){
        printf("error in: func Malloc\n");
        exit(1);
    }
    return temp;
}

FILE *
Fopen(char *file_name, char *mode)
{
    FILE *f;
    if((f=fopen(file_name,mode))==NULL){
        printf("error in: func Fopen %s\n",file_name);
        exit(1);
    }
    return f;
}

struct Node *
Create_node(double *index, double *index_1, double *index_2, int id, struct Node *left, struct Node *right)
{
    struct Node *temp=NULL;
    temp = (struct Node *) Malloc(sizeof(struct Node));
    temp->index   =index;
    temp->index_1 =index_1;
    temp->index_2 = index_2;
    temp->id = id;
    temp->left = left;
    temp->right = right;
    return temp;
}

void
Init_node(struct Node *node, double *index, double *index_1, double *index_2, int id, struct Node *left, struct Node *right)
{
    if(node==NULL){
        printf("error in: func Init_node \n");
        exit(1);
    }
    if(index!=NULL) node->index   =index;
    if(index_1!=NULL) node->index_1 =index_1;
    if(index_1!=NULL) node->index_2 = index_2;
    node->id = id;
    node->left = left;
    node->right = right;
}

double
calculate(double *D, int n, int s, int r, int dimesion,double ***matrix){
    int i;
    double *ma,sum=0.0;
    ma = matrix[n][s]+r*dimesion;
    for(i=0;i<dimesion;i++) sum+=(D[i]*(*(ma+i)));
    return sum;
}


void
save_node(FILE *f, int dimesion, int id, double *index, double *index_1, double *index_2)
{
    int i,t;
    t=dimesion-1;
    fprintf(f,"id:%d\nindex: ",id);
    for(i=0;i<t;i++) fprintf(f,"%.13lf ",index[i]);
    fprintf(f,"%.13lf\n",index[t]);

    fprintf(f,"index_1: ");
    for(i=0;i<t;i++) fprintf(f,"%.13lf ",index_1[i]);
    fprintf(f,"%.13lf\n",index_1[t]);

    fprintf(f,"index_2: ");
    for(i=0;i<t;i++) fprintf(f,"%.13lf ",index_2[i]);
    fprintf(f,"%.13lf\n",index_2[t]);
}

int max_num=0;
void
count_num_loop_1(struct Node *head)
{
    if(head==NULL) return;
    max_num++;
    count_num_loop_1(head->left);
    count_num_loop_1(head->right);
}
int
count_num(struct Node *head)
{
    max_num=0;
    count_num_loop_1(head);
    printf("node=%d\n",max_num);
    return max_num;
}

int
count_need_node(int num)
{
    int a,la,x;
    a=(int)log2(num);
    la=pow(2,a);
    x=2*(num-la);
    return x;
}

struct timeval start,end;
double usetime;
void
time_start(char *message)
{
    extern int is_show;
    if(is_show) printf("start: %s\n",message);
    gettimeofday(&start,NULL);
}

double
time_end(char *message)
{
    extern int is_show;
    gettimeofday(&end,NULL);
    usetime = (double) (1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec) /1000;
    if(is_show) printf("end: %s use time %f ms\n\n",message,usetime);
    return usetime;
}

int fgmrsii_num=0;
 void
count_num_loop(struct fgmrsii *head, int id)
{
    if(id==-1) return;
    fgmrsii_num++;
    count_num_loop(head,head[id].left);
    count_num_loop(head,head[id].right);
}
int
Fgmrs_num(struct fgmrsii *head)
{
    fgmrsii_num=0;
    count_num_loop(head,0);
    //printf("FGMRS-II node=%d\n",fgmrsii_num);
    return fgmrsii_num;
}
