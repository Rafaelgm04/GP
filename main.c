#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Objective Find  program whose output matche a fibonacci over the range 0 <= x <= 100
//function set +,-,% 
//terminal set x end constants 
//fitness proportionate 
//initial pop rramped haf-and-half
//parameters population size 1000, 90% sub tree crossover,5% reproduction,5% sub tree size limits
//terminationn individual with gfitness better than 0.1 found
#define population 1000

#define crossover  0.9
#define reproduction  0.05
#define mutação  0.05

#define tam_list 1000

typedef enum {
    CONST,
    FUNC
} TYPE;

typedef union FUNCTION {
    float Const;
    char func;
}FUNCTION;

typedef struct NO 
{
   TYPE tipo;
   FUNCTION function;
   struct NO* dir;
   struct NO* esq;
   char funcao[tam_list];
   double erro;
   
    
}NO;



typedef struct GP
{
    NO* pai[population];
    struct GP (*init)();
    void      (*fitness)(GP *);
    



}GP;

void 
Crossover(){

}

void 
get_exp(NO* pt,char* expl){

    if(pt->esq != NULL)
        get_exp(pt->esq,expl);

    if(pt->tipo == FUNC)
        get_exp(expl,pt->function.func);
    else{
        char buffer[10];
        sprintf(buffer,"%f",pt->function.Const);
        strcat(expl,buffer);
    }
    if(pt->dir != NULL)
        get_exp(pt->dir,expl);
}

double 
exec(char* expl,unsigned n){
    unsigned tam  = strlen(expl);

    
    

}

double
fitness(GP *gp){
    unsigned v[100] = {1,1};
    for(unsigned i = 2;i < 100;i++){
        v[i] = v[i-2] + v[i-1];
    }
    float erro = 0;
    for(unsigned i =0;i < 100;i++){
        for(unsigned n =0;i <100;i++){
            erro += abs(exec(gp->pai[i]->funcao,n) - v[n]); 
        }
        gp->pai[i]->erro = erro;
    }
}


int 
main(){

    

    return 0;
}