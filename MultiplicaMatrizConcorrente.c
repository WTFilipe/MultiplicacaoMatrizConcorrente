/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include<pthread.h>
#include "timer.h"

int *matA; //Matriz A a ser multiplicada
int *matB; //Matriz B a ser multiplicada
int *matC; //Matriz C resultante da multiplicação sequencial de A e B 
int *matD; //Matriz D resultante da multiplicação concorrente de A e B
int nthreads; //numero de threads
int dim; //dimensao da matriz de entrada
double inicioSeq; // Armazena momento de início do cálculo sequencial
double fimSeq; // Armazena momento de término do cálculo sequencial
double inicioConc; // Armazena momento de início do cálculo concorrente
double fimConc; // Armazena momento de término do cálculo concorrente

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int dim; //dimensao das estruturas de entrada
} tArgs;

//funcao que as threads executarao
void * tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;
  
   for(int i=args->id; i<args->dim; i+=nthreads) {
   	for(int j = 0; j < dim; j++){
   		for(int k = 0; k < dim; k++){
   			matD[i*dim+j] += matA[k*dim+j] * matB[i*dim + k]; //Realiza o cálculo do valor de cada elemento da matriz resultante
   		}
   	}
   }
   pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   
   srand(time(NULL)); //Permite calcular números aleatóriso
   
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
   }
   dim = atoi(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > dim) nthreads=dim;

   //alocacao de memoria para as estruturas de dados
   matA = (int *) malloc(sizeof(int) * dim * dim);
   if (matA == NULL) {printf("ERRO--malloc\n"); return 2;}
   matB = (int *) malloc(sizeof(int) * dim * dim);
   if (matB == NULL) {printf("ERRO--malloc\n"); return 2;}
   matC = (int *) malloc(sizeof(int) * dim * dim);
   if (matC == NULL) {printf("ERRO--malloc\n"); return 2;}
   matD = (int *) malloc(sizeof(int) * dim * dim);
   if (matD == NULL) {printf("ERRO--malloc\n"); return 2;}

   //inicializacao das estruturas de dados de entrada e saida
   for(int i=0; i<dim; i++) {
      for(int j=0; j<dim; j++){
         matA[i*dim+j] = 1; 
         matB[i*dim+j] = 1;
         matC[i*dim+j] = 0;
         matD[i*dim+j] = 0;    //equivalente mat[i][j]
         }
   }
   
   //Iniciando Calculo Sequencial
   
   GET_TIME(inicioSeq);
   for(int i = 0; i < dim; i++) {
   	for(int j = 0; j < dim; j++){
   		for(int k = 0; k < dim; k++){
   			matC[i*dim+j] += matA[k*dim+j] * matB[i*dim + k];
   		}
   	}
   }
   GET_TIME(fimSeq);
   
   
   //Iniciando Calculo Concorrente
   
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
   
   //criacao das threads
   GET_TIME(inicioConc);
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = dim;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   }
   
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   GET_TIME(fimConc);

   
   
   //Checando corretude da versão concorrente
   for(int i = 0; i < dim*dim; i++){
   	if(matC[i] != matD[i]) {
   		printf("Calculo concorrente INCORRETO\n");
   		return 2;
   	}
   }  
   
   printf("Calculo concorrente CORRETO\n");
   printf("Tempo do cálculo sequencial: %f\n", fimSeq - inicioSeq);
   printf("Tempo do cálculo concorrente: %f\n", fimConc - inicioConc);

   //liberacao da memoria
   free(matA);
   free(matB);
   free(matC);
   free(matD);
   free(tid);
   free(args);
   //printf("Tempo finalizacao:%lf\n", delta);

   return 0;
}
