#include "rgmts.h"


int max_deep, now;
double sqrt_w;
double min_score = 0.1;

static int
findword(struct words_search_tree * search_tree,char *words)
{
    /**
        查找words 的 id
        如果此words不存在返回-1
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
        if(next==NULL) return -1;
        if(i+1!=lens)
        {
            temp = next;
            next = temp->child;
        }
    }
    if(next!=NULL && next ->isend == 1) return next ->id;
    else return -1;

}

struct result *unuse_results=NULL,*EGMRS_results=NULL;
int EGMRS_top;

static struct result *
EGMRS(struct fquery *trapdoor, int document_num, int topnum, int dimension, const struct m_index *m_index_head, int **extend_files, int is_show)
{
    double min_tf=0.0,sum;
    int d,t,i,j,document_id;
    struct fquery *temp;
    struct result *unuse=NULL;
    Init_result(unuse_results, topnum);
    unuse = unuse_results;
    EGMRS_top=0;

    for(d=0; d<document_num; d++)
    {
        sum=0.0;
        temp = trapdoor;
        while(temp!=NULL)
        {
            t=temp->id;
            for(i=0,j=t*dimension; i<dimension; i++,j++)
            {
                sum+=(temp->query[i]*(m_index_head[j].index[d]));
            }
            temp=temp->next;
        }
        if(sum>0.000001 && (sum>=min_tf || EGMRS_top<topnum))
        {
            document_id=d;
            //document_id=extend_files[d][0];
            EGMRS_top = insert_to_result(&EGMRS_results, &unuse, sum,EGMRS_top,topnum,document_id,&min_tf);
        }
    }
    if(is_show) print_results(EGMRS_results, EGMRS_top);
    return NULL;
}

double EX_DFMRS_min_tf;
int EX_DFMRS_top;
struct result *EX_DFMRS_unuse_results,*EX_DFMRS_unuse ,*EX_DFMRS_results=NULL;
double EX_DFMRS_all_filter,EX_DFMRS_documents_all;

void
RGMTS_search(double *indexs_1, double *indexs_2, fgmrsii *root, fgmrsii *head,int topnum, int dimension, double sum, int deep)
{
    double sum_left=0.0,sum_right=0.0,*nindex_1, *nindex_2;
    struct fgmrsii *left,*right;
    int j;
    if(sum >= min_score && (sum>=EX_DFMRS_min_tf || EX_DFMRS_top<topnum))
    {
        if(head->id == -1)
        {
            if(head->left!=-1)
            {
                EX_DFMRS_all_filter++;
                left = &root[head->left];
                nindex_1 = left->index_1;
                nindex_2 = left->index_2;
                for(j=0; j<dimension; j++) sum_left+=(indexs_1[j]* nindex_1[j]+indexs_2[j]* nindex_2[j]);
            }
            if(head->right!=-1)
            {
                EX_DFMRS_all_filter++;
                right = &root[head->right];
                nindex_1 = right->index_1;
                nindex_2 = right->index_2;
                for(j=0; j<dimension; j++) sum_right+=(indexs_1[j]* nindex_1[j]+indexs_2[j]* nindex_2[j]);
            }
            if(sum_left>sum_right)
            {
                if(sum_left  >= min_score) RGMTS_search(indexs_1,indexs_2, root, left,topnum,dimension,sum_left, deep+1);
                if(sum_right >= min_score) RGMTS_search(indexs_1,indexs_2, root, right,topnum,dimension,sum_right, deep+1);
            }else{
                if(sum_right >= min_score) RGMTS_search(indexs_1,indexs_2, root, right,topnum,dimension,sum_right, deep+1);
                if(sum_left  >= min_score) RGMTS_search(indexs_1,indexs_2, root, left,topnum,dimension,sum_left, deep+1);
            }
        }
        else
        {
            EX_DFMRS_top = insert_to_result(&EX_DFMRS_results, &EX_DFMRS_unuse, sum,EX_DFMRS_top,topnum,head->id,&EX_DFMRS_min_tf);
        }
    }
}


void
RGMTS_search_init( struct fquery *trapdoor, struct fquery *i_trapdoor, int document_num, int topnum, int magnify, int e_dimension, int ex_dimension, pfgmrsii *root, m_index *file_lists, int d_extend, int is_show, int **documents_temp)
{
    int i=0, d, id, s, document_id;
    double sum=0.0, *nindex_1, *nindex_2;
    struct fquery *qtemp;
    struct result *rtemp;
    struct fgmrsii *head;
    double *results = (double*) Calloc(document_num, sizeof(double));

    //extern int **documents_temp;
    qtemp = trapdoor;

    while(qtemp!=NULL)
    {
        id = qtemp->id;
        head = root[id];
        sum=0.0;
        EX_DFMRS_min_tf=0.1;
        EX_DFMRS_top=0;
        Init_result(EX_DFMRS_unuse_results, magnify*topnum);
        EX_DFMRS_unuse = EX_DFMRS_unuse_results;
        EX_DFMRS_results=NULL;
        EX_DFMRS_all_filter++;

        nindex_1=head[0].index_1;
        nindex_2=head[0].index_2;
        for(i=0; i<ex_dimension; i++)
        {
            sum+=(qtemp->D_1[i]*nindex_1[i]+qtemp->D_2[i]*nindex_2[i]) ;
        }
        if(sum >= min_score) RGMTS_search(qtemp->D_1,qtemp->D_2,head,&head[0],topnum*magnify,ex_dimension,sum,0);

        rtemp = EX_DFMRS_results;
        while(rtemp!=NULL){
            for(i=0;i<rtemp->used;i++)  results[rtemp->documents[i]]=1;
            rtemp=rtemp->next;
        }
        //print_results(EX_DFMRS_results, EX_DFMRS_top);
        qtemp=qtemp->next;
    }

    EX_DFMRS_min_tf=0.1;
    EX_DFMRS_top=0;
    Init_result(EX_DFMRS_unuse_results, magnify*topnum);
    EX_DFMRS_unuse = EX_DFMRS_unuse_results;
    EX_DFMRS_results=NULL;

    for(d=0;d<document_num;d++){
        if(results[d]!=1) continue;
        nindex_1=file_lists[d].index_1;
        nindex_2=file_lists[d].index_2;
        EX_DFMRS_documents_all++;

        sum=0;
        qtemp=i_trapdoor;
        while(qtemp!=NULL)
        {
            id = qtemp->id;
            s=id*e_dimension;
            for(i=0;i<e_dimension;i++) sum+=(qtemp->D_1[i]*nindex_1[s+i]+qtemp->D_2[i]*nindex_2[s+i]);
            qtemp=qtemp->next;
        }
        if(sum >= min_score && (sum>=EX_DFMRS_min_tf || EX_DFMRS_top<topnum))
        {
            document_id=documents_temp[d][0]-1;
            //document_id=d;
            EX_DFMRS_top = insert_to_result(&EX_DFMRS_results, &EX_DFMRS_unuse, sum,EX_DFMRS_top,topnum,document_id,&EX_DFMRS_min_tf);
        }
    }
    if(is_show) print_results(EX_DFMRS_results, EX_DFMRS_top);
    free(results);
}

static struct fquery *
create_my_trapdoor(struct fquery *query_m, int words_num,int *search_list, int number, int dimension, int trun, int extend, struct words *words_list, int *group_num)
{
    struct fquery *head=NULL,*temp;
    int t,word_id,w,g=0;

    for(w=0; w<number; w++)
    {
        word_id = search_list[w];
        t=word_id/dimension;
        temp = &query_m[t];
        if(temp->query==NULL)
        {
            temp->id = t;
            temp->query = (double *)Malloc((extend+dimension)*sizeof(double));
        }
        if(trun != temp->trun)
        {
            g++;
            memset(temp->query,0,sizeof(double)*(extend+dimension));
            temp->trun = trun;
            if(head==NULL) temp->next =NULL;
            else  temp->next =head;
            head = temp;
        }
        temp->query[word_id-t*dimension] = words_list[word_id].idf;
    }
   (*group_num)=g;
    return head;
}

void
m_search(Keys* my_keys, Trapdoor_list *trapdoor_ret, Index_Group* index_g, int *lists, int numbers, double a, int is_show, int magnify, int words_num, int document_num, int dimension, int topnum, int extend,int extend_num, int key_len,int d_extend)
{
    /**
        搜索入口函数分别执行：
        1.没有加密的words倒排索引搜索
        2.加密的以words的倒排索引搜索
        3.引入过滤机制的倒排索引搜索
    **/

    assert(trapdoor_ret != NULL && trapdoor_ret->query_GMTS != NULL && trapdoor_ret->query_RGMTS != NULL && trapdoor_ret->query_LIST != NULL);
    assert(my_keys != NULL && my_keys->s != NULL && my_keys->words_list != NULL);

    struct words *words_list;
	words_list  = my_keys->words_list;


    struct fquery *trapdoor, *RGMTS_trapdoor, *LIST_trapdoor;

    struct fquery *query_m = Malloc(words_num/dimension*sizeof(fquery));
    memset(query_m,0,words_num/dimension*sizeof(fquery));

    RGMTS_trapdoor = trapdoor_ret->RGMTS_trapdoor;
    LIST_trapdoor = trapdoor_ret->LIST_trapdoor;


    unuse_results = (struct result *)Malloc(topnum*sizeof(struct result));
    EX_DFMRS_unuse_results = (struct result *)Malloc(topnum*30*sizeof(struct result));

    srand(time(0));

    double test_EGMRS,test_my_trapdoor;
    int group_num;

    double test_EX_DFMRS,test_all_E_EX_DFMRS,test_hit_E_EX_DFMRS,privacy_E_EX_DFMRS;

    double test_hit_E_EX_DFMRS_O, test_all_E_EX_DFMRS_O;
    double privacy_E_EX_DFMRS_O;

    test_EGMRS=0;
    test_EX_DFMRS=0;
    test_my_trapdoor=0;
    privacy_E_EX_DFMRS=0;
    privacy_E_EX_DFMRS_O=0;



    EX_DFMRS_documents_all=0;
    EX_DFMRS_all_filter=0;
    test_hit_E_EX_DFMRS=0;
    test_all_E_EX_DFMRS=0;
    test_hit_E_EX_DFMRS_O=0;
	test_all_E_EX_DFMRS_O = 0;


    time_start("create trapdoor", is_show);
    trapdoor = create_my_trapdoor(query_m,words_num, lists,numbers,dimension, 1,extend, words_list,&group_num);
    test_my_trapdoor+=time_end("create trapdoor", is_show);


    time_start("EGMRS search", is_show);
    EGMRS(trapdoor, document_num, topnum, dimension, index_g->m_index_head, index_g->extend_files,is_show); //加密的以words的倒排索引搜索
    test_EGMRS+=time_end("EGMRS use", is_show);


    time_start("RGMTS search", is_show);
    RGMTS_search_init(RGMTS_trapdoor, LIST_trapdoor, document_num*extend_num, topnum, magnify, dimension+extend, dimension+key_len*extend_num+extend, index_g->RGMTS_head, index_g->RGMTS_documents_lists, d_extend, is_show, index_g->documents_temp);
    test_EX_DFMRS+=time_end("RGMTS search", is_show);


    compare_results(EGMRS_results,EX_DFMRS_results,EGMRS_top,EX_DFMRS_top,&test_all_E_EX_DFMRS,&test_hit_E_EX_DFMRS,&privacy_E_EX_DFMRS,&test_all_E_EX_DFMRS_O,&test_hit_E_EX_DFMRS_O,&privacy_E_EX_DFMRS_O, index_g->documents_temp);

    printf("RGMTS              = %.3lf documents_all=%.3f all_filter        =%.3f\n",test_EX_DFMRS,EX_DFMRS_documents_all,EX_DFMRS_all_filter);
    printf("\n");

    printf("RGMTS precision    = %.1f/%.1f   %.3lf privacy=%.3lf  OP= %.1f/%.1f   %.3lf   privacy_O=%.3lf\n",test_hit_E_EX_DFMRS,test_all_E_EX_DFMRS,test_hit_E_EX_DFMRS/ test_all_E_EX_DFMRS,privacy_E_EX_DFMRS,test_hit_E_EX_DFMRS_O,test_all_E_EX_DFMRS_O,test_hit_E_EX_DFMRS_O/ test_all_E_EX_DFMRS_O,privacy_E_EX_DFMRS_O);

    free(query_m);

}
