#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

//Objective Find  program whose output matche a fibonacci over the range 0 <= x <= 100
//function set +,-,*,% 
//terminal set x end constants 
//fitness proportionate 
//initial pop rramped haf-and-half
//parameters population size 1000, 90% sub tree crossover,5% reproduction,5% sub tree size limits
//terminationn individual with gfitness better than 0.1 found
#define population 100

#define crossover  0.9 // = 0.9
#define reproduction  0.95 //0.05
#define mutação  1 // = 0.05

#define MAX_D 3
// se o numerod e torneio for maior que o numero da populacao vai dar loop infinito!!!

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
    FUNC_SET,
    CONST
}SET;

typedef union FUNCTION {
    double num; //vai ser num 
    char func;// vai ser +,-,%
    //char vari;// vai ser o x
}FUNCTION;

typedef struct 
{   
    FUNCTION func;
    TYPE type;
}tupla_retorno;

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
    NO           *pai[population];
    char                *func_set;
    char *term_set;
    unsigned max_d;
    char                   method;
    unsigned tam_populacao;
    unsigned num_de_individuos_torneio;
    double crossoverPorcen;
    double reproductionPorcen;
    double mutationPorcen;
    //struct         GP   (*init)();
    void (*fitness)(struct GP *);
    void (*initialisation_population)(struct GP *);
    void (*get_exp)(NO* ,char *);
    double (*valor_arvore)(NO* ,double );
    void (*runGP)(struct GP *);
    void (*Crossover)(struct GP *);

    
}GP;

NO *
initialize_individual(char *func_set, char *term_set,unsigned max_d, char method);

GP
init(char *func_set, char *term_set,unsigned max_d, char method, unsigned tam_populacao, 
    unsigned num_de_individuos_torneio, double crossoverPorcen, double reproductionPorcen, 
    double mutationPorcen)
    {
    GP p;
    p.func_set = func_set;
    p.term_set = term_set;
    p.max_d    =    max_d;
    p.method   =   method;

    p.tam_populacao = tam_populacao;
    p.num_de_individuos_torneio = num_de_individuos_torneio;
    p.crossoverPorcen = crossoverPorcen;
    p.reproductionPorcen = reproductionPorcen;
    p.mutationPorcen = mutationPorcen;


    //p.fitness  = fitness;
    return p;
}

void 
initialisation_population(GP *gp){

    for(unsigned i = 0; i < population; i++){
        gp->pai[i] = initialize_individual(gp->func_set, gp->term_set, gp->max_d, gp->method);
    }

}

tupla_retorno  
choose_random_element_term(char *set,SET type){
    int p = (int)(rand() % strlen(set));


    switch (set[p])
    {
    case 'c':{
        return (tupla_retorno){.func.num = (double)rand(),.type = NUM};
        break;
    }
    
    default:
        if(type == TERM_SET){
            if(set[p] == 'c')
                return (tupla_retorno){{.func = (double)rand()},.type = NUM};
            else{
                return (tupla_retorno){{.func = set[p]},.type = VARI};
            }
        }
        else if(type == FUNC_SET)
            return (tupla_retorno){{.func = set[p]},.type = FUNC};
        break;
    }
}

NO*
initialize_individual(char *func_set,char *term_set,unsigned max_d, char method){
    double p = (double)rand() /RAND_MAX;
    NO *temp = malloc(sizeof(NO));
    temp->dir = NULL;
    temp->esq = NULL;
    if (temp == NULL) {
        printf("Erro ao alocar memória\n");
        return NULL;
    }

    if (max_d == 0 || (method == 'g' && 
                      p < (strlen(term_set)/
                      (strlen(term_set) + strlen(func_set)) ) 
                     ) 
        ){
            tupla_retorno ret = choose_random_element_term(term_set,TERM_SET);
            temp->tipo = ret.type;
            if(ret.type == NUM)
                temp->function.num = ret.func.num;
            else
                temp->function.func = ret.func.func;
            return temp;
    }else{
        tupla_retorno ret = choose_random_element_term(func_set,FUNC_SET);
    
        temp->tipo = FUNC;
        temp->function.func = ret.func.func;
        
        //for(unsigned i = 1; i <= 2; i++){// caso a funcaoes precisasse de mais argumetos teria que apricar isso junto com um vetor dinamico de filhos
            temp->esq = initialize_individual(func_set,term_set,max_d - 1, method);
            temp->dir = initialize_individual(func_set,term_set,max_d - 1, method);
        //}
        return temp;

    }

}

unsigned
tournament_selection(GP *gp,NO* tira){
    NO* individuos[gp->num_de_individuos_torneio];
    int melhorIndividuo  = -1;
    double melhorFitness = -1;
    for(unsigned i =0;i < gp->num_de_individuos_torneio;i++)
        individuos[i] = NULL;
    
    for(unsigned i = 0;i < gp->num_de_individuos_torneio; i++){
        unsigned p = (int)rand() % gp->tam_populacao;
        unsigned n = 0;
        while(n < i && individuos[n] != NULL && gp->pai[p] != individuos[n] && tira != gp->pai[p]){
            n++;
        }
        if(gp->pai[p] != individuos[n] && tira != gp->pai[p]){
            individuos[i] = gp->pai[p];
            if(gp->pai[p]->erro < melhorFitness || melhorFitness == -1){
                melhorIndividuo = p;
                melhorFitness   = gp->pai[p]->erro;
            }

        }else
            i--;
    }

    if(melhorIndividuo >= 0)
        return (unsigned)melhorIndividuo;
    else{
        printf("\nerr tournament_selection");
        exit(1);
    }
}

void
desalocaArvore(NO* pt){
    if(pt->esq != NULL)
        desalocaArvore(pt->esq);
    if(pt->dir != NULL)
        desalocaArvore(pt->dir);
    free(pt);
}

NO* 
copyCodigo(NO* pt){
    //visita()
    NO* temp = malloc(sizeof(NO));
    temp->dir = NULL;
    temp->esq = NULL;
    temp->tipo = pt->tipo;
    if(pt->tipo == NUM)
        temp->function.num  = pt->function.num;
    else{
        temp->function.func = pt->function.func;
    }
    if(pt->esq != NULL)
        temp->esq = copyCodigo(pt->esq);
    if(pt->dir != NULL)
        temp->dir = copyCodigo(pt->dir);
    return temp;
}

//escolhe o no
NO** 
Escolhe_NO_Ale(NO* pt){

    int p = (int)rand() % 1;

    //se for 1 vai ser o no escolhido para fazer o  Crossover se nao vai para dir ou esq 
    //dar uma olhada dpis nesse pt == NULL
    if(p || pt == NULL || (pt->dir == NULL && pt->esq == NULL)){
        p = (int)rand() % 1;
        //dar uma olhada dpis nesse pt == NULL
        if(pt == NULL || ((p || pt->dir == NULL) && pt->esq != NULL)){
            //nao sei se pode isso ne nao pode fazer uma tupla para retorno
            return &pt->esq;
        }else{
            //nao sei se pode isso ne nao pode fazer uma tupla para retorno
            return &pt->dir;
        }
    }else{
        p = (int)rand() % 1;
        //vai para dir
        if((p || pt->dir == NULL) && pt->esq != NULL){
            Escolhe_NO_Ale(pt->esq);
        }else{
            //vai para esq
            Escolhe_NO_Ale(pt->dir);
        }
    }

}

NO* 
Crossover(GP *gp){
    unsigned individo1 = tournament_selection(gp,NULL);
    unsigned individo2 = tournament_selection(gp,gp->pai[individo1]);
    
    NO* individo1_pt_copy = copyCodigo(gp->pai[individo1]);
    //NO* individo2_pt_copy = copyCodigo(gp->pai[individo2]);


    NO** individo1_pt = Escolhe_NO_Ale(individo1_pt_copy);

    if(*individo1_pt != NULL)
        desalocaArvore(*individo1_pt);

    NO** Escolhe_NO_Ale1 = Escolhe_NO_Ale(gp->pai[individo2]);

    if(*Escolhe_NO_Ale1 != NULL)
        *individo1_pt = copyCodigo(*Escolhe_NO_Ale1);
    else{
        *individo1_pt = NULL;
    }

    
    return individo1_pt_copy;
}

void 
novaPopulacao(GP *gp){
    NO* novaPopu[gp->tam_populacao];
    for(unsigned i = 0; i < gp->tam_populacao; i++){
        double p = (double)rand() /RAND_MAX;

        if(p <= gp->crossoverPorcen){
            novaPopu[i] = Crossover(gp);
        }else if(p <= gp->mutationPorcen){
            unsigned individo1 = tournament_selection(gp,NULL);
            NO* individo1_pt_copy = copyCodigo(gp->pai[individo1]);

            NO** individo1_pt = Escolhe_NO_Ale(individo1_pt_copy);
            if(*individo1_pt != NULL)
                desalocaArvore(*individo1_pt);
            *individo1_pt = initialize_individual(gp->func_set,gp->term_set,5/*tem que ver um limite*/,gp->method);

            novaPopu[i] = individo1_pt_copy;
        }else{
            //Reprodução
            unsigned individo1 = tournament_selection(gp,NULL);
            NO* individo1_pt_copy = copyCodigo(gp->pai[individo1]);
            novaPopu[i] = individo1_pt_copy;
        }
    }

    for(unsigned i = 0; i < gp->tam_populacao; i++){
        if(gp->pai[i] != NULL)
            desalocaArvore(gp->pai[i]);
        gp->pai[i] = novaPopu[i];
    }
}

void 
get_exp(NO* pt,char *explecao){

    if(pt->esq != NULL)
        get_exp(pt->esq,explecao);

    if(pt->tipo == NUM){
        char buffer[100];
        sprintf(buffer,"%.5f",pt->function.num);
        strcat(explecao,buffer);
    }else{
        strcat(explecao,&pt->function.func);
    }


    if(pt->dir != NULL)
        get_exp(pt->dir,explecao);
}

double 
valor_arvore(NO* pt,double x,double y){
    if(pt->tipo == NUM)
        return pt->function.num;
    
    if(pt->tipo == VARI)
        switch (pt->function.func){
            case 'x':    
                return x;
                break;
            case 'y':
                return y;
                break;
            default: 
                printf("func invalida\n");
                exit(1);
                break;
        }
    if(pt->tipo == FUNC){
        if(pt->function.func == '+'){
            return valor_arvore(pt->dir,x,y) + valor_arvore(pt->esq,x,y);
        }
        switch (pt->function.func){
            case '+':{
                return valor_arvore(pt->dir,x,y) + valor_arvore(pt->esq,x,y);
                break;
                }
            case '-':{
                return valor_arvore(pt->dir,x,y) - valor_arvore(pt->esq,x,y);
                break;
                }
            case '*':{
                return valor_arvore(pt->dir,x,y) * valor_arvore(pt->esq,x,y);
                break;
                }
            case '%': {
                double tem1 = valor_arvore(pt->dir,x,y);
                if(tem1 == 0)
                    return 1;
                return valor_arvore(pt->esq,x,y) / tem1;
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

void
fitness(GP *gp){
    unsigned v[100] = {1,1};
    for(unsigned i = 2;i < 100;i++){
        v[i] = v[i-2] + v[i-1];
    }
    float erro = 0;
    for(unsigned i = 0;i < population;i++){
        for(unsigned n = 0;n <100; n++){
            for(unsigned t = 0;t <100; t++){
                erro += fabs(valor_arvore(gp->pai[i],n,t) - v[n]); 
            }
        }
        gp->pai[i]->erro = erro;
    }
    
}

void
runGP(){

}

int 
main(){
    srand(time(NULL));


    char func_set[] = {'+','-','*','%','\0'};
    char term_set[] = {'c','x','y','\0'};
    char method     = 'g';
    char explecao[1000000];
    explecao[0] = '\0';
    GP gp = init(func_set, term_set, MAX_D, method, 
    population, 10, crossover, reproduction, mutação);




    initialisation_population(&gp);
    explecao[0] = '\0';
    
    get_exp(gp.pai[0],explecao);
    printf("explecao : %s \n",explecao);


   
    printf("valor com 2 :%f \n",valor_arvore(gp.pai[0],1,2));

    fitness(&gp);

    for(unsigned i =0; i < population; i++){
        printf("erro(%d) = %f \n",i,gp.pai[i]->erro);
    }

    for(unsigned i = 0; i < 10;i++){
        printf("\n-------------------\n");
        printf("Populacao(%d)",i);
        printf("\n-------------------\n");

        novaPopulacao(&gp);
        fitness(&gp);
        for(unsigned n =0; n < population; n++){
            printf("erro(%d) = %f \n",n,gp.pai[n]->erro);
    }
    }

    for(unsigned i = 0; i < population; i++){
        if(gp.pai[i] != NULL)
            desalocaArvore(gp.pai[i]);
    }
    unsigned v[100] = {1,1};
    for(unsigned i = 2;i < 100;i++){
        v[i] = v[i-2] + v[i-1];
    }

    for(unsigned i = 1;i < 100;i++){
        printf("fib(%d): %f\n ",i,valor_arvore(gp.pai[0],v[i-1],v[i]));
    }



    printf("\nfinalizo com sucesso\n");


    return 0;
}

//implementar Crossover e ajutar a inicilizaca para ser metade do tipo