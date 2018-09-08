#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// usando man sbrk
// ---------------
// void *sbrk(intptr_t increment);
//
// sbrk() increments the program's data space by increment bytes.  Calling sbrk() with an increment of 0  can  be
//       used to find the current location of the program break.
// On success, sbrk() returns the previous program break.  (If the break was increased,  then  this  value  is  a
//       pointer  to  the start of the newly allocated memory).  On error, (void *) -1 is returned, and errno is set to
//       ENOMEM.
// ---------------- 
// En otras palabras...
// sbrk(nbytes) incrementa el espacio de datos del programa en nbytes. Si nbytes=0, entonces sbrk retorna la primera
// posicion donde el heap empieza a crecer.
// Notar que es necesario saber cual es el espacio libre para no pasarse de acuerdo a lo pedido con sbrk() o bien
// llamar de nuevo a sbrk(0) para saber donde quedó el req despues del incremento

typedef struct mblock memblock;

struct mblock {
  size_t size;
  memblock *next;
  void *mip;
  int free;
  int magic; 
};

int main(int argc, char *argv[]) {
  int nbytes = atoi(argv[1]);
  //printf("nbytes = %d\n", nbytes);
 	int B_SIZE = sizeof(memblock);
  memblock *block = sbrk(0);
  printf("block = %p\n", block);
	printf("B_SIZE = %d\n",B_SIZE);
  void *request = sbrk(nbytes + B_SIZE); // notar que aca se esta pidiendo ademas para lo que ocupa el memblock
  if (request == (void*) -1) { // verifica si se puede incrementar este espacio entonces 
    return 1; // sbrk failed.
  }
  printf("request = %p\n", request);
  void *nn = sbrk(0);

  block->size = nbytes;
  block->next = NULL;
  block->free = 1;
  block->magic = 0x12345678;
// ademas hay que especificar la posición inicial para el puntero de datos a retornar del mimalloc
  printf ("block %p request %p nn %p\n", block, request,nn);
  printf ("B_SIZE %d\n", (int)B_SIZE);
  printf ("block size %d, block free %d, block magic %x \n", (int)block->size, block->free, block->magic);

  int nextalloc = 24;
  memblock *new;
  new = (memblock *)(block + B_SIZE + nextalloc);
  new->size = block->size - nextalloc - B_SIZE;
  new->next = block->next;
  new->free = 0;
  new->magic = 0x77777777;
  block->size = nextalloc;
  block->next = new;

  memblock *pp = new;
  while(pp != NULL){
      printf(" pp pointer %p\n", pp);
      printf(" pp size %d free %d magic %x\n", (int)pp->size, pp->free, pp->magic);
      pp = pp->next;
  }




}
