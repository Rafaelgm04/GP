#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Objective Find  program whose output matche a fibonacci over the range 0 <= x <= 100
//function set +,-,*,% 
//terminal set x end constants 
//fitness proportionate 
//initial pop rramped haf-and-half
//parameters population size 1000, 90% sub tree crossover,5% reproduction,5% sub tree size limits
//terminationn individual with gfitness better than 0.1 found
#define population 1000

#define crossover  0.9
#define reproduction  0.05
#define mutação  0.05
#define MAX_D 15
#define tam_list 1000

typedef enum{
    F,
    V
} Bool;

typedef enum {
    VARI,
    NUM,
    FUNC
} TYPE;

typedef enum{
    TERM_SET,
    FUNC_SET
}SET;

typedef union FUNCTION {
    double num; //vai ser num 
    char func;// vai ser +,-,%
    //char vari;// vai ser o x
}FUNCTION;

typedef struct NO 
{
   TYPE tipo;
   FUNCTION function;
   struct NO* dir;
   struct NO* esq;//talvez usar um vetor de ponteiros depois para permitir mais paramentros para as funcaoes 

   double erro;
   
   //Bool ecostura;
   //Bool dcostura;
    
}NO;



typedef struct GP
{
    NO*   pai[population];
    char *func_set;
    char *term_set;unsigned max_d;
    char method;
    struct GP   (*init)();
    void (*fitness)(GP *);
    
}GP;

void
init(){

}

void 
initialisation_population(GP *gp){

    for(unsigned i = 0; i < population; i++){
        gp->pai[i] = initialize_individual(gp->func_set, gp->term_set, gp->max_d, gp->method);
    }

}

typedef struct 
{   
    FUNCTION func;
    TYPE type;
}tupla_retorno;

tupla_retorno  
choose_random_element_term(char *set,SET type){
    char p = set[rand() % strlen(set)]; 

    switch (set[p])
    {
    case 'c':{
        return (tupla_retorno){(double)rand(),NUM};
        break;
    }
    
    default:
        if(type == TERM_SET)
            return (tupla_retorno){{.func = set[p]},VARI};
        else if(type == FUNC_SET)
            return (tupla_retorno){{.func = set[p]},FUNC};
        break;
    }
}
NO*
initialize_individual(char *func_set,char *term_set,unsigned max_d, char method){
    double p = (double)rand() /RAND_MAX;
    NO *temp = malloc(sizeof(NO));

    if (max_d = 0 || (method == 'g' && 
                      p < (strlen(term_set)/
                      (strlen(term_set) + strlen(func_set)) ) 
                     ) 
        ){
            tupla_retorno ret = choose_random_element_term(term_set,TERM_SET);
            temp->tipo = ret.type;
            if(ret.type == 'c')
                temp->function.num = ret.func.num;
            else
                temp->function.func = ret.func.func;
            return temp;
    }else{
        tupla_retorno ret = choose_random_element_term(func_set,FUNC_SET);
    
        temp->tipo = FUNC;
        temp->function.func = ret.func.func;
        NO* arg[1];
        //for(unsigned i = 1; i <= 2; i++){// caso a funcaoes precisasse de mais argumetos teria que apricar isso junto com um vetor dinamico de filhos
            temp->esq = initialize_individual(func_set,term_set,max_d - 1, method);
            temp->dir = initialize_individual(func_set,term_set,max_d - 1, method);
        //}
        return temp;

    }

}

void 
Crossover(){

}



void 
get_exp(NO* pt,char *explecao){

    if(pt->esq == NULL)
        get_exp(pt->esq,explecao);

    if(pt->tipo == NUM){
        char buffer[100];
        buffer[0] = '\0';
        strcat(explecao,sprintf(buffer,"&.99f",pt->function.num));
    }else{
        strcat(explecao,pt->function.func);
    }


    if(pt->dir == NULL)
        get_exp(pt->dir,explecao);
}


double valor_arvore(NO* pt,double x){
    if(pt->tipo == NUM)
        return pt->function.num;
    
    if(pt->tipo == VARI)
        return x;


    if(pt->tipo == FUNC){
        if(pt->function.func == '+'){
            return valor(pt->dir,x) + valor(pt->esq,x);
        }
        switch (pt->function.func){
            case '+':
                return valor(pt->dir,x) + valor(pt->esq,x);
                break;
            case '-':
                return valor(pt->dir,x) - valor(pt->esq,x);
                break;
            case '*':
                return valor(pt->dir,x) * valor(pt->esq,x);
                break;
            case '%': {
                double tem1 = valor(pt->dir,x);
                if(tem1 == 0)
                    return 1;
                return valor(pt->esq,x) / tem1;
                break;
            }
            default: {
                printf("func invalida\n");
                exit(1);
                break;
            }
        }
    }
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
            erro += abs(valor_arvore(gp->pai[i],n) - v[n]); 
        }
        gp->pai[i]->erro = erro;
    }
}

void
runGP(){

}

int 
main(){

    

    return 0;
}