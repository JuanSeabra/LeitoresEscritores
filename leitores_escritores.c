#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

int numConta, numLeitores, numEscritores, modo;
pthread_t *escritores, *leitores;
pthread_mutex_t saldo_mutex, mutex;
sem_t escritores_exec, leitores_exec;
float saldo;

int contLeitores = 0, contEscritores = 0;

void *escritor(void *arg){
	long tid = (long) arg;

	if (modo == 1 || modo == 2) {
		pthread_mutex_lock(&saldo_mutex);
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mesta editando a SC\n\n", tid);
		sleep(3);
		saldo += 5.5;
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mNovo saldo: %.2f\n\n",tid, saldo );
		printf("\033[1m\033[31mEscritor %d \t \033[1m\033[35mesta saindo da SC\n\n", tid);
		pthread_mutex_unlock(&saldo_mutex);

	}	

	if (modo == 3) {
		pthread_mutex_lock(&mutex);
		contEscritores++;

		if (contEscritores == 1) 
			pthread_mutex_lock(&saldo_mutex);
		pthread_mutex_unlock(&mutex);
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mesta editando a SC\n\n", tid);
		sleep(3);
		saldo += 5.5;
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mNovo saldo: %.2f\n\n",tid, saldo );
		pthread_mutex_lock(&mutex);
		printf("\033[1m\033[31mEscritor %d \t \033[1m\033[35mesta saindo da SC\n\n", tid);


		contEscritores--;
		if (contEscritores == 0)
			pthread_mutex_unlock(&saldo_mutex);

		pthread_mutex_unlock(&mutex);
	}
}


void *leitor(void *arg){
	long tid = (long) arg;
	if (modo == 1 || modo == 3) {

		pthread_mutex_lock(&saldo_mutex);

		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mesta lendo a SC\n\n", tid);
		sleep(5);
		printf("\033[1m\033[31mLeitor: %d \t  \033[1m\033[35mSaldo: %.2f\n\n",tid,  saldo);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mcompletando leitura da SC\n\n", tid);
		pthread_mutex_unlock(&saldo_mutex);
	}

	if (modo == 2) {
		pthread_mutex_lock(&mutex);
		contLeitores++;

		if (contLeitores == 1) 
			pthread_mutex_lock(&saldo_mutex);
		pthread_mutex_unlock(&mutex);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mesta lendo a SC\n\n", tid);
		sleep(5);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mSaldo: %.2f\n\n",  saldo);
		pthread_mutex_lock(&mutex);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mcompletando leitura da SC\n\n", tid);

		contLeitores--;
		if (contLeitores == 0)
			pthread_mutex_unlock(&saldo_mutex);

		pthread_mutex_unlock(&mutex);
	}

}

int main(int argc, const char *argv[]) {
	if(argc != 5){
		printf("%d\n",argc);
		printf("USO: ./leitorescritor <numero de leitores> <numero de escritores> <saldoInicial> <tipo 1-Mesma prioridade 2-Leitor > escritor 3-Escritor > leitor >\n");
		exit(1);
	}
	numLeitores = atoi(argv[1]);
	numEscritores = atoi(argv[2]);
	saldo = atof(argv[3]);
	modo = atoi(argv[4]);

	int i,j,aux;

	pthread_t leitores[numLeitores];
	pthread_t escritores[numEscritores];

	pthread_mutex_init(&saldo_mutex, NULL);
	pthread_mutex_init(&mutex, NULL);

	if(modo == 3){
		for (j = 0; j < numEscritores; j++) {
			//		pthread_create(&leitores[j],0,leitor,(void*)j);
			pthread_create(&escritores[j],0,escritor,(void*)j);
		}
		for (i = 0; i < numLeitores; i++) {
			//		pthread_create(&escritores[i],0,escritor,(void*)i);
			pthread_create(&leitores[i],0,leitor,(void*)i);

		}

		for (j = 0; j < numLeitores; j++) {
			//pthread_join(leitores[j],0);
			pthread_join(escritores[j],0);
		}

		for (i = 0; i < numEscritores; i++) {
			//pthread_join(escritores[i],0);
			pthread_join(leitores[i],0);
		}
	} else {
		for (j = 0; j < numLeitores; j++) {
			pthread_create(&leitores[j],0,leitor,(void*)j);
			//pthread_create(&escritores[j],0,escritor,(void*)j);
		}
		for (i = 0; i < numEscritores; i++) {
			pthread_create(&escritores[i],0,escritor,(void*)i);
			//pthread_create(&leitores[i],0,leitor,(void*)i);

		}

		for (j = 0; j < numLeitores; j++) {
			pthread_join(leitores[j],0);
			//pthread_join(escritores[j],0);
		}

		for (i = 0; i < numEscritores; i++) {
			pthread_join(escritores[i],0);
			//pthread_join(leitores[i],0);
		}
	}
	return 0;
}
