#include "rgmts.h"
int main(int argc, char *argv[])
{

	int document_num = 4000, words_num = 4000, new_b = 0, dimension = 80, kind = 3, extend = 20, extend_num = 2, key_len = 10, d_extend;
	int is_show = 1, numbers;
	int topnum = 50, ma = 2, i, magnify = 2;
	int next_option;
	double a = 0.02;


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

	d_extend = extend_num*key_len / 2;



	Keys* c_keys =  Client_Init(document_num, words_num, dimension, extend, extend_num, key_len);
	printf("get keys success!\n");
	Trapdoor_list * send_t = Trapdoor_Malloc(words_num, dimension);

	printf("get keys ...\n");
	Keys *i_keys = Server_Init(document_num, words_num, dimension, extend, extend_num, key_len);
	printf("get index ...\n");

	Index_Group* index_g = Get_Index(i_keys, words_num, document_num, dimension, new_b, extend, extend_num, key_len, d_extend, kind, is_show, topnum, ma);
	printf("start search ...\n");

	int lists[1000];
	printf("Input: number a is_show magnify!\n");
	while (scanf("%d %lf %d %d", &numbers, &a, &is_show, &magnify) != EOF)
	{
	    printf("Enter %d number [0,%d):\n",numbers, words_num);
		for (i = 0; i < numbers; i++) scanf("%d", &lists[i]);
		send_t = TrapDoor_Construction(c_keys, send_t, lists, numbers, words_num, document_num, dimension, topnum, extend, extend_num, key_len, d_extend);

		m_search(i_keys, send_t, index_g, lists, numbers, a, is_show, magnify, words_num, document_num, dimension, topnum, extend, extend_num, key_len, d_extend);
		printf("Input: number a is_show test_turn magnify!\n");
	}

	printf("End search!\n");

	Free_Trapdoor(send_t);
	return 0;
}
