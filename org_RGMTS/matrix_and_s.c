#include "head.h"

int *
create_s(char *s_file_name, int total)
{
    int *temp=NULL, i;
    FILE *f;
    temp = (int *)Malloc(total*sizeof(int));

    if(!access(s_file_name,0)){
        f=fopen(s_file_name,"r");
        for (i = 0; i < total; i++)
            fscanf(f, "%d", &(temp[i]));
    }else{
        f=fopen(s_file_name,"w");
        srand( (unsigned int)time(0) );
        for (i = 0; i < total; i++){
            temp[i] = rand()%2;
            fprintf(f,"%d ",temp[i]);
        }
    }
    fclose(f);
    return temp;
}

double ***
get_matrix(char *s_file_name, int dimension, int total)
{
    int i,j,t,n,num;
    char name[50];
    FILE *f;
    double ***re=(double ***)Malloc(4*sizeof(double **)),**temp;
    for(n=0,t=dimension*dimension;n<4;n++){
        sprintf(name,"%s_%d",s_file_name,n);
        f=Fopen(name,"r");
        temp=(double **)Malloc(total*sizeof(double *));
        for(i=0;i<total;i++){
            temp[i]=(double *)Malloc(sizeof(double)*t);
            //printf("1fp= %ld\n",ftell(f));
            fscanf(f, "%d", &num);
            if(num!=i){
                printf("error in: function -> get_matrix_%d_%d\n",dimension,total);
                exit(3);
            }
            for(j=0;j<t;j++) fscanf(f, "%lf", &(temp[i][j]));
            fscanf(f,"\n");
        }
        re[n]=temp;
        fclose(f);

    }
    return re;
}
