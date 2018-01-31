#ifndef RESULTS_H_INCLUDED
#define RESULTS_H_INCLUDED

void Init_result(struct result *, int );
int insert_to_result(struct result **, struct result **, double , int ,int ,int ,double *);
void print_results(struct result *, int );
void compare_results(struct result *results_1,struct result *results_2, int top_1, int top_2, double *test_all, double *test_hit, double *privacy, double *test_all_o, double *test_hit_o, double *privacy_o);
#endif // RESULTS_H_INCLUDED
