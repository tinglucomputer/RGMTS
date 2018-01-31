#include "head.h"

void
Init_result(struct result *unuse_results, int topnum)
{
    int i;
    unuse_results[0].up = NULL;
    if(topnum==1)  unuse_results[0].next = NULL;
    for(i=1;i<topnum;i++){
        unuse_results[i].next=NULL;
        unuse_results[i].up = &unuse_results[i-1];
        unuse_results[i-1].next = &unuse_results[i];
        unuse_results[i].used = 0;
    }
}

static struct result *
get_a_result_node(struct result **unuse_results,int document_id,double sum)
{
    struct result *temp;
    temp = (*unuse_results);
    if(temp == NULL) return NULL;
    (*unuse_results)=temp->next;
     temp->value = sum;
     temp->documents[0]=document_id;
     temp->used = 1;
     temp->next=NULL;
     temp->up =NULL;
     return temp;
}



int
insert_to_result(struct result **results, struct result **unuse_results, double sum, int top, int topnum, int document_id ,double *min_tf)
{
    struct result *temp,*mtemp;
    int i=0;
    if(top==0){
        (*min_tf) = sum;
        mtemp = get_a_result_node(unuse_results, document_id, sum);
        *results = mtemp;
        return top+1;
    }else if(top == topnum){
        temp = (*results);
        while(i<top){
            if(fabs(temp->value - sum)<0.0000001){
                if(temp->used==50) return top;
                temp->documents[temp->used]=document_id;
                temp->used++;
                return top;
            }
            i++;
            if(i<top) temp=temp->next;
            else break;
        }
        if(temp->up!=NULL) temp->up->next=NULL;
        temp->next=NULL;
        (*unuse_results) = temp;
        top--;
    }else{
        temp = (*results);
        while(temp!=NULL){
            if(fabs(temp->value - sum)<0.0000001){
                if(temp->used==50) return top;
                temp->documents[temp->used]=document_id;
                temp->used++;
                return top;
            }
            temp=temp->next;
        }
    }
    temp = (*results);
    mtemp = get_a_result_node(unuse_results, document_id, sum);
    i=0;
    while(i<top){
        if(temp==NULL){
            printf("error in: insert_to_result ->find value\n");
            exit(5);
        }
        if(temp->value<sum){
            mtemp->next = temp;
            mtemp->up = temp->up;
            temp->up = mtemp;
            if(i==0) (*results) = mtemp;
            else mtemp->up->next=mtemp;
            return top+1;
        }
        i++;
        if(i<top) temp=temp->next;
        else break;
    }
    mtemp->up = temp;
    temp->next = mtemp;
    (*min_tf) = sum;
    return top+1;
}

void
print_results(struct result *results, int top)
{
    struct result *temp;
    int i;
    temp = results;
    printf("[");
    while(temp!=NULL){
        printf("[%.13lf ",temp->value);
        for(i=0;i<temp->used;i++){
            printf("%d ",temp->documents[i]);
        }
        printf("] ");
        temp=temp->next;
    }
    printf("]\n");
}
void
compare_results(struct result *results_1,struct result *results_2, int top_1, int top_2, double *test_all, double *test_hit, double *privacy, double *test_all_o, double *test_hit_o, double *privacy_o)
{
    struct result *temp1,*temp2;
    int i,j,sum=0,all=0,rank_1=0,rank_2,d_id_1,d_id_2;
    double rank_all=0.0;
    temp2 = results_1;
    while(temp2!=NULL){
        for(i=0;i<temp2->used;i++){
            temp1 = results_2;
            rank_1++;
            rank_2=0;
            while(temp1!=NULL){
                for(j=0;j<temp1->used;j++){
                    rank_2++;
                    if(temp1->documents[j]==temp2->documents[i]){
                        rank_all+=fabs(rank_1-rank_2);
                        sum++;
                        break;
                    }
                }
                if(j!=temp1->used) break;
                temp1=temp1->next;
            }
            all++;
        }
        temp2=temp2->next;
    }
    rank_all/=((double)(top_1*top_1));
    (*privacy)+=rank_all;
    (*test_hit)+=sum;
    (*test_all)+=all;

    extern int **documents_temp;
    sum=0;
    all=0;
    rank_1=0;
    rank_all=0.0;
    temp2 = results_1;
    while(temp2!=NULL){
        for(i=0;i<temp2->used;i++){
            d_id_1 = documents_temp[temp2->documents[i]][0]-1;
            temp1 = results_2;
            rank_1++;
            rank_2=0;
            while(temp1!=NULL){
                for(j=0;j<temp1->used;j++){
                    d_id_2=documents_temp[temp1->documents[j]][0]-1;
                    rank_2++;
                    if(d_id_2==d_id_1){
                        rank_all+=fabs(rank_1-rank_2);
                        sum++;
                        break;
                    }
                }
                if(j!=temp1->used) break;
                temp1=temp1->next;
            }
            all++;
        }
        temp2=temp2->next;
    }
    rank_all/=((double)(top_1*top_1));
    (*privacy_o)+=rank_all;
    (*test_hit_o)+=sum;
    (*test_all_o)+=all;

}
