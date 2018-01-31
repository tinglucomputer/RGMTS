#ifndef WRAPPER_FUNCTION_H_INCLUDED
#define WRAPPER_FUNCTION_H_INCLUDED

void * Malloc(int);
void * Calloc(int, int);

FILE *Fopen(char *, char *);

struct Node * Create_node(double *, double *, double *, int , struct Node *, struct Node *);

void Init_node(struct Node*, double*, double *, double *, int , struct Node *, struct Node *);

double calculate(double *, int , int ,int , int , double ***);

void save_node(FILE *, int , int , double *, double *, double *);

int count_num(struct Node *);

int count_need_node(int);
void time_start(char*, int );
double time_end(char *, int);

int Fgmrs_num(struct fgmrsii *head);
void Error_Print(char *msg, int code);

#endif // WRAPPER_FUNCTION_H_INCLUDED
