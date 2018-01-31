#include "head.h"

int **extend_files;
int **documents_temp;

static void
Init_fgmrsii_node(struct fgmrsii *node, double *index, double *index_1, double *index_2, int id, int list_index, int left, int right)
{
    if(node==NULL){
        printf("error in: func Init_fgmrsii_node \n");
        exit(1);
    }
    node->id = id;
    node->list_index = list_index;
    if(index!=NULL) node->index   =index;
    if(index_1!=NULL) node->index_1 =index_1;
    if(index_2!=NULL) node->index_2 = index_2;
    node->left = left;
    node->right = right;
}

/**
 *coping each document.
 *assigning a document identification for each copy
**/
void
disturb_documents(int document_num,int extend_num,int key_len)
{
    int i,j,t,k,s,e,all_g,all_d;
    int is_used[extend_num*key_len];
    int groups[extend_num];

    all_d=document_num*extend_num;
    t=document_num/key_len;
    all_g=key_len*extend_num;

    extend_files=(int **)Malloc(document_num*sizeof(int *));
    for(i=0;i<document_num;i++) extend_files[i]=(int *)calloc(extend_num,sizeof(int));
    documents_temp = (int **)Malloc(all_d*sizeof(int *));
    for(i=0;i<all_d;i++) documents_temp[i]=(int *)calloc(3,sizeof(int));

    memset(is_used,0,sizeof(is_used));

    srand(time(0));

    for(j=0;j<document_num;j+=t){
        for(i=0;i<extend_num;i++){
            s=rand()%all_g;
            while(is_used[s]==1){
                s=(s+1)%all_g;
            }
            is_used[s]=1;
            groups[i]=s*t;
        }
        e=j+t;
        for(k=j;k<e;k++){
            for(i=0;i<extend_num;i++){
                s=rand()%t;
                while(documents_temp[groups[i]+s][0]>0){
                    s=(s+1)%t;
                }
                documents_temp[groups[i]+s][0]=k+1; //record the original document identification of a copy
                documents_temp[groups[i]+s][1]=i;
                documents_temp[groups[i]+s][2]=j/t; // j/t is the group of this copy belong to.
                extend_files[k][i]=groups[i]+s; // the document identification of this copy.
            }
        }
    }
}

static int
insert_to_sorted_result(topval *sorted_result, int top_use, int top_max, double *top_min,double value,int document_id)
{
    int i;
    if(top_use==0){
        sorted_result[top_use].value=value;
        sorted_result[top_use++].document_id=document_id;
        (*top_min)=value;
    }else{
        if(top_use<top_max){
            i=top_use;
            top_use++;
        }else{
            i=top_use-1;
        }
        while(i>0 && sorted_result[i-1].value<value){
            sorted_result[i].value = sorted_result[i-1].value;
            sorted_result[i].document_id = sorted_result[i-1].document_id;
            i--;
        }
        sorted_result[i].value=value;
        sorted_result[i].document_id=document_id;
    }
    (*top_min)=sorted_result[top_use-1].value;
    return top_use;
}


/**
* saving the top-ck documents of each word.
* removing the documents that out of top-ck
**/
static m_index *
extend_original_index(m_index *root, int words_num, int documents_num)
{
    extern int topnum,ma;
    m_index *extend_result;
    int i,j,top_max=topnum*ma,top_use=0,document_id;
    topval sorted_result[1500];
    double top_min,value;

    extend_result=(m_index *)Malloc(sizeof(m_index)*documents_num);

    for(i=0;i<documents_num;i++){
        extend_result[i].index = (double *)calloc(words_num,sizeof(double));
    }
    for(i=0;i<words_num;i++){
        top_min=0;
        top_use=0;
        for(j=0;j<documents_num;j++){
            value=root[i].index[j];
            if(value>0 && (value>top_min || top_use<top_max)){
                top_use=insert_to_sorted_result(sorted_result,top_use,top_max,&top_min,value,j);
            }
        }
        for(j=0;j<top_use;j++){
            document_id = sorted_result[j].document_id;
            extend_result[document_id].index[i] = sorted_result[j].value;
        }
    }
    return extend_result;
}

void
free_index(m_index *extend_root, int d)
{
    int i;
    for(i=0;i<d;i++) free(extend_root[i].index);
    free(extend_root);
}


static struct fgmrsii *
build_tree_for_RGMTS(struct fgmrsii *nodes, int all, int dimension,int node_num,int e_dimension,int extend_num)
{
    int i,j,k,s=0,e,mark=0,mark1,t,s1,s2;
    int x=count_need_node(all);
    struct fgmrsii *lnode,*rnode;
    e=node_num-1;
    j=node_num-all-1;
    if(x==0) s=node_num-all;
    else s=node_num-x;
    while(s<e){
        for(i=e;i>s;i-=2,j--){
            lnode=&nodes[i-1];
            rnode=&nodes[i];
            mark=0;
            mark1=0;

            for(k=0;k<dimension;k++)
            {
                nodes[j].index[k] = MAX(lnode->index[k], rnode->index[k]);
                if(nodes[j].index[k]>0.00001){
                    nodes[j].index[k] += nodes[j].index[k]*((double)(rand()%100)/100.0);
                }
            }

            for(k=dimension;k<e_dimension;k++){
                if((lnode->index[k]>=1 && rnode->index[k]<=1) || (lnode->index[k]<=1 && rnode->index[k]>=1)){
                        mark=1;
                        break;
                }
                if(lnode->index[k]>=1 && rnode->index[k]>=1){
                        mark1++;
                        t=k-dimension;

                }
            }
            if(mark==0 && mark1==1){
                s1=dimension+t/extend_num*extend_num;
                s2=s1+extend_num;
                for(k=s1;k<s2;k++){
                    if(lnode->index[k]>=1){
                        nodes[j].index[k]=8.8+(double)(rand()%10);
                    }else{
                        nodes[j].index[k]=((double)(rand()%10))/1000000.0;
                    }
                }
            }
            Init_fgmrsii_node(&nodes[j],NULL,NULL,NULL,-1,j,lnode->list_index,rnode->list_index);
        }
        e=s-1;
        s=j+1;
    }
    return &nodes[0];
}
int test_nums =0 ;
static struct fgmrsii *
build_tree_for_GMTS(struct fgmrsii *nodes, int all, int dimension,int node_num,int e_dimension)
{   //build_tree_for_GMTS(temp,all,dimension,node_num,e_dimension)
    int i,j,k,s=0,e;
    int x=count_need_node(all);
    struct fgmrsii *lnode,*rnode;
    e=node_num-1;
    j=node_num-all-1;
    if(x==0) s=node_num-all;
    else s=node_num-x;
    while(s<e){
        for(i=e;i>s;i-=2,j--){
            lnode=&nodes[i-1];
            rnode=&nodes[i];
            //for(k=0;k<dimension;k++) nodes[j].index[k] = MAX(lnode->index[k], rnode->index[k]); -
            /**+++++++++++++++*/
            for(k=0;k<dimension;k++){
                nodes[j].index[k] = MAX(lnode->index[k], rnode->index[k]);
                /*if(test_nums<500){
                    printf("%.5lf ", nodes[j].index[k]);
                }*/
                if(nodes[j].index[k]>0.00001){
                    nodes[j].index[k] += nodes[j].index[k]*((double)(rand()%100)/100.0);
                }
               /* if(test_nums<500){
                    printf("%.5lf\n", nodes[j].index[k]);
                }
                test_nums++;*/

            }
            /***********end*******/
            Init_fgmrsii_node(&nodes[j],NULL,NULL,NULL,-1,j,lnode->list_index,rnode->list_index);
        }
        e=s-1;
        s=j+1;
    }
    return &nodes[0];
}

m_index *
bulid_FILES_LIST(m_index *root, int words_num,int document_num,int dimension,int d_extend, int extend, unsigned int type)
{
    /**
    type = 1  for GMTS
    type = 2  for RGMTS
    **/
    m_index *lists;
    int k,s,e,i,j,t,d_len=d_extend*2;
    double temp[d_len];
    double limt;
    limt  = sqrt(6.0/((double)extend));

    lists=(m_index *)Malloc(document_num*sizeof(m_index));
    t=(words_num/dimension + 1)*(dimension + d_len);//note that: we only consider words_num is the multiple of the dimension

    for(i=0; i < document_num; i++){
        lists[i].index=(double *)calloc(t,sizeof(double));
    }

    for(i=0; i<document_num; i++){
        lists[i].index_1=(double *)calloc(t,sizeof(double));
        lists[i].index_2=(double *)calloc(t,sizeof(double));
    }

    for(i=0;i<document_num;i++){
        memset(temp,0,sizeof(temp));
        for(j=0;j<d_extend;j++){
            t=rand()%d_len;
            while(temp[t]!=0){
                t=(t+1)%d_len;
            }
            temp[t]=1;
        }
        s=0;
        for(j=0;j<words_num;j+=dimension){
            e=j+dimension;
            if(type & 2){
                for(k=j;k<e;k++,s++){
                    lists[i].index[s]=root[k].index[documents_temp[i][0]-1];
                }
            }else{
                for(k=j;k<e;k++,s++){
                    lists[i].index[s]=root[k].index[i];
                }
            }

            for(k=0;k<d_len;k++,s++){
                lists[i].index[s]=temp[k];
            }
        }
        /*************************structure**********************/
        /** random numbers ...0 0 ... 0 0 other random numbers **/
        /********************************************************/
        /** adding random numbers **/
        for(j=0;j<extend;j++,s++){
            lists[i].index[s] =  (limt-2*limt*((double)rand())/((double) RAND_MAX));
        }

        /** adding other random numbers  **/
        for(j=0,s+=(dimension-extend);j<d_len;j++,s++){
            lists[i].index[s]=temp[j];
        }
    }
    return lists;
}

void encrypt_FILES_LIST(m_index *file_lists, int document_num, int e_dimension, int words_num, int e_words_num, int group_num, int *s, double ***matrix){
    int d,j,k,i,group_id;
    double D_1[e_dimension],D_2[e_dimension],t1;
    double *index;

    for(d=0;d<document_num;d++){
        index = file_lists[d].index;
        for(j=0;j<e_words_num;j+=e_dimension){
            for(k=j,i=0;i<e_dimension;i++,k++){
                t1=index[k];
                if(s[k%words_num]==0){
                    D_1[i]=D_2[i]=t1;
                }else{
                    D_1[i] = 2.5-(double)(rand()%50)/10.0;
                    D_2[i] = t1 - D_1[i];
                }
            }
            group_id=(j/e_dimension) % group_num;
            for(k=0;k<e_dimension;k++){
                file_lists[d].index_1[j+k] = calculate(D_1, 0, group_id, k, e_dimension, matrix);
                file_lists[d].index_2[j+k] = calculate(D_2, 2, group_id, k, e_dimension, matrix);
            }
        }
    }
}

void
encrypt_each_node_in_index(struct fgmrsii *root, int group_id, int id, int words_num, int e_dimension,int *s, double ***matrix)
{
    int j;
    double D_1[e_dimension],D_2[e_dimension],t1;
    struct fgmrsii *head;
    head = &root[id];
    for(j=0;j<e_dimension;j++){
        t1=head->index[j];
        if(s[(group_id*e_dimension+j)%words_num]==0){
            D_1[j]=D_2[j]=t1;
        }else{
            D_1[j] = 2.5-(double)(rand()%50)/10.0;
            D_2[j] = t1 - D_1[j];
        }
    }
    for(j=0;j<e_dimension;j++){
        head->index_1[j] = calculate(D_1, 0, group_id,j, e_dimension, matrix);
        head->index_2[j] = calculate(D_2, 2, group_id,j, e_dimension, matrix);
    }

    free(head->index);

    if(head->left!=-1)  encrypt_each_node_in_index(root,group_id,head->left,words_num,e_dimension,s,matrix);
    if(head->right!=-1) encrypt_each_node_in_index(root,group_id,head->right,words_num,e_dimension,s,matrix);
}

void
encrypt_GMTS_and_RGMTS_index(pfgmrsii *root, int group_num, int words_num,int e_dimension ,int *s, double ***matrix)
{
    int i;
    for(i=0;i<group_num;i++){
        encrypt_each_node_in_index(root[i],i,0, words_num,e_dimension,s,matrix);
    }
}

pfgmrsii *
bulid_GMTS(m_index *root, int group_num, int words_num, int document_num, int dimension, int extend)
{
    pfgmrsii *head;
    fgmrsii *temp;
    double sum;
    int i, j, s, e, k, all, node_num, start, all_node=0;
    int d_temp[document_num];
    int e_dimension = dimension + extend;
    double limt;

    limt  = sqrt(6.0/((double)extend));
    head = (pfgmrsii *)Malloc(sizeof(pfgmrsii)*group_num);

    for(i=0;i<words_num;i+=dimension){
        all=0;
        s=i;
        e=s+dimension;
        memset(d_temp,0,sizeof(d_temp));
        for(j=0;j<document_num;j++){
            sum=0.0;
            for(k=s;k<e;k++){
                 sum+=root[j].index[k];
            }
            if(sum>0.000001){
                all++;
                d_temp[j]=1;
            }
        }
        //printf("all=%d\n",all);
        node_num=all*2-1;
        all_node+=node_num;
        temp=(struct fgmrsii *)Malloc(sizeof(struct fgmrsii)*node_num);
        for(j=0;j<node_num;j++) {
            temp[j].index =   (double *)calloc(e_dimension,sizeof(double));
        }
        for(j=0;j<node_num;j++) {
            temp[j].index_1 = (double *)calloc(e_dimension,sizeof(double));
            temp[j].index_2 = (double *)calloc(e_dimension,sizeof(double));
        }
        start = node_num-all;
        for(j=0;j<document_num;j++){
            if(d_temp[j]!=1) continue;
            for(k=s;k<e;k++) temp[start].index[k-s]=root[j].index[k];
            Init_fgmrsii_node(&temp[start],NULL,NULL,NULL,j,start,-1,-1);
            start++;
        }
        head[i/dimension] = build_tree_for_GMTS(temp,all,dimension,node_num,e_dimension);
        for(j=0;j<node_num;j++){
            for(k=0;k<extend;k++){
                temp[j].index[dimension+k] =  (limt-2*limt*((double)rand())/((double) RAND_MAX));
            }
        }
    }
    //printf("DF_all_node=%d\n\n",all_node);
    return head;
}

pfgmrsii *
bulid_RGMTS(m_index *root, int words_num,int document_num,int dimension,int extend_num,int key_len, int extend)
{
    pfgmrsii *head;
    fgmrsii *temp;
    double sum;
    int i,j,s,e,k,t,all,node_num,start,all_node=0,all_d=document_num*extend_num;
    int d_temp[all_d];
    int e_dimension=dimension+key_len*extend_num;
    int ex_dimension = e_dimension+extend;
    int group_num;
    double limt;

    limt  = sqrt(6.0/((double)extend));
    group_num=words_num/dimension;

    head = (pfgmrsii *)Malloc(sizeof(pfgmrsii)*group_num);

    for(i=0;i<words_num;i+=dimension){
        all=0;
        s=i;
        e=s+dimension;
        memset(d_temp,0,sizeof(d_temp));
        for(j=0;j<document_num;j++){
            sum=0.0;
            for(k=s;k<e;k++){
                 sum+=root[j].index[k];
            }
            if(sum>0.000001){
                all+=extend_num;
                for(k=0;k<extend_num;k++)  d_temp[extend_files[j][k]]=1;
            }
        }
        node_num=all*2-1;
        all_node+=node_num;
        temp=(struct fgmrsii *)Malloc(sizeof(struct fgmrsii)*node_num);
        for(j=0;j<node_num;j++) {
            temp[j].index =   (double *)calloc(ex_dimension,sizeof(double));
        }
        for(j=0;j<node_num;j++) {
            temp[j].index_1 = (double *)calloc(ex_dimension,sizeof(double));
            temp[j].index_2 = (double *)calloc(ex_dimension,sizeof(double));
        }
        start = node_num-all;
        for(j=0;j<all_d;j++){
            if(d_temp[j]!=1) continue;
            for(k=s;k<e;k++) temp[start].index[k-s]=root[documents_temp[j][0]-1].index[k];
            /** assigning switch for each document.**/
            t=dimension+documents_temp[j][2]*extend_num;
            for(k=0;k<extend_num;k++){
                if(k!=documents_temp[j][1]){
                    temp[start].index[t+k]=0;
                }else{
                    temp[start].index[t+k]=5.8+(double)(rand()%5);
                }
            }
            Init_fgmrsii_node(&temp[start],NULL,NULL,NULL,j,start,-1,-1);
            start++;
        }
        head[i/dimension] = build_tree_for_RGMTS(temp,all,dimension,node_num,e_dimension,extend_num);

        for(j=0;j<node_num;j++){
            for(k=0;k<extend;k++){
                temp[j].index[e_dimension+k] =  (limt-2*limt*((double)rand())/((double) RAND_MAX));
            }
        }
    }
    return head;
}

void get_GMTS_index(m_index *top_m_index, int group_num, int words_num, int document_num, int dimension, int e_dimension, int e_words_num, int extend, int d_extend, int *s, double ***matrix)
{
    extern struct m_index *m_index_head,*GMTS_documents_lists;
    extern pfgmrsii *GMTS_head;

    time_start("building GMTS index...");
    GMTS_head=bulid_GMTS(top_m_index, group_num, words_num, document_num, dimension, extend);
    encrypt_GMTS_and_RGMTS_index(GMTS_head, group_num, words_num, e_dimension, s, matrix);
    GMTS_documents_lists=bulid_FILES_LIST(m_index_head, words_num, document_num, dimension, d_extend, extend, 1);
    encrypt_FILES_LIST(GMTS_documents_lists, document_num, e_dimension, words_num, e_words_num, group_num, s, matrix);
    time_end("create DS FGMRS-II index");
}

void get_RGMTS_index(m_index *top_m_index, int group_num, int words_num,int document_num,int dimension, int e_dimension, int e_words_num, int extend, int extend_num, int key_len,int d_extend,int *s,double ***matrix,double ***E_matrix){
    extern struct m_index *m_index_head,*RGMTS_documents_lists;
    extern pfgmrsii *RGMTS_head;

    int ex_dimension;
    ex_dimension= e_dimension+extend;

    time_start("building RGMTS index...");
    RGMTS_head=bulid_RGMTS(top_m_index, words_num, document_num, dimension, extend_num, key_len, extend);
    encrypt_GMTS_and_RGMTS_index(RGMTS_head, group_num, words_num,ex_dimension ,s, E_matrix);
    RGMTS_documents_lists=bulid_FILES_LIST(m_index_head, words_num, document_num*extend_num, dimension, d_extend, extend, 2);
    encrypt_FILES_LIST(RGMTS_documents_lists, document_num*extend_num, e_dimension, words_num, e_words_num, group_num, s, matrix);
    time_end("create DS FGMRS-II index");

}

void
get_GMTS_and_RGMTS_index(int words_num,int document_num,int dimension,int new_b, int extend, int extend_num, int key_len,int d_extend,int *s,double ***matrix,double ***E_matrix){
    extern struct m_index *m_index_head;
    m_index *top_m_index=NULL;

    int group_num, e_words_num, e_dimension;
    group_num = words_num/dimension;
    e_dimension = dimension+extend;
    e_words_num = (group_num+1)*e_dimension;

    disturb_documents(document_num,extend_num,key_len);
    top_m_index = extend_original_index(m_index_head, words_num, document_num);
    get_GMTS_index(top_m_index,  group_num, words_num, document_num, dimension, e_dimension, e_words_num, extend, d_extend, s, matrix);
    get_RGMTS_index(top_m_index, group_num, words_num, document_num, dimension, e_dimension, e_words_num, extend, extend_num, key_len, d_extend, s, matrix, E_matrix);
    free_index(top_m_index,document_num);
}



