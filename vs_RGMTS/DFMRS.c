#include "rgmts.h"


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
disturb_documents(Index_Group *index_g, int document_num,int extend_num,int key_len)
{
    int i,j,t,k,s,e,all_g,all_d;
    int *is_used = Malloc(extend_num*key_len*sizeof(int));
    int *groups = Malloc(extend_num*sizeof(int));
	int **extend_files;
	int **documents_temp;

    all_d=document_num*extend_num;
    t=document_num/key_len;
    all_g=key_len*extend_num;

    extend_files=(int **)Malloc(document_num*sizeof(int *));
    for(i=0;i<document_num;i++) extend_files[i]=(int *)Calloc(extend_num,sizeof(int));
    documents_temp = (int **)Malloc(all_d*sizeof(int *));
    for(i=0;i<all_d;i++) documents_temp[i]=(int *)Calloc(3,sizeof(int));

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
    free(is_used);
    free(groups);
	index_g->documents_temp = documents_temp;
	index_g->extend_files = extend_files;
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
extend_original_index(m_index *root, int words_num, int documents_num, int topnum, int ma)
{
    m_index *extend_result;
    int i,j,top_max=topnum*ma,top_use=0,document_id;
    topval sorted_result[1500];
    double top_min,value;

    extend_result=(m_index *)Malloc(sizeof(m_index)*documents_num);

    for(i=0;i<documents_num;i++){
        extend_result[i].index = (double *)Calloc(words_num,sizeof(double));
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
            for(k=0;k<dimension;k++) nodes[j].index[k] = MAX(lnode->index[k], rnode->index[k]);
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

m_index *
bulid_FILES_LIST(m_index *root, int words_num,int document_num,int dimension,int d_extend, int extend, unsigned int type, int **documents_temp)
{
    /**
    type = 1  for GMTS
    type = 2  for RGMTS
    **/
    m_index *lists;
    int k,s,e,i,j,t,d_len=d_extend*2;
    double *temp = Malloc(d_len*sizeof(double));
    double limt;
    limt  = sqrt(6.0/((double)extend));

    lists=(m_index *)Malloc(document_num*sizeof(m_index));
    t=(words_num/dimension + 1)*(dimension + d_len);//note that: we only consider words_num is the multiple of the dimension

    for(i=0; i < document_num; i++){
        lists[i].index=(double *)Calloc(t,sizeof(double));
    }

    for(i=0; i<document_num; i++){
        lists[i].index_1=(double *)Calloc(t,sizeof(double));
        lists[i].index_2=(double *)Calloc(t,sizeof(double));
    }

    for(i=0;i<document_num;i++){
        memset(temp,0,sizeof(double)*d_len);
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
    free(temp);
    return lists;
}
double *D_1, *D_2;
void encrypt_FILES_LIST(m_index *file_lists, int document_num, int e_dimension, int words_num, int e_words_num, int group_num, int *s, double ***matrix){
    int d,j,k,i,group_id;
    D_1 = Malloc(sizeof(double)*e_dimension);
    D_2 = Malloc(sizeof(double)*e_dimension);
    double *index, t1;

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
    free(D_1);
    free(D_2);
}



void
encrypt_each_node_in_index(struct fgmrsii *root, int group_id, int id, int words_num, int e_dimension,int *s, double ***matrix)
{
    int j;
    struct fgmrsii *head;
    double t1;
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
    D_1 = Malloc(sizeof(double)*e_dimension);
    D_2 = Malloc(sizeof(double)*e_dimension);

    for(i=0;i<group_num;i++){
        encrypt_each_node_in_index(root[i],i,0, words_num,e_dimension,s,matrix);
    }
    free(D_1);
    free(D_2);
}

pfgmrsii *
bulid_RGMTS(m_index *root, int words_num,int document_num,int dimension,int extend_num,int key_len, int extend, int **extend_files, int **documents_temp)
{
    pfgmrsii *head;
    fgmrsii *temp;
    double sum;
    int i,j,s,e,k,t,all,node_num,start,all_node=0,all_d=document_num*extend_num;

    int *d_temp = Malloc(all_d*sizeof(int));
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
        memset(d_temp,0,all_d*sizeof(int));
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
            temp[j].index =   Calloc(ex_dimension,sizeof(double));
        }
        for(j=0;j<node_num;j++) {
            temp[j].index_1 = Calloc(ex_dimension,sizeof(double));
            temp[j].index_2 = Calloc(ex_dimension,sizeof(double));
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
    free(d_temp);
    return head;
}

Index_Group* get_RGMTS_index(Index_Group *index_g, m_index *top_m_index, int group_num, int words_num,int document_num,int dimension, int e_dimension, int e_words_num, int extend, int extend_num, int key_len,int d_extend,int *s,double ***matrix,double ***E_matrix, int is_show){


    struct m_index *RGMTS_documents_lists;
    pfgmrsii *RGMTS_head;

    int ex_dimension;
    ex_dimension= e_dimension+extend;

    time_start("building RGMTS index...", is_show);
    RGMTS_head=bulid_RGMTS(top_m_index, words_num, document_num, dimension, extend_num, key_len, extend, index_g->extend_files, index_g->documents_temp);
    encrypt_GMTS_and_RGMTS_index(RGMTS_head, group_num, words_num,ex_dimension ,s, E_matrix);
    RGMTS_documents_lists=bulid_FILES_LIST(index_g->m_index_head, words_num, document_num*extend_num, dimension, d_extend, extend, 2, index_g->documents_temp);
    encrypt_FILES_LIST(RGMTS_documents_lists, document_num*extend_num, e_dimension, words_num, e_words_num, group_num, s, matrix);
    time_end("create DS FGMRS-II index", is_show);

    index_g->RGMTS_documents_lists = RGMTS_documents_lists;
    index_g->RGMTS_head = RGMTS_head;
    return index_g;

}

Index_Group*
get_GMTS_and_RGMTS_index(Index_Group *index_g, int words_num,int document_num,int dimension,int new_b, int extend, int extend_num, int key_len,int d_extend,int *s,double ***matrix,double ***E_matrix, int is_show, int topnum, int ma){
    if(index_g==NULL || index_g->m_index_head ==NULL){
        Error_Print("error: index_g is empty in get_GMTS_and_RGMTS_index\n", 1);
    }
    m_index *top_m_index=NULL;

    int group_num, e_words_num, e_dimension;
    group_num = words_num/dimension;
    e_dimension = dimension+extend;
    e_words_num = (group_num+1)*e_dimension;

    disturb_documents(index_g, document_num,extend_num,key_len);
    top_m_index = extend_original_index(index_g->m_index_head, words_num, document_num, topnum, ma);
    get_RGMTS_index(index_g, top_m_index, group_num, words_num, document_num, dimension, e_dimension, e_words_num, extend, extend_num, key_len, d_extend, s, matrix, E_matrix, is_show);
    free_index(top_m_index,document_num);
    return index_g;
}



