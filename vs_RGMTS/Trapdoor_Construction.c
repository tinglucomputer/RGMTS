#include "rgmts.h"

static struct fquery *
create_my_trapdoor(struct fquery *query_m, int words_num, int *search_list, int number, int dimension, int trun, int extend, struct words *words_list, int *group_num)
{
	struct fquery *head = NULL, *temp;
	int t, word_id, w, g = 0;

	for (w = 0; w<number; w++)
	{
		word_id = search_list[w];
		t = word_id / dimension;
		temp = &query_m[t];
		if (temp->query == NULL)
		{
			temp->id = t;
			temp->query = (double *)Malloc((extend + dimension) * sizeof(double));
		}
		if (trun != temp->trun)
		{
			g++;
			memset(temp->query, 0, sizeof(double)*(extend + dimension));
			temp->trun = trun;
			if (head == NULL) temp->next = NULL;
			else  temp->next = head;
			head = temp;
		}
		temp->query[word_id - t*dimension] = words_list[word_id].idf;
	}
	(*group_num) = g;
	return head;
}

static struct fquery *
create_LIST_trapdoor(struct fquery *i_trapdoor, struct fquery *query_m, int words_num, int dimension, int trun, int extend, int d_extend, double a, double add_r, int *s, double ***matrix)
{
	struct fquery *head = NULL, *i_temp, *temp;
	int t, t1, w, i, j, dummy_all;
	int e_dimension;
	int d_len = d_extend * 2;
	int group_num = words_num / dimension;
	e_dimension = dimension + d_len;
	double *document_1 = (double *)Malloc(e_dimension * sizeof(double));
	double *document_2 = (double *)Malloc(e_dimension * sizeof(double));
	double sum, stemp, sqrt_w;
	sqrt_w = add_r*a;

	i_temp = i_trapdoor;
	srand(time(0));
	dummy_all = extend / 2;
	j = 0;

	/**
	*  start: coping the first d dimension from query one to LIST trapdoor
	*/
	while (i_temp != NULL) {
		temp = &query_m[j++];
		if (temp->query == NULL)
		{
			temp->query = (double *)Malloc(e_dimension * sizeof(double));
			temp->D_1 = (double *)Malloc(e_dimension * sizeof(double));
			temp->D_2 = (double *)Malloc(e_dimension * sizeof(double));
		}

		temp->id = i_temp->id;
		memset(temp->query, 0, sizeof(double)*e_dimension);
		temp->trun = trun;
		if (head == NULL) temp->next = NULL;
		else  temp->next = head;
		head = temp;

		for (i = 0; i<dimension; i++) {
			temp->query[i] = i_temp->query[i];
		}

		i_temp = i_temp->next;
	}
	/** end coping**/

	/**
	* adding a vector to the query group of FILE LIST search.
	*/
	temp = &query_m[j++];
	if (temp->query == NULL)
	{
		temp->query = (double *)Malloc(e_dimension * sizeof(double));
		temp->D_1 = (double *)Malloc(e_dimension * sizeof(double));
		temp->D_2 = (double *)Malloc(e_dimension * sizeof(double));
	}
	temp->id = group_num;
	memset(temp->query, 0, sizeof(double)*e_dimension);
	temp->trun = trun;
	if (head == NULL) temp->next = NULL;
	else  temp->next = head;
	head = temp;
	/** end **/

	/**
	* -> adding random numbers
	**/
	for (i = 0; i<dummy_all; i++) {
		t1 = rand() % extend;
		while (temp->query[t1] != 0) {
			t1 = (t1 + 1) % extend;
		}
		temp->query[t1] = sqrt_w;
	}
	/** end **/

	/**
	*-> adding other random numbers
	**/
	for (w = 0; w<d_len; w++) {
		sum = 0;
		temp = head;
		while (temp != NULL) {
			if (temp->next != NULL) {
				stemp = add_r*(5.0 - ((double)(rand() % 100)) / 10.0);
				temp->query[w + dimension] = stemp;
				sum += stemp;
			}
			else {
				temp->query[w + dimension] = 0 - sum;
			}
			temp = temp->next;
		}
	}
	/** end **/

	/**
	* encrypting LIST trapdoor
	**/
	temp = head;
	while (temp != NULL) {
		t = temp->id;
		for (i = 0, j = t*e_dimension; i<e_dimension; i++, j++)
		{
			if (s[j%words_num] == 1)
			{
				document_1[i] = document_2[i] = temp->query[i];
			}
			else
			{
				document_1[i] = 2.5 - (double)(rand() % 50) / 10.0;
				document_2[i] = temp->query[i] - document_1[i];
			}
		}
		t %= group_num;
		for (i = 0; i<e_dimension; i++)
		{
			temp->D_1[i] = calculate(document_1, 1, t, i, e_dimension, matrix);
			temp->D_2[i] = calculate(document_2, 3, t, i, e_dimension, matrix);
		}
		temp = temp->next;
	}
	free(document_1);
	free(document_2);
	return head;
}

static struct fquery *
init_trapdoor(unsigned int type, struct fquery *query_m, int words_num, int *search_list, int number, int dimension, int e_dimension, int trun, int extend, struct words *words_list, int extend_num, int key_len, double a, double add_r, int *s, double ***matrix)
{
	struct fquery *head = NULL, *temp;
	int t, t1, word_id, w, g = 0, i, j, dummy_all;
	int  start = dimension;
	double sqrt_w;
	double *document_1 = (double*)Malloc(e_dimension * sizeof(double));
	double *document_2 = (double*)Malloc(e_dimension * sizeof(double));

	sqrt_w = add_r*a;

	for (w = 0; w<number; w++)
	{
		word_id = search_list[w];
		t = word_id / dimension;
		temp = &query_m[t];

		if (temp->query == NULL)
		{
			temp->id = t;
			temp->query = (double *)Malloc(e_dimension * sizeof(double));
			temp->D_1   = (double *)Malloc(e_dimension * sizeof(double));
			temp->D_2   = (double *)Malloc(e_dimension * sizeof(double));
		}

		if (trun != temp->trun)
		{
			g++;
			memset(temp->query, 0, sizeof(double)*e_dimension);
			temp->trun = trun;
			if (head == NULL) temp->next = NULL;
			else  temp->next = head;
			head = temp;
			temp->wordlists_used = 0;
		}

		temp->wordlists[temp->wordlists_used] = word_id;
		temp->wordlists_used++;
		temp->query[word_id - t*dimension] = add_r*words_list[word_id].idf;
	}

	/** start: type = 2
	* when type is equal to 2, this query is belong to RGMTS.
	* so assigning key for each query.
	**/
	if (type & 2) {
		int all_len_of_key = extend_num*key_len;
		double *keys = (double*)Malloc(all_len_of_key * sizeof(double));

		/** generating key **/
		srand(time(0));
		for (i = 0; i<all_len_of_key; i += extend_num) {
			t = rand() % extend_num;
			for (j = 0; j<extend_num; j++) {
				if (j == t) {
					keys[i + j] = 0;
				}
				else {
					keys[i + j] = -5.8 - (double)(rand() % 5);
				}
			}
		}

		/** inserting key to each query **/
		start += all_len_of_key;
		temp = head;
		while (temp != NULL) {
			for (i = 0; i<all_len_of_key; i++) {
				temp->query[dimension + i] = keys[i];
			}
			temp = temp->next;
		}
		free(keys);
	}
	/** end assigning key **/


	/**
	*start: inserting random numbers to each query
	**/
	temp = head;
	dummy_all = extend / 2;
	while (temp != NULL) {
		t = temp->id;
		for (i = 0; i<dummy_all; i++) {
			t1 = rand() % extend;
			while (temp->query[start + t1] != 0) {
				t1 = (t1 + 1) % extend;
			}
			temp->query[start + t1] = sqrt_w;
		}
		temp = temp->next;
	}
	/** end: inserting random numbers **/


	/**
	* start: encrypting trapdoor
	**/
	temp = head;
	while (temp != NULL) {
		t = temp->id;
		for (i = 0, j = t*e_dimension; i<e_dimension; i++, j++)
		{
			if (s[j%words_num] == 1)
			{
				document_1[i] = document_2[i] = temp->query[i];
			}
			else
			{
				document_1[i] = 2.5 - (double)(rand() % 50) / 10.0;
				document_2[i] = temp->query[i] - document_1[i];
			}
		}
		for (i = 0; i<e_dimension; i++)
		{
			temp->D_1[i] = calculate(document_1, 1, t, i, e_dimension, matrix);
			temp->D_2[i] = calculate(document_2, 3, t, i, e_dimension, matrix);
		}
		temp = temp->next;
	}
	/** end: encryption **/

	free(document_1);
	free(document_2);
	return head;
}


Trapdoor_list *
Trapdoor_Malloc(int words_num, int dimension)
{
	int query_size = words_num / dimension;
	Trapdoor_list *trapdoor_ret = (Trapdoor_list*)Malloc(sizeof(Trapdoor_list));
	trapdoor_ret->query_RGMTS = (fquery*)Malloc(query_size * sizeof(fquery));
	trapdoor_ret->query_GMTS = (fquery*)Malloc(query_size * sizeof(fquery));
	trapdoor_ret->query_LIST = (fquery*)Malloc((query_size + 1) * sizeof(fquery));
	return trapdoor_ret;
}

Trapdoor_list *
TrapDoor_Construction(Keys* my_keys, Trapdoor_list *trapdoor_ret, int *lists, int numbers, int words_num, int document_num, int dimension, int topnum, int extend, int extend_num, int key_len, int d_extend)
{
	/**
	搜索入口函数分别执行：
	1.没有加密的words倒排索引搜索
	2.加密的以words的倒排索引搜索
	3.引入过滤机制的倒排索引搜索
	**/

	assert(trapdoor_ret != NULL && trapdoor_ret->query_GMTS != NULL && trapdoor_ret->query_RGMTS != NULL && trapdoor_ret->query_LIST != NULL);
	assert(my_keys != NULL && my_keys->matrix != NULL && my_keys->matrix_EX != NULL && my_keys->s != NULL && my_keys->words_list != NULL);

	int  *s, trun = 1, i;
	double a;
	int query_size = words_num / dimension;
	double  ***DF_matrix, ***E_DF_matrix;
	struct words *words_list;

	s = my_keys->s;
	DF_matrix = my_keys->matrix;
	E_DF_matrix = my_keys->matrix_EX;
	words_list = my_keys->words_list;
	struct fquery *RGMTS_trapdoor, *GMTS_trapdoor, *LIST_trapdoor;
	struct fquery *query_RGMTS, *query_GMTS, *query_LIST;

	query_GMTS = trapdoor_ret->query_GMTS;
	query_RGMTS = trapdoor_ret->query_RGMTS;
	query_LIST = trapdoor_ret->query_LIST;

	memset(query_RGMTS, 0, query_size * sizeof(fquery));
	memset(query_GMTS, 0, query_size * sizeof(fquery));
	memset(query_LIST, 0, (query_size + 1) * sizeof(fquery));


	double  test_DF_trapdoor = 0.0, add_r;

	/***
	1. keyword number
	2. variance
	3. is show?
	4. turn number.
	***/
	a=0.02;

	add_r = (double)(rand() % 10 + 1);

	RGMTS_trapdoor = init_trapdoor(2, query_RGMTS, words_num, lists, numbers, dimension, dimension + extend + key_len*extend_num, trun, extend, words_list, extend_num, key_len, a, add_r, s, E_DF_matrix);
	GMTS_trapdoor = init_trapdoor(1, query_GMTS, words_num, lists, numbers, dimension, dimension + extend, trun, extend, words_list, extend_num, key_len, a, add_r, s, DF_matrix);
	LIST_trapdoor = create_LIST_trapdoor(GMTS_trapdoor, query_LIST, words_num, dimension, trun, extend, d_extend, a, add_r, s, DF_matrix);

	trapdoor_ret->RGMTS_trapdoor = RGMTS_trapdoor;
	trapdoor_ret->LIST_trapdoor = LIST_trapdoor;

	query_GMTS = RGMTS_trapdoor;
	while (query_GMTS != NULL)
	{
		printf("id->%d : ", query_GMTS->id);
		for (i = 0; i < query_GMTS->wordlists_used; i++)
			printf("%d ", query_GMTS->wordlists[i]);
		printf("\n");
		query_GMTS = query_GMTS->next;
	}

	return trapdoor_ret;
}

void Free_Trapdoor(Trapdoor_list *trapdoor_ret)
{
	free(trapdoor_ret->query_GMTS);
	free(trapdoor_ret->query_LIST);
	free(trapdoor_ret->query_RGMTS);
	free(trapdoor_ret);
}
