#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define N 3000
#define random(x) (rand()%x)
#define tamBuffer 10

void *Productor(void *);
void *Consumidor(void *);

sem_t mutex, vacias, llenas;
int producto[tamBuffer-1];
int nThreadsP;
int nThreadsC;
int contador=0;
int sTime;
int numerodeProductos = 0;

int producir_elemento()
{
	int elem;
	srand(time(NULL)+contador);
	elem = random(1024+contador);
	return elem;
}


int insertar_elemento(int elemento)
{
	if(tamBuffer>=numerodeProductos>0)
	{
		producto[numerodeProductos] = elemento;
		numerodeProductos++;
		return 1;
	}
	else
		return 0;
}

int quitar_elemento(int *elemento)
{
	if(tamBuffer >= numerodeProductos >0)
	{
		elemento = &producto[numerodeProductos];
		producto[numerodeProductos] = 0;
		numerodeProductos--;
		int e= *elemento;
		return e;
	}
	else
		return 0;
}

void *Productor(void *arg)
{
	int elemento;
	for(;;)
	{
		elemento = producir_elemento();
		sem_wait(&vacias);
		sem_wait(&mutex);
		int resultado = insertar_elemento(elemento);
		if(resultado == 0)
		{
			sem_post(&mutex);
			printf("Se produjo un error");
		}
		else if(resultado == 1)
		{
			sem_post(&mutex);
			sem_post(&llenas);
			printf("El productor produjo %d \t Buffer que queda ahora: %d/10 \n\n", elemento, numerodeProductos);
			if(tamBuffer==numerodeProductos)
			{
				printf("Espera, el buffer esta lleno");
			}
		}
		contador++;
		sleep(N);
		return 0;
	}
}
void *Consumidor(void *arg)
{
	int elemento, resultado;
	for(;;)
	{
		sem_wait(&llenas);
		sem_wait(&mutex);
		resultado = quitar_elemento(&elemento);
		/*if(resultado<= 0)
		{
			sem_post(&mutex);
			printf("El consumidor fallo \n");
		}
		else
		{*/
			printf("El consumidor consumio %d \t Buffer que queda ahora: %d/10 \n\n", producto[numerodeProductos], numerodeProductos);
			producto[numerodeProductos] = 0;
			if(numerodeProductos == 0)
				printf("Espera, el buffer esta vacio\n\n");
			sem_post(&mutex);
			sem_post(&vacias);
			sleep(N);
		//}
	}
	return 0;
}



int main()
{
	sTime = N;
	nThreadsP = 5;
	nThreadsC = 5;
	sem_init(&mutex, 0,1);
	sem_init(&llenas, 0,0);
	sem_init(&vacias, 0, tamBuffer);
	pthread_t producer[nThreadsP -1];
	pthread_t consumer[nThreadsC - 1];
	int i, j;
	for(i = 0; i<nThreadsP-1; i++)
	{
		pthread_create(&producer[i], NULL, Productor, NULL);
		printf("\tProductor %d creado\n\n", i+1);
		if(&producer[i] == NULL)
			return -1;
	}
	for(j=0; j<nThreadsC-1; j++)
	{
		pthread_create(&consumer[j], NULL, Consumidor, NULL);
		printf("\tConsumidor %d creado \n\n", j+1);
		if(&consumer[j] == NULL)
			return -1;	
	}

	for(i = 0; i<nThreadsP-1; i++)
	{
		pthread_join(producer[i], NULL);
		sleep(1);
	}
	return 0;

	Sleep(sTime);
	return 0;
	
}
