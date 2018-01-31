#include "head.h"

extern pfgmrsii *GMTS_head,*RGMTS_head;
extern m_index *GMTS_documents_lists,*RGMTS_documents_lists;
int is_show,test_turn;
extern int **extend_files;
int max_deep,now;
int magnify=2;
double sqrt_w;
double min_score = 0.1;



int run_turn;


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
        fscanf(f,"%s",head[i].words);
        insert_to_tree(search_tree,head[i].words,i);
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
EGMRS(struct fquery *trapdoor, int document_num, int topnum, int dimension, const struct m_index *m_index_head)
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
            //document_id=d;
            document_id=extend_files[d][0];
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
RGMTS_search_init( struct fquery *trapdoor, struct fquery *i_trapdoor, int group_num, int document_num, int topnum, int e_dimension, int ex_dimension, pfgmrsii *root, m_index *file_lists, int d_extend)
{
    int i=0,d,id,s;
    double sum=0.0,*nindex_1,*nindex_2;
    struct fquery *qtemp;
    struct result *rtemp;
    struct fgmrsii *head;
    double results[document_num],document_id;
    //extern int **documents_temp;
    memset(results,0,sizeof(double)*document_num);
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
            //document_id=documents_temp[d][0]-1;
            document_id=d;
            EX_DFMRS_top = insert_to_result(&EX_DFMRS_results, &EX_DFMRS_unuse, sum,EX_DFMRS_top,topnum,document_id,&EX_DFMRS_min_tf);
        }
    }
    if(is_show) print_results(EX_DFMRS_results, EX_DFMRS_top);
}

double DFMRS_min_tf;
int DFMRS_top;
struct result *DFMRS_unuse_results,*DFMRS_unuse ,*DFMRS_results=NULL;
double DFMRS_all_filter,DFMRS_documents_all;

void
GMTS_search(double *indexs_1, double *indexs_2, fgmrsii *root, fgmrsii *head,int topnum, int dimension, double sum, int deep)
{
    double sum_left=0.0,sum_right=0.0,*nindex_1, *nindex_2;
    struct fgmrsii *left,*right;
    int j;
    if(sum >= min_score && (sum>=DFMRS_min_tf || DFMRS_top<topnum))
    {
        if(head->id == -1)
        {
            if(head->left!=-1)
            {
                DFMRS_all_filter++;
                left = &root[head->left];
                nindex_1 = left->index_1;
                nindex_2 = left->index_2;
                for(j=0; j<dimension; j++) sum_left+=(indexs_1[j]* nindex_1[j]+indexs_2[j]* nindex_2[j]);
            }
            if(head->right!=-1)
            {
                DFMRS_all_filter++;
                right = &root[head->right];
                nindex_1 = right->index_1;
                nindex_2 = right->index_2;
                for(j=0; j<dimension; j++) sum_right+=(indexs_1[j]* nindex_1[j]+indexs_2[j]* nindex_2[j]);
            }
            if(sum_left>sum_right)
            {
                if(sum_left  >= min_score) GMTS_search(indexs_1,indexs_2, root, left,topnum,dimension,sum_left, deep+1);
                if(sum_right >= min_score) GMTS_search(indexs_1,indexs_2, root, right,topnum,dimension,sum_right, deep+1);
            }else{
                if(sum_right >= min_score) GMTS_search(indexs_1,indexs_2, root, right,topnum,dimension,sum_right, deep+1);
                if(sum_left  >= min_score) GMTS_search(indexs_1,indexs_2, root, left,topnum,dimension,sum_left, deep+1);
            }
        }
        else
        {
            DFMRS_top = insert_to_result(&DFMRS_results, &DFMRS_unuse, sum,DFMRS_top,topnum,head->id,&DFMRS_min_tf);
        }
    }
}

void
GMTS_search_init( struct fquery *trapdoor, struct fquery *i_trapdoor, int group_num, int document_num, int topnum, int e_dimension, pfgmrsii *root, m_index *file_lists, int d_extend)
{
    int i=0,d,id,s;
    double sum=0.0,*nindex_1,*nindex_2;
    struct fquery *qtemp;
    struct result *rtemp;
    struct fgmrsii *head;
    double results[document_num],document_id;
    memset(results,0,sizeof(double)*document_num);
    qtemp = trapdoor;

    while(qtemp!=NULL)
    {
        id = qtemp->id;
        head = root[id];
        sum=0.0;
        DFMRS_min_tf=-10.0;
        DFMRS_top=0;
        Init_result(DFMRS_unuse_results, magnify*topnum);
        DFMRS_unuse = DFMRS_unuse_results;
        DFMRS_results=NULL;
        DFMRS_all_filter++;

        nindex_1=head[0].index_1;
        nindex_2=head[0].index_2;
        for(i=0; i<e_dimension; i++)
        {
            sum+=(qtemp->D_1[i]*nindex_1[i]+qtemp->D_2[i]*nindex_2[i]) ;
        }
        if(sum >= min_score) GMTS_search(qtemp->D_1, qtemp->D_2 ,head, &head[0], topnum*magnify, e_dimension, sum, 0);

        rtemp = DFMRS_results;
        while(rtemp!=NULL){
            for(i=0;i<rtemp->used;i++) results[rtemp->documents[i]]=1;
            rtemp=rtemp->next;
        }
        qtemp=qtemp->next;
    }

    DFMRS_min_tf=0.0;
    DFMRS_top=0;
    Init_result(DFMRS_unuse_results, magnify*topnum);
    DFMRS_unuse = DFMRS_unuse_results;
    DFMRS_results=NULL;

    for(d=0;d<document_num;d++){
        if(results[d]!=1) continue;
        nindex_1=file_lists[d].index_1;
        nindex_2=file_lists[d].index_2;
        DFMRS_documents_all++;

        sum=0;
        qtemp=i_trapdoor;
        while(qtemp!=NULL)
        {
            id = qtemp->id;
            s=id*e_dimension;
            for(i=0;i<e_dimension;i++) sum+=(qtemp->D_1[i]*nindex_1[s+i]+qtemp->D_2[i]*nindex_2[s+i]);
            qtemp=qtemp->next;
        }
        if(sum >= min_score && (sum>=DFMRS_min_tf || DFMRS_top<topnum))
        {
            //document_id=d;
            document_id=extend_files[d][0];
            DFMRS_top = insert_to_result(&DFMRS_results, &DFMRS_unuse, sum,DFMRS_top,topnum,document_id,&DFMRS_min_tf);
        }
    }
    if(is_show) print_results(DFMRS_results, DFMRS_top);
}

double E_TPDS15_min_tf;
int E_TPDS15_top,document_id;
struct result *E_TPDS15_unuse=NULL,*E_TPDS15_results=NULL,*E_TPDS15_unuse_results=NULL;
double E_TPDS15_filter;
void
tpds_search(struct Node *head, double *tpds_trapdoor_1, double *tpds_trapdoor_2, int words_num, int topnum, int deep)
{
    double sum=0.0;
    int i;
    if(head==NULL) return;
    E_TPDS15_filter++;
    for(i=0; i<words_num; i++) sum+=(tpds_trapdoor_1[i]*(head->index_1[i])+tpds_trapdoor_2[i]*(head->index_2[i]));
    if(sum>0.000001 && (sum>=E_TPDS15_min_tf || E_TPDS15_top<topnum))
    {
        if(head->id>0)
        {
            //document_id=extend_files[head->id][0];
            document_id=head->id;
            E_TPDS15_top = insert_to_result(&E_TPDS15_results, &E_TPDS15_unuse, sum,E_TPDS15_top,topnum,document_id,&E_TPDS15_min_tf);
        }
        else
        {
            tpds_search(head->left,tpds_trapdoor_1,tpds_trapdoor_2,words_num,topnum,deep+1);
            tpds_search(head->right,tpds_trapdoor_1,tpds_trapdoor_2,words_num,topnum,deep+1);
        }
    }
}

static struct result*
tpds_encrypt_search(double *tpds_trapdoor_1, double *tpds_trapdoor_2, int words_num, int topnum,struct Node *tpds_index_head)
{
    E_TPDS15_min_tf=0.0;
    E_TPDS15_top=0;
    Init_result(E_TPDS15_unuse_results, topnum);
    E_TPDS15_unuse=E_TPDS15_unuse_results;
    E_TPDS15_results=NULL;
    tpds_search(tpds_index_head,tpds_trapdoor_1,tpds_trapdoor_2,words_num,topnum,0);
    if(is_show)
    {
        print_results(E_TPDS15_results, E_TPDS15_top);
    }
    return NULL;
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

static struct fquery *
create_LIST_trapdoor(struct fquery *i_trapdoor, struct fquery *query_m, int words_num, int dimension, int trun, int extend, int d_extend, double a, double add_r, int *s ,double ***matrix)
{
    struct fquery *head=NULL,*i_temp, *temp;
    int t,t1,w,i,j,dummy_all;
    int e_dimension;
    int d_len = d_extend*2;
    int group_num = words_num/dimension;
    e_dimension =dimension+d_len;

    double document_1[e_dimension],document_2[e_dimension],sum,stemp;
    double sqrt_w;
    sqrt_w  = add_r*a;

    i_temp=i_trapdoor;
    srand(time(0));
    dummy_all=extend/2;
    j=0;

    /**
     *  start: coping the first d dimension from query one to LIST trapdoor
     */
    while(i_temp!=NULL){
        temp = &query_m[j++];
        if(temp->query==NULL)
        {
            temp->query = (double *)Malloc(e_dimension*sizeof(double));
            temp->D_1   = (double *)Malloc(e_dimension*sizeof(double));
            temp->D_2   = (double *)Malloc(e_dimension*sizeof(double));
        }

        temp->id = i_temp->id;
        memset(temp->query,0,sizeof(double)*e_dimension);
        temp->trun = trun;
        if(head==NULL) temp->next =NULL;
        else  temp->next =head;
        head = temp;

        for(i=0;i<dimension;i++) {
            temp->query[i] = i_temp->query[i];
        }

        i_temp=i_temp->next;
    }
    /** end coping**/

    /**
    * adding a vector to the query group of FILE LIST search.
    */
    temp = &query_m[j++];
    if(temp->query==NULL)
    {
            temp->query = (double *)Malloc(e_dimension*sizeof(double));
            temp->D_1   = (double *)Malloc(e_dimension*sizeof(double));
            temp->D_2   = (double *)Malloc(e_dimension*sizeof(double));
    }
    temp->id = group_num;
    memset(temp->query,0,sizeof(double)*e_dimension);
    temp->trun = trun;
    if(head==NULL) temp->next =NULL;
    else  temp->next =head;
    head = temp;
    /** end **/

    /**
     * -> adding random numbers
     **/
    for(i=0;i<dummy_all;i++){
        t1=rand()%extend;
        while(temp->query[t1]!=0){
            t1=(t1+1)%extend;
        }
        temp->query[t1] = sqrt_w;
    }
    /** end **/

    /**
     *-> adding other random numbers
     **/
    for(w=0; w<d_len; w++){
        sum = 0;
        temp = head;
        while(temp!=NULL){
            if(temp->next!=NULL){
                stemp = add_r*(5.0- ((double)(rand()%100))/10.0);
                temp->query[w+dimension]=stemp;
                sum+=stemp;
            }else{
                temp->query[w+dimension]=0-sum;
            }
            temp=temp->next;
        }
    }
    /** end **/

    /**
     * encrypting LIST trapdoor
     **/
    temp=head;
    while(temp!=NULL){
        t=temp->id;
        for(i=0,j=t*e_dimension; i<e_dimension; i++,j++)
        {
            if(s[j%words_num]==1)
            {
                document_1[i] = document_2[i] = temp->query[i];
            }
            else
            {
                document_1[i] = 2.5-(double)(rand()%50)/10.0;
                document_2[i] = temp->query[i] - document_1[i];
            }
        }
        t %= group_num;
        for(i=0; i<e_dimension; i++)
        {
            temp->D_1[i] =calculate(document_1, 1, t ,i, e_dimension, matrix);
            temp->D_2[i] =calculate(document_2, 3, t,i, e_dimension, matrix);
        }
        temp=temp->next;
    }

    return head;
}

static struct fquery *
init_trapdoor(unsigned int type, struct fquery *query_m, int words_num,int *search_list, int number, int dimension, int e_dimension, int trun, int extend, struct words *words_list, int *group_num,int extend_num,int key_len, double a, double add_r, int *s ,double ***matrix)
{
    struct fquery *head=NULL, *temp;
    int t, t1, word_id, w, g=0, i, j, dummy_all;
    int  start = dimension;
    double sqrt_w;
    double document_1[e_dimension], document_2[e_dimension];

    sqrt_w  = add_r*a;

    for(w=0; w<number; w++)
    {
        word_id = search_list[w];
        t=word_id/dimension;
        temp = &query_m[t];

        if(temp->query==NULL)
        {
            temp->id = t;
            temp->query = (double *)Malloc(e_dimension*sizeof(double));
            temp->D_1   = (double *)Malloc(e_dimension*sizeof(double));
            temp->D_2   = (double *)Malloc(e_dimension*sizeof(double));
        }

        if(trun != temp->trun)
        {
            g++;
            memset(temp->query,0,sizeof(double)*e_dimension);
            temp->trun = trun;
            if(head==NULL) temp->next =NULL;
            else  temp->next =head;
            head = temp;
            temp->wordlists_used=0;
        }

        temp->wordlists[temp->wordlists_used]=word_id;
        temp->wordlists_used++;
        temp->query[word_id-t*dimension] = add_r*words_list[word_id].idf;
    }

    /** start: type = 2
     * when type is equal to 2, this query is belong to RGMTS.
     * so assigning key for each query.
     **/
    if(type & 2){
        int all_len_of_key = extend_num*key_len;
        double keys[all_len_of_key];

        /** generating key **/
        srand(time(0));
        for(i=0;i<all_len_of_key;i+=extend_num){
            t=rand()%extend_num;
            for(j=0;j<extend_num;j++){
                if(j==t){
                    keys[i+j]=0;
                }else{
                    keys[i+j]=-5.8-(double)(rand()%5);
                }
            }
        }

        /** inserting key to each query **/
        start += all_len_of_key;
        temp = head;
        while(temp!=NULL){
            for(i=0; i<all_len_of_key; i++){
                temp->query[dimension+i]=keys[i];
            }
            temp=temp->next;
        }
    }
    /** end assigning key **/


    /**
     *start: inserting random numbers to each query
     **/
    temp=head;
    dummy_all=extend/2;
    while(temp!=NULL){
        t=temp->id;
        for(i=0;i<dummy_all;i++){
            t1=rand()%extend;
            while(temp->query[start+t1]!=0){
                t1=(t1+1)%extend;
            }
            temp->query[start+t1] = sqrt_w;
        }
        temp=temp->next;
    }
    /** end: inserting random numbers **/


    /**
     * start: encrypting trapdoor
     **/
    temp=head;
    while(temp!=NULL){
        t=temp->id;
        for(i=0,j=t*e_dimension; i<e_dimension; i++,j++)
        {
            if(s[j%words_num]==1)
            {
                document_1[i] = document_2[i] = temp->query[i];
            }else
            {
                document_1[i] = 2.5-(double)(rand()%50)/10.0;
                document_2[i] = temp->query[i] - document_1[i];
            }
        }
        for(i=0; i<e_dimension; i++)
        {
            temp->D_1[i] =calculate(document_1, 1, t,i, e_dimension, matrix);
            temp->D_2[i] =calculate(document_2, 3, t,i, e_dimension, matrix);
        }
        temp=temp->next;
    }
    /** end: encryption **/

    (*group_num)=g;
    return head;
}

void
create_tpds_trapdoor(int *search_list, double *tpds_trapdoor, double *tpds_trapdoor_1, double *tpds_trapdoor_2, int number, int word_num, struct words *words_list, int *s , double ***matrix)
{
    int i,t;
    double t1,document_1[word_num],document_2[word_num];
    for(i=0; i<number; i++)
    {
        t=search_list[i];
        t1=words_list[t].idf;
        tpds_trapdoor[t] = t1;
    }
    for(i=0; i<word_num; i++)
    {
        if(s[i]==1)
        {
            document_1[i] = document_2[i] = tpds_trapdoor[i];
        }
        else
        {
            document_1[i] = 2.5-(double)(rand()%50)/10.0;
            document_2[i] = tpds_trapdoor[i] - document_1[i];
        }
    }
    for(i=0; i<word_num; i++)
    {
        tpds_trapdoor_1[i]=calculate(document_1,1,0,i,word_num,matrix);
        tpds_trapdoor_2[i]=calculate(document_2,3,0,i,word_num,matrix);
    }

}

void
m_search(struct m_index *m_index_head, struct m_index * max_index_head, struct Node *tpds_index_head, int words_num, int document_num, int dimension, int topnum, int extend,int *s,double ***matrix,double ***DF_matrix,double ***E_DF_matrix,int extend_num, int key_len,int d_extend)
{
    /**
        搜索入口函数分别执行：
        1.没有加密的words倒排索引搜索
        2.加密的以words的倒排索引搜索
        3.引入过滤机制的倒排索引搜索
        4.tpds 2015中提出的搜索（末加密版本）
        5.tpds 2015中提出的搜索（加密版本）
    **/
    struct words_search_tree *search_tree = NULL;
    struct words *words_list = NULL;
    struct fquery query_m[words_num/dimension], query_RGMTS[words_num/dimension], query_GMTS[words_num/dimension], query_LIST[words_num/dimension + 1], *trapdoor, *RGMTS_trapdoor, *GMTS_trapdoor, *LIST_trapdoor;
    int numbers,lists[200],i,trun=0,group_num,df_group_num;
    double *tpds_trapdoor,*tpds_trapdoor_1,*tpds_trapdoor_2,a;

    tpds_trapdoor = (double *)Malloc(words_num*sizeof(double));
    tpds_trapdoor_1 = (double *)Malloc(words_num*sizeof(double));
    tpds_trapdoor_2 = (double *)Malloc(words_num*sizeof(double));

    memset(query_m,0,sizeof(query_m));
    memset(query_RGMTS,0,sizeof(query_RGMTS));
    memset(query_GMTS,0,sizeof(query_GMTS));
    memset(query_LIST,0,sizeof(query_LIST));

    search_tree = create_words_search_node();
    words_list = get_client_index(words_num,document_num,search_tree);
    get_client_list(words_list,words_num, document_num);


    unuse_results = (struct result *)Malloc(topnum*sizeof(struct result));

    DFMRS_unuse_results = (struct result *)Malloc(topnum*30*sizeof(struct result));
    EX_DFMRS_unuse_results = (struct result *)Malloc(topnum*30*sizeof(struct result));

    E_TPDS15_unuse_results = (struct result *)Malloc(topnum*sizeof(struct result));

    max_deep=log2(words_num)+1; //树的最大高度

    //srand(time(0));
    srand(time(0));

    printf("keywords:\n");

    double test_EGMRS,test_TPDS,test_my_trapdoor,test_DF_trapdoor,test_tpds_trapdoor,test_DFMRS;
    int test;
    double test_all_E_TPDS15;
    double test_hit_E_TPDS15,test_hit_E_DFMRS,test_all_E_DFMRS,privacy_E_DFMRS;
    double privacy_E_TPDS15;
    double add_r,my_group,df_group;

    double test_EX_DFMRS,test_all_E_EX_DFMRS,test_hit_E_EX_DFMRS,privacy_E_EX_DFMRS;

    double test_hit_E_DFMRS_O,test_hit_E_EX_DFMRS_O,test_all_E_DFMRS_O,test_all_E_EX_DFMRS_O;
    double privacy_E_DFMRS_O,privacy_E_EX_DFMRS_O;

    /***
    1. keyword number
    2. variance
    3. is show?
    4. turn number.
    ***/
    int ss=0;
    /*numbers=10;
    a=0.02;
    is_show=0;
    test_turn=100;
    magnify =2;*/
    while(scanf("%d %lf %d %d %d", &numbers,&a, &is_show, &test_turn, &magnify)!=EOF)
    {
        ss++;
        test_EGMRS=0;
        test_TPDS=0;
        test_DFMRS=0;
        test_EX_DFMRS=0;
        test_my_trapdoor=0;
        test_DF_trapdoor=0;
        test_tpds_trapdoor=0;
        test_all_E_TPDS15=0;
        test_hit_E_TPDS15=0;
        privacy_E_DFMRS=0;
        privacy_E_EX_DFMRS=0;
        privacy_E_DFMRS_O=0;
        privacy_E_EX_DFMRS_O=0;


        E_TPDS15_filter = 0;

        DFMRS_documents_all=0;
        EX_DFMRS_documents_all=0;
        EX_DFMRS_all_filter=0;
        DFMRS_all_filter=0;
        test_hit_E_DFMRS=0;
        test_hit_E_EX_DFMRS=0;
        test_all_E_DFMRS=0;
        test_all_E_EX_DFMRS=0;
        test_hit_E_DFMRS_O=0;
        test_hit_E_EX_DFMRS_O=0;
        test_all_E_DFMRS_O=0;
        test_all_E_EX_DFMRS_O=0;

        privacy_E_TPDS15=0;

        my_group=0;
        df_group=0;

       // int list_temp[10]={37, 3169, 3178, 3181, 3031, 1070, 110, 856, 10, 1521};

        for(test=0; test<test_turn; test++)
        {
            run_turn=ss*1000+test+1;
            trun++;

            if(is_show) printf("word: [");
            for(i=0; i<numbers; i++)
            {
                //lists[i]=list_temp[i];
                lists[i]=rand()%words_num;
                if(is_show) printf("%d:%s ",lists[i],words_list[lists[i]].words);
            }
            if(is_show) printf("]\n\n");

            /**printf("enter %d words:\n",numbers);
            for(i=0; i<numbers; i++)
            {
               scanf("%d", &lists[i]);
               lists[i]=rand()%words_num;

            }
            printf("word: [");
            for(i=0;i<numbers;i++){
                printf("%d:%s ",lists[i],words_list[lists[i]].words);
            }
            printf("]\n\n");**/

            add_r = (double)(rand()%10+1);

            time_start("create trapdoor");
            trapdoor = create_my_trapdoor(query_m,words_num, lists,numbers,dimension, trun,extend, words_list,&group_num);
            my_group+=group_num;
            test_my_trapdoor+=time_end("create trapdoor");

            time_start("create DF  trapdoor");
            RGMTS_trapdoor = init_trapdoor(2, query_RGMTS, words_num, lists, numbers, dimension, dimension + extend + key_len*extend_num, trun, extend, words_list, &df_group_num, extend_num, key_len, a, add_r, s , E_DF_matrix);
            GMTS_trapdoor  = init_trapdoor(1, query_GMTS,  words_num, lists, numbers, dimension, dimension + extend,                      trun, extend, words_list, &df_group_num, extend_num, key_len, a, add_r, s , DF_matrix);
            LIST_trapdoor  = create_LIST_trapdoor(GMTS_trapdoor, query_LIST, words_num, dimension, trun, extend, d_extend, a, add_r, s , DF_matrix);
            df_group+=df_group_num;
            test_DF_trapdoor+=time_end("create DF trapdoor");
/**
            time_start("create tpds trapdoor");
            memset(tpds_trapdoor,0,sizeof(double)*words_num);
            memset(tpds_trapdoor_1,0,sizeof(double)*words_num);
            memset(tpds_trapdoor_2,0,sizeof(double)*words_num);
            create_tpds_trapdoor(lists, tpds_trapdoor, tpds_trapdoor_1, tpds_trapdoor_2, numbers, words_num, words_list, s , matrix);
            test_tpds_trapdoor+=time_end("create tpds trapdoor");
**/
            time_start("EGMRS search");
            EGMRS(trapdoor, document_num, topnum, dimension, m_index_head); //加密的以words的倒排索引搜索
            test_EGMRS+=time_end("EGMRS use");

            time_start("GMTS search");
            GMTS_search_init( GMTS_trapdoor, LIST_trapdoor, df_group_num, document_num, topnum, dimension+extend, GMTS_head, GMTS_documents_lists, d_extend);
            test_DFMRS+=time_end("GMTS search");

            time_start("RGMTS search");
            RGMTS_search_init(RGMTS_trapdoor, LIST_trapdoor, df_group_num, document_num*extend_num, topnum, dimension+extend, dimension+key_len*extend_num+extend, RGMTS_head, RGMTS_documents_lists, d_extend);
            test_EX_DFMRS+=time_end("RGMTS search");
/**
            time_start("encrypt tpds search");
            tpds_encrypt_search(tpds_trapdoor_1, tpds_trapdoor_2, words_num, topnum,tpds_index_head);
            test_TPDS+=time_end("encrypt tpds search");
**/

            compare_results(EGMRS_results,DFMRS_results,EGMRS_top,DFMRS_top,&test_all_E_DFMRS,&test_hit_E_DFMRS,&privacy_E_DFMRS,&test_all_E_DFMRS_O,&test_hit_E_DFMRS_O,&privacy_E_DFMRS_O);
            compare_results(EGMRS_results,EX_DFMRS_results,EGMRS_top,EX_DFMRS_top,&test_all_E_EX_DFMRS,&test_hit_E_EX_DFMRS,&privacy_E_EX_DFMRS,&test_all_E_EX_DFMRS_O,&test_hit_E_EX_DFMRS_O,&privacy_E_EX_DFMRS_O);

           // compare_results(EGMRS_results,E_TPDS15_results,EGMRS_top,E_TPDS15_top,&test_all_E_TPDS15,&test_hit_E_TPDS15,&privacy_E_TPDS15);

        }

        //printf("create_my_trapdoor  = %lf\n",test_my_trapdoor/test_turn);
        //printf("creating GMTS and RGMTS trapdoor    = %lf\n",test_DF_trapdoor/test_turn);
        //printf("create_tpds_trapdoor= %lf\n",test_tpds_trapdoor/test_turn);
       // printf("my group=%.3lf DF group %.3lf\n",my_group/test_turn,df_group/test_turn);

        printf("\n");
        //printf("EGMRS              = %.3lf\n",test_EGMRS/test_turn);
        printf("GMTS               = %.3lf documents_all=%.3f all_filter        =%.3f\n",test_DFMRS/test_turn,DFMRS_documents_all/test_turn,DFMRS_all_filter/test_turn);
        printf("RGMTS              = %.3lf documents_all=%.3f all_filter        =%.3f\n",test_EX_DFMRS/test_turn,EX_DFMRS_documents_all/test_turn,EX_DFMRS_all_filter/test_turn);
//        printf("tpds_encrypt_search= %.3lf all_filter   =%.3f\n",test_TPDS/test_turn,E_TPDS15_filter/test_turn);
        printf("\n");

        printf("GMTS precision     = %.1f/%.1f   %.3lf privacy=%.3lf  OP= %.1f/%.1f   %.3lf   privacy_O=%.3lf\n",test_hit_E_DFMRS,test_all_E_DFMRS,test_hit_E_DFMRS/ test_all_E_DFMRS,privacy_E_DFMRS/((double) test_turn),test_hit_E_DFMRS_O,test_all_E_DFMRS_O,test_hit_E_DFMRS_O/ test_all_E_DFMRS_O,privacy_E_DFMRS_O/((double) test_turn));
        printf("RGMTS precision    = %.1f/%.1f   %.3lf privacy=%.3lf  OP= %.1f/%.1f   %.3lf   privacy_O=%.3lf\n",test_hit_E_EX_DFMRS,test_all_E_EX_DFMRS,test_hit_E_EX_DFMRS/ test_all_E_EX_DFMRS,privacy_E_EX_DFMRS/((double) test_turn),test_hit_E_EX_DFMRS_O,test_all_E_EX_DFMRS_O,test_hit_E_EX_DFMRS_O/ test_all_E_EX_DFMRS_O,privacy_E_EX_DFMRS_O/((double) test_turn));
        //printf("E_TPDS2015 precision= %.1f/%.1f   %.3lf \n",test_hit_E_TPDS15,test_all_E_TPDS15,((double) test_hit_E_TPDS15)/((double) test_all_E_TPDS15));

        printf("\nkeywords:\n");
    }

}
