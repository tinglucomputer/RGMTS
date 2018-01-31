#include "head.h"
int fgmrsii_num=0;
static void
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
    printf("FGMRS-II node=%d\n",fgmrsii_num);
    return fgmrsii_num;
}

void
Init_fgmrsii_node(struct fgmrsii *node, double *index, double *index_1, double *index_2, int id, int list_index, int left, int right)
{
    if(node==NULL){
        printf("error in: func Init_fgmrsii_node \n");
        exit(1);
    }
    node->id = id;
    node->list_index = list_index;
    node->index   =index;
    node->index_1 =index_1;
    node->index_2 = index_2;
    node->left = left;
    node->right = right;
}


static struct fgmrsii *
create_imp_tree(struct fgmrsii *nodes, int node_num, int dimension,int start)
{
    int i,j,s=0,e,t,left=node_num,temp[node_num];
    for(i=0;i<node_num;i++) {
        temp[i]=start+i;
    }
    start--;
    double *index;
    struct fgmrsii *lnode,*rnode;

    while(left>0){
        if(left==1) return &nodes[0];
        t=(int) pow(2,(int) (log2(left)));
        e = s+t-1;
        for(i=e;i>=s;i-=2){
            lnode=&nodes[temp[i-1]];
            rnode=&nodes[temp[i]];
            index = (double *)Malloc(sizeof(double)*dimension);
            for(j=0;j<dimension;j++) index[j] = MAX(lnode->index[j], rnode->index[j]);
            Init_fgmrsii_node(&nodes[start],index,NULL,NULL,-1,start,lnode->list_index,rnode->list_index);
            temp[e--] = nodes[start--].list_index;
        }
        s=e+1;
        left = node_num-s;
    }
    printf("error in fun -> create_imp_tree\n");
    exit(6);
}

static pfgmrsii *
building_tree_imp_index_from_m_index(struct m_index *m_index_head,int group_num, int document_num, int dimension, int tree_imp_node_num)
{
    int i,j,d,k,s,start;
    struct fgmrsii *temp;
    double *index;
    pfgmrsii *head = (pfgmrsii *)malloc(sizeof(pfgmrsii)*group_num);
    start = tree_imp_node_num-document_num;
    for(i=0;i<group_num;i++){
        temp=(struct fgmrsii *)Malloc(sizeof(struct fgmrsii)*tree_imp_node_num);
        for(d=0,s=i*dimension;d<document_num;d++){
            index = (double *)Malloc(sizeof(double)*dimension);
            for(k=s,j=0;j<dimension;k++,j++){
                index[j] = m_index_head[k].index[d];
            }
            Init_fgmrsii_node(&temp[start+d],index,NULL,NULL,d,start+d,-1,-1);
        }
        head[i] = create_imp_tree(temp,document_num,dimension,start);
    }
    return head;
}

void
encrypt_FGMRS_II_each_node(struct fgmrsii *root, int id, int g, int dimension,int *s, double ***m_matrix)
{
    if(id==-1) return;
    int i,j;
    double D_1[dimension],D_2[dimension],t1,*index_1,*index_2;
    struct fgmrsii *head;

    head = &root[id];
    index_1 = (double *)Malloc(dimension*sizeof(double));
    index_2 = (double *)Malloc(dimension*sizeof(double));
    for(i=0,j=g*dimension;i<dimension;i++,j++){
        t1=head->index[i];
        if(s[j]==0){
            D_1[i]=D_2[i]=t1;
        }else{
            D_1[i] = 2.5-(double)(rand()%50)/10.0;
            D_2[i] = t1 - D_1[i];
        }
    }
    for(i=0;i<dimension;i++){
        index_1[i] = calculate(D_1, 0, g,i, dimension, m_matrix);
        index_2[i] = calculate(D_2, 2, g,i, dimension, m_matrix);
    }
    head->index_1=index_1;
    head->index_2=index_2;
    if(head->left!=-1) encrypt_FGMRS_II_each_node(root,head->left,g,dimension,s,m_matrix);
    if(head->right!=-1) encrypt_FGMRS_II_each_node(root,head->right,g,dimension,s,m_matrix);
}

void
encrypt_FGMRS_II(pfgmrsii *head, int dimension, int total, int *s, double ***m_matrix)
{
    int i;
    for(i=0;i<total;i++) encrypt_FGMRS_II_each_node(head[i],0,i,dimension,s,m_matrix);
}

void
save_GGMRS_II_node(FILE *f, int dimesion, int id, int list_index, int left, int right, double *index, double *index_1, double *index_2)
{
    int i,t;
    t=dimesion-1;
    fprintf(f,"%d %d %d %d\nindex: ",id,list_index,left,right);
    for(i=0;i<t;i++) fprintf(f,"%.13lf ",index[i]);
    fprintf(f,"%.13lf\n",index[t]);

    fprintf(f,"index_1: ");
    for(i=0;i<t;i++) fprintf(f,"%.13lf ",index_1[i]);
    fprintf(f,"%.13lf\n",index_1[t]);

    fprintf(f,"index_2: ");
    for(i=0;i<t;i++) fprintf(f,"%.13lf ",index_2[i]);
    fprintf(f,"%.13lf\n",index_2[t]);
}

void
sava_FGMRS_II_index_to_file(char *file_name, pfgmrsii *head, int total, int document_num, int dimension, int tree_imp_node_num)
{
    if(head==NULL) return ;
    FILE *f=Fopen(file_name,"w");
    pfgmrsii queue[tree_imp_node_num],temp[tree_imp_node_num],*t1,*t2,*t3;
    int qnum=0,tnum=0,i,g;

    fprintf(f,"all:%d\n",tree_imp_node_num);
    for(g=0;g<total;g++){
        qnum=0;
        tnum=0;
        t1=queue;
        t2=temp;
        t1[qnum++]=head[g];
        while(qnum>0){
            for(i=0;i<qnum;i++){
                save_GGMRS_II_node(f,dimension,t1[i]->id, t1[i]->list_index,t1[i]->left,t1[i]->right,t1[i]->index, t1[i]->index_1, t1[i]->index_2);
                if(t1[i]->left!=-1) t2[tnum++]=&head[g][t1[i]->left];
                if(t1[i]->right!=-1) t2[tnum++]=&head[g][t1[i]->right];
            }
            t3=t1;
            t1=t2;
            t2=t3;
            qnum=tnum;
            tnum=0;
        }
    }
    fclose(f);
}
pfgmrsii *
get_FGMRS_II_index_from_file(char *index_file_name, int dimension, int total, int *all)
{
    FILE *f=Fopen(index_file_name,"r");
    int id,i,j=0,_all,g,list_index,left,right;
    double *index,*index_1=NULL,*index_2=NULL;
    struct fgmrsii *temp;

    if(fscanf(f,"all:%d",&_all)<1){
        printf("error in: func get_FGMRS_II_index_from_file -> get all\n");
        exit(1);
    }
    fscanf(f,"\n");
    pfgmrsii *head;
    head = (pfgmrsii *)malloc(sizeof(pfgmrsii)*total);
    for(g=0;g<total;g++){
        temp = (struct fgmrsii *)Malloc(_all*sizeof(struct fgmrsii));
        j=0;
        while(j<_all){
            if(fscanf(f, "%d %d %d %d",&id,&list_index,&left,&right)>0){
                fscanf(f,"\n");
                fscanf(f, "%s", sBuffer);
                index = (double *)Malloc(dimension*sizeof(double));
                for(i=0;i<dimension;i++){
                    fscanf(f, "%lf", &(index[i]));
                }
                fscanf(f,"\n");
                fscanf(f, "%s",sBuffer);
                index_1 = (double *)Malloc(sizeof(double)*dimension);
                for(i=0;i<dimension;i++){
                    fscanf(f, "%lf", &(index_1[i]));
                }
                fscanf(f,"\n");
                fscanf(f, "%s",sBuffer);
                index_2 = (double *)Malloc(sizeof(double)*dimension);
                for(i=0;i<dimension;i++){
                    fscanf(f, "%lf", &(index_2[i]));
                }
                fscanf(f,"\n");
                Init_fgmrsii_node(&temp[list_index],index,index_1,index_2,id,list_index,left,right);
            }else{
                printf("error in: func get_FGMRS_II_index_from_file -> get id j=%d\n ",j);
                exit(4);
            }
            j++;
        }
        head[g]=&temp[0];
    }
    (*all) = _all;
    return head;
}

void
extend_each_node(struct fgmrsii *root, int id, int g, int dimension, int add_num, double limt, int lim)
{
    if(id==-1) return;
    int i;
    double *eindex;
    struct fgmrsii *head;

    head = &root[id];
    eindex = (double *)Malloc((dimension+add_num+2)*sizeof(double));
    for(i=0;i<dimension;i++){
        eindex[i] =head->index[i];
    }

    for(i=0;i<add_num;i++){
        eindex[dimension+i] =  (limt-(double)(rand()%lim))/10000;
    }
    eindex[dimension+add_num] = 1.0;
    eindex[dimension+add_num+1] = 1.0;

    head->eindex =eindex;
    if(head->left!=-1) extend_each_node(root,head->left,g,dimension,add_num,limt,lim);
    if(head->right!=-1) extend_each_node(root,head->right,g,dimension,add_num,limt,lim);
}

void
extend_FGMRS_II(pfgmrsii *head, int dimension, int total, int extend)
{
    int i;
    double limt;
    int lim;

    limt  = sqrt(3.0)*10000;
    lim = (int)(limt*2);

    for(i=0;i<total;i++) {
        extend_each_node(head[i],0,i,dimension,extend,limt,lim);
    }
}

void
improved_each_node(struct fgmrsii *root, int id, int g, int dimension)
{
    if(id==-1) return;
    int i;
    double *pindex;
    struct fgmrsii *head,*left,*right;

    head = &root[id];
    pindex = (double *)Malloc((dimension)*sizeof(double));

    if(head->left!=-1) {
        improved_each_node(root,head->left,g,dimension);
        improved_each_node(root,head->right,g,dimension);
        left = &root[head->left];
        right = &root[head->right];
        for(i=0;i<dimension;i++){
            pindex[i] =left->pindex[i]+right->pindex[i];
            /** (1)*/
                if(left->pindex[i]!=0 && right->pindex[i]!=0) pindex[i]=MIN(left->pindex[i],right->pindex[i]);
            /**/
            /** (2)  */
               // if(left->pindex[i]!=0 && right->pindex[i]!=0) pindex[i]/=2.0;
            /**/
            /**(3)*/
                //pindex[i]=MAX(left->pindex[i], right->pindex[i]);
            /**/
            /**(4)*/
                //pindex[i]=MAX(left->pindex[i], right->pindex[i]);
            /**/

        }
    }else{
        for(i=0;i<dimension;i++) pindex[i] =head->index[i];
    }
    head->pindex =pindex;
}

void
improved_FGMRS_II(pfgmrsii *head, int dimension, int total)
{
    int i;
    for(i=0;i<total;i++) {
        improved_each_node(head[i],0,i,dimension);
    }
}

void
improved_encrypt_FGMRS_II_each_node(struct fgmrsii *root, int id, int g, int dimension,int *s, double ***m_matrix)
{
    if(id==-1) return;
    int i,j;
    double D_1[dimension],D_2[dimension],t1,*index_1,*index_2;
    struct fgmrsii *head;

    head = &root[id];
    index_1 = (double *)Malloc(dimension*sizeof(double));
    index_2 = (double *)Malloc(dimension*sizeof(double));
    for(i=0,j=g*dimension;i<dimension;i++,j++){
        t1=head->pindex[i];
        if(s[j]==0){
            D_1[i]=D_2[i]=t1;
        }else{
            D_1[i] = 2.5-(double)(rand()%50)/10.0;
            D_2[i] = t1 - D_1[i];
        }
    }
    for(i=0;i<dimension;i++){
        index_1[i] = calculate(D_1, 0, g,i, dimension, m_matrix);
        index_2[i] = calculate(D_2, 2, g,i, dimension, m_matrix);
    }
    head->pindex_1=index_1;
    head->pindex_2=index_2;
    if(head->left!=-1) improved_encrypt_FGMRS_II_each_node(root,head->left,g,dimension,s,m_matrix);
    if(head->right!=-1) improved_encrypt_FGMRS_II_each_node(root,head->right,g,dimension,s,m_matrix);
}

void
improved_encrypt_FGMRS_II(pfgmrsii *head, int dimension, int total, int *s, double ***m_matrix)
{
    int i;
    for(i=0;i<total;i++) improved_encrypt_FGMRS_II_each_node(head[i],0,i,dimension,s,m_matrix);
}

pfgmrsii *
get_FGMRS_II_index(int words_num,int document_num,int dimension,int new_b, int extend, int *s,double ***m_matrix){
    char file_name[50];
    int t1=words_num/dimension;
    extern int tree_imp_node_num;
    extern struct m_index *m_index_head;

    pfgmrsii *head=NULL;
    sprintf(file_name, "./index/FGMRS_II_index_w_%d_f_%d_d_%d",words_num,document_num,dimension);
    if(new_b==1 || access(file_name,0)!=0){
        tree_imp_node_num = count_need_node(document_num);
        printf("building FGMRS-II index...\n");
        time_start();
        head = building_tree_imp_index_from_m_index(m_index_head,t1,document_num, dimension,tree_imp_node_num);
        printf("encrypt FGMRS-II index...\n");
        encrypt_FGMRS_II(head,dimension,t1,s,m_matrix);
        printf("save FGMRS-II index...\n");
        sava_FGMRS_II_index_to_file(file_name,head,t1,document_num,dimension,tree_imp_node_num);
        time_end("create FGMRS-II index");
    }else{
        printf("get FGMRS-II from file...\n");
        head = get_FGMRS_II_index_from_file(file_name, dimension, t1,&tree_imp_node_num);
        printf("finish.\n\n");
    }
    Fgmrs_num(head[0]);
    extend_FGMRS_II(head,dimension,t1,extend);
    improved_FGMRS_II(head,dimension,t1);
    improved_encrypt_FGMRS_II(head,dimension,t1,s,m_matrix);
    return head;
}
