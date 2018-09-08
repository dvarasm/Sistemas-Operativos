#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct bano {
    int    hombres;
    int    mujeres;
    int    total_hombres;
    int    total_mujeres;
    pthread_mutex_t mutex;
    pthread_cond_t  lleno;
}bano_u;

bano_u bano_unisex = {0,0,0,0,PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER};

static void usar_bano(bano_u *bano, int ms) {
	printf("Usando el baño: %d Hombres || %d Mujeres\n",bano->hombres, bano->mujeres);
	(void) usleep(1000 * ms);
}

static void* hombre(void *datos){
	bano_u *bano = (bano_u *) datos;
        pthread_mutex_lock(&bano->mutex);
	while (bano->mujeres > 0 || bano->hombres > 2) {
		printf("Hombre esperando....\n");
		pthread_cond_wait(&bano->lleno, &bano->mutex);
	}
	printf("Hombre entra al baño\n");
	bano->hombres++;
	bano->total_hombres++;
        pthread_mutex_unlock(&bano->mutex);
	usar_bano(bano, 100);
        pthread_mutex_lock(&bano->mutex);
	printf("Hombre salio del baño\n");
	bano->hombres--;
        pthread_cond_broadcast(&bano->lleno);
        pthread_mutex_unlock(&bano->mutex);
	return NULL;
}

static void* mujer(void *datos) {
	bano_u *bano = (bano_u *) datos;
        pthread_mutex_lock(&bano->mutex);
	while (bano->hombres > 0 || bano->mujeres > 2) {
		printf("Mujer esperando....\n");
            pthread_cond_wait(&bano->lleno, &bano->mutex);
	}
	printf("Mujer entra al baño\n");
	bano->mujeres++;
	bano->total_mujeres++;
        pthread_mutex_unlock(&bano->mutex);
	usar_bano(bano, 100);
        pthread_mutex_lock(&bano->mutex);
	bano->mujeres--;
	printf("Mujer salio del baño\n");
        pthread_cond_broadcast(&bano->lleno);
        pthread_mutex_unlock(&bano->mutex);
	return NULL;
}

int main(int argc, char *argv[]) {
	int num_h = 1, num_m = 1;
	
	if ( argc < 3 ) {
                printf("Numero de parametros incorrecto. Indicar el numero de datos a producir.\n") ;
                exit(1);              
        }	
	
	num_h = atoi(argv[1]);
	num_m = atoi(argv[2]);
	if (num_h <= 0) {
		printf("Numero de Hombres debe ser > 0\n");
		exit(EXIT_FAILURE);
	}
	if (num_m <= 0) {
		printf("Numero de Mujeres debe ser > 0\n");
		exit(EXIT_FAILURE);
	}
	int err, n = num_h + num_m;
	pthread_t thread[n];
	for (int i = 0; i < n; i++) {
		if(i < num_h){
			err = pthread_create(&thread[i], NULL,hombre, &bano_unisex);
		}
		if(i >= num_h){
			err = pthread_create(&thread[i], NULL,mujer, &bano_unisex);
		}
		if (err) {
			printf("No se pudo crear la Hebra %d: %d\n", i, err);
            	return EXIT_FAILURE;
        	}
	}
	for (int i = 0; i < n; i++) {
		if (thread[i]){
			(void) pthread_join(thread[i], NULL);
		}			
	}
    return 0;
}
