#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

	sem_t menSemaphore; 
	sem_t womanSemaphore;
	sem_t empty;
	sem_t full; 

	int h_count = 0; 
	int m_count = 0; 

	void *men(void *id);
	void *woman(void *id);
	void bathroom();

int main(int argc, char *argv[]){
	
	if ( argc < 3 ) {
                printf("Numero de parametros incorrecto. Indicar el numero de datos a producir.\n") ;
                exit(1);              
        }
	
	int MEN = atoi(argv[1]);
	int WOMAN = atoi(argv[2]);

	int i;
	sem_init(&empty,0,3);
	sem_init(&full,0,0);
	sem_init(&menSemaphore,0,1);
	sem_init(&womanSemaphore,0,1);
	
	int hid[MEN],mid[WOMAN];
	
	pthread_t hombre[MEN],mujer[WOMAN];

	for (i = 0; i < MEN; i++) {
		pthread_create(&hombre[i],NULL,men,(void *) &hid[i]);
		}
	for (i = 0; i < WOMAN; i++)
		pthread_create(&mujer[i],NULL,woman,(void *) &mid[i]);

	for(i=0;i<MEN;i++)
		pthread_join(hombre[i],NULL);
	for(i=0;i<WOMAN;i++)
		pthread_join(mujer[i],NULL);
	return 0;
}

void bathroom(){
	printf("Usando el baño: %d Hombres || %d Mujeres\n",h_count,m_count);
}

void * men(void *id){
	sem_wait(&empty);
	sem_wait(&menSemaphore);
	h_count++;
	if(h_count == 1){
		sem_wait(&womanSemaphore);
	}
	printf("Hombre entra al baño\n");
	sem_post(&full);
	sem_post(&menSemaphore);
	bathroom();
	sem_wait(&menSemaphore);
	sem_post(&empty);
	printf("Hombre sale del baño\n");
	h_count--;
	if (h_count == 0){
		sem_post(&womanSemaphore);
	}
	sem_post(&menSemaphore);
	sem_wait(&full);
	pthread_exit(NULL);
}

void * woman(void *id){
	sem_wait(&empty);
	sem_wait(&womanSemaphore);
	m_count++;
	if(m_count == 1){
		sem_wait(&menSemaphore);
	}
	printf("Mujer entra al baño\n");
	sem_post(&full);
	sem_post(&womanSemaphore);
	bathroom();
	sem_wait(&womanSemaphore);
	sem_post(&empty);
	printf("Mujer sale del baño\n");
	m_count--;	
	if (m_count == 0){
		sem_post(&menSemaphore);
	}
	sem_post(&womanSemaphore);
	sem_wait(&full);
	pthread_exit(NULL);
}
