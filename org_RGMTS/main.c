#include "head.h"
int encrypt_num=0;
int tpds_node_num=0;
int max_imp_node_num =0;
int tree_imp_node_num =0;
int test_hit,test_all;
int topnum=20,ma=2;

struct m_index *m_index_head=NULL,*max_index_head=NULL,*GMTS_documents_lists=NULL,*RGMTS_documents_lists=NULL;
double ***matrix = NULL,***DF_matrix=NULL,***EX_DF_matrix=NULL;;
pfgmrsii *GMTS_head=NULL,*RGMTS_head=NULL;

void
m_index_building(int words_num, int document_num, int dimension, int new_b, int *s, int extend, int kind, int extend_num, int key_len,int d_extend)
{
    char DF_file_name[50],EX_DF_file_name[50];
    int total;

    total = words_num/dimension;
    sprintf(DF_file_name,"./matrix/matrix_%d_%d",dimension+key_len*extend_num,total);
    sprintf(EX_DF_file_name,"./matrix/matrix_%d_%d",dimension+key_len*extend_num+extend,total);
    DF_matrix = get_matrix(DF_file_name,dimension+key_len*extend_num,total);
    EX_DF_matrix = get_matrix(EX_DF_file_name,dimension+key_len*extend_num+extend,total);

    m_index_head = (struct m_index *) get_EGMRS_index(words_num,document_num,dimension,new_b);
    if(kind!=1) {
        get_GMTS_and_RGMTS_index(words_num,document_num,dimension,new_b, extend, extend_num, key_len,d_extend,s,DF_matrix,EX_DF_matrix);
    }

}

struct Node *tpds_index_head = NULL;

void
get_tpds_index(int words_num, int document_num, int new_b, int *s)
{

    tpds_index_head = tpds_index_building(words_num,document_num,new_b,s);
}

int in_one_dimesion(int *temp, int e, int t, int dimesion){
    int i;
    for(i=0;i<e;i++){
        if((temp[i]/dimesion)==(t/dimesion)) return 1;
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    int document_num=1000,words_num=1000,*s,next_option,new_b = 0,dimension=50,kind=3,extend=20,extend_num=2, key_len=10,d_extend;

    char file_name[50];
    const char * const short_options = "w:f:n:d:k:t:s:u:e:y:m:";
    const struct option long_option[]={{"word",1,NULL,'w'},{"file",1,NULL,'f'},{"new",1,NULL,'n'},{"dimension",1,NULL,'d'},{"kind",1,NULL,'k'},{"topnum",1,NULL,'t'},{"show",1,NULL,'s'},{"turn",1,NULL,'u'},{"extend",1,NULL,'u'},{"ma",1,NULL,'m'}};
    do{
        next_option = getopt_long(argc,argv,short_options,long_option,NULL);
        switch(next_option)
        {
            case 'w':
                words_num = atoi(optarg);
                break;
            case 'f':
                document_num = atoi(optarg);
                break;
            case 'n':
                new_b = atoi(optarg);
                break;
            case 'd':
                dimension = atoi(optarg);
                break;
            case 'k':
                kind = atoi(optarg);
                break;
            case 't':
                topnum = atoi(optarg);
                break;
            case 'e':
                extend_num = atoi(optarg);
            case 'y':
                key_len = atoi(optarg);
		break;
            case 'm':
                ma = atoi(optarg);
                break;
        }
    }while(next_option!=-1);

    d_extend=extend_num*key_len/2;

    srand((unsigned int)time(0));
    sprintf(file_name,"./index/s_%d",words_num);
    s=create_s(file_name, words_num);

    switch(kind){
        case 1:
            m_index_building(words_num, document_num, dimension,new_b,s,extend,1,extend_num,key_len,d_extend);
            break;
        case 2:
            get_tpds_index(words_num, document_num, new_b,s);
            break;
        default:
            printf("w=%d f=%d e=%d\n",words_num,document_num,extend_num);
            printf(">>>>>>>>>>>>>>>>>start<<<<<<<<<<<<<<<<<<<<\n");
            m_index_building(words_num, document_num, dimension,new_b,s,extend,0,extend_num,key_len,d_extend);
            //get_tpds_index(words_num, document_num, new_b,s);
            printf("start search ...\n");
            m_search(m_index_head,max_index_head,tpds_index_head,words_num,document_num,dimension,topnum,extend,s,matrix,DF_matrix,EX_DF_matrix,extend_num,key_len,d_extend);
            printf("*****************end**********************\n\n\n");
            break;
    }
    return 0;
}
