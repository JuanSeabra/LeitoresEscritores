/*Equipe:   Gabriel Saldanha
			Nadny Dantas
			Victoria Aires
			Waldomiro Seabra
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

//Dados: numero de leitores, numero de escritores e o modo de operacao:
//sem prioridades, com prioridade para leitura ou com prioridade para escrita
int numLeitores, numEscritores, modo, velocidade;

//controle de velocidade
enum{Iguais=1, LeitorRapido=2, EscritorRapido=3};

//threads de escritores e leitores
pthread_t *escritores, *leitores;

//semaforos do tipo mutex para controle da SC
pthread_mutex_t saldo_mutex, mutex;

//variavel compartilhada onde se deseja fazer alteracao
float saldo;

float alteracoes[100];

//contadores de leitores e escritores
int contLeitores = 0, contEscritores = 0;

//funcao ESCRITOR
void *escritor(void *arg){
	//obtem o id da thread
	long tid = (long) arg;

	//se modo sem prioridade ou com prioridade para leitura
	if (modo == 1 || modo == 2) {
		//solicita acesso a SC
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35msolicitando acesso a SC\n\n", tid);
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mbloqueado! Aguardando...\n\n", tid);
		pthread_mutex_lock(&saldo_mutex);

		//realiza alteracoes
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mdesbloqueado!\n\n", tid);
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mesta editando a SC\n\n", tid);

		saldo += alteracoes[tid];

		//varia velocidades
		if (velocidade == Iguais)
			sleep(3);
		else if (velocidade == LeitorRapido)
			sleep(5);
		else sleep(2);

		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mNovo saldo: %.2f\n\n",tid, saldo );
		printf("\033[1m\033[31mEscritor %d \t \033[1m\033[35mesta saindo da SC\n\n", tid);

		//libera a SC
		pthread_mutex_unlock(&saldo_mutex);
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mfinalizado\n\n", tid);

	}

	//modo com prioridade para escritor
	if (modo == 3) {
		//solicita acesso ao mutex de controle
		pthread_mutex_lock(&mutex);

		//incrementa o contador de escritor
		contEscritores++;

		//ha um escritor querendo acessar a SC: solicita acesso
		if (contEscritores == 1) {
			printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35msolicitando acesso a SC\n\n", tid);
			printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mbloqueado! Aguardando...\n\n", tid);
			pthread_mutex_lock(&saldo_mutex);
		}

		//libera o mutex de controle
		pthread_mutex_unlock(&mutex);

		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mdesbloqueado!\n\n", tid);
		//realiza alteracao na SC
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mesta editando a SC\n\n", tid);

		saldo += alteracoes[tid];
		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mNovo saldo: %.2f\n\n",tid, saldo );

		if (velocidade == Iguais)
			sleep(3);
		else if (velocidade == LeitorRapido)
			sleep(5);
		else sleep(2);

		//solicita acesso ao mutex de controle
		pthread_mutex_lock(&mutex);

		printf("\033[1m\033[31mEscritor %d \t \033[1m\033[35mesta saindo da SC\n\n", tid);

		//decrementa o num de escritores
		contEscritores--;

		//nao ha mais escritores, libera a SC
		if (contEscritores == 0) {
			pthread_mutex_unlock(&saldo_mutex);
		}

		printf("\033[1m\033[31mEscritor: %d \t \033[1m\033[35mfinalizado\n\n", tid);
		//libera o mutex de controle
		pthread_mutex_unlock(&mutex);
	}
}

//Funcao leitor
void *leitor(void *arg){
	//obtem o id da thread
	long tid = (long) arg;

	//modo sem prioridade ou com prioridade para escritor
	if (modo == 1 || modo == 3) {

		//solicita acesso a SC
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35msolicitando acesso a SC\n\n", tid);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mbloqueado! Aguardando...\n\n", tid);
		pthread_mutex_lock(&saldo_mutex);

		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mDesbloqueado!\n\n", tid);
		//Le os dados
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mesta lendo a SC\n\n", tid);

		if (velocidade == Iguais)
			sleep(3);
		else if (velocidade == LeitorRapido)
			sleep(2);
		else sleep(5);

		printf("\033[1m\033[31mLeitor: %d \t  \033[1m\033[35mSaldo: %.2f\n\n", tid,  saldo);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mcompletando leitura da SC\n\n", tid);


		//libera a SC
		pthread_mutex_unlock(&saldo_mutex);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mfinalizado\n\n", tid);

	}

	//modo prioridade leitor
	if (modo == 2) {
		//solicita acesso ao mutex de controle
		pthread_mutex_lock(&mutex);

		//incrementa contador de leitores
		contLeitores++;

		//ha um leitor querendo acesso a SC: solicita acesso
		if (contLeitores == 1) {
			printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35msolicitando acesso a SC\n\n", tid);
			printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mbloqueado! Aguardando...\n\n", tid);
			pthread_mutex_lock(&saldo_mutex);
		}

		//libera mutex de controle
		pthread_mutex_unlock(&mutex);

		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mDesbloqueado!\n\n", tid);
		//le os dados
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mesta lendo a SC\n\n", tid);

		if (velocidade == Iguais)
			sleep(3);
		else if (velocidade == LeitorRapido)
			sleep(2);
		else sleep(5);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mSaldo: %.2f\n\n",  tid, saldo);


		//solicita acesso ao mutex de controle
		pthread_mutex_lock(&mutex);
		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mcompletando leitura da SC\n\n", tid);

		//decrementa o contador de leitores
		contLeitores--;


		//nao ha mais leitores: libera a SC
		if (contLeitores == 0) {
			pthread_mutex_unlock(&saldo_mutex);
		}

		printf("\033[1m\033[31mLeitor: %d \t \033[1m\033[35mfinalizado\n\n", tid);
		//libera o mutex de controle
		pthread_mutex_unlock(&mutex);
	}

}

int main(int argc, const char *argv[]) {

	//verifica se o num de argumentos passados esta correto
	if(argc < 6){
		printf("%d\n",argc);
		printf("USO: ./leitorescritor <numero de leitores> <numero de escritores> <saldoInicial>");
		printf(" <tipo 1-Mesma prioridade 2-Prioridade leitor 3-Prioridade escritor> <velocidade 1- mesma velocidade 2-leitor mais rapido 3-escritor mais rapido> <alteracoes dos escritores>\n");
		exit(1);
	}

	//atribui os valores para as variaveis
	numLeitores = atoi(argv[1]);
	numEscritores = atoi(argv[2]);
	saldo = atof(argv[3]);
	modo = atoi(argv[4]);
	velocidade = atoi(argv[5]);

	int i,j,aux;

	for (i = 0; i < numEscritores; i++) {
		alteracoes[i] = atof(argv[i+6]);
	}

	//cria threads de leitores e escritores
	pthread_t leitores[numLeitores];
	pthread_t escritores[numEscritores];

	//inicia os semaforos mutex
	pthread_mutex_init(&saldo_mutex, NULL);
	pthread_mutex_init(&mutex, NULL);

	//modo com prioridade para escritor
	if(modo == 3){

		//cria threads de escritores - elas comecam a executar
		for (j = 0; j < numEscritores; j++) {
			pthread_create(&escritores[j],0,escritor,(void*)j);
		}

		//cria threads de leitores - elas comecam a executar
		for (i = 0; i < numLeitores; i++) {
			pthread_create(&leitores[i],0,leitor,(void*)i);
		}

		//espera pelo termino das threads de leitores
		for (j = 0; j < numLeitores; j++) {
			pthread_join(leitores[j],0);
		}

		//espera pelo termino das threads de escritores
		for (i = 0; i < numEscritores; i++) {
			pthread_join(escritores[i],0);
		}
	}
	//modo sem prioridade ou com prioridade para leitor
	else {
		//cria threads de leitores - elas comecam a executar
		for (j = 0; j < numLeitores; j++) {
			pthread_create(&leitores[j],0,leitor,(void*)j);
		}

		//cria threads de escritores - elas comecam a executar
		for (i = 0; i < numEscritores; i++) {
			pthread_create(&escritores[i],0,escritor,(void*)i);
		}

		//espera pelo termino das threads de leitores
		for (j = 0; j < numLeitores; j++) {
			pthread_join(leitores[j],0);
		}

		//espera pelo termino das threads de escritores
		for (i = 0; i < numEscritores; i++) {
			pthread_join(escritores[i],0);
		}
	}
	return 0;
}
