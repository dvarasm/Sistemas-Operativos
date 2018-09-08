#include <stdio.h>
#include <unistd.h>


typedef struct s_block *memblock;
struct s_block {
	int size;
	memblock next;
	memblock prev;
	int free;
	int magic;
	void *mip;
};

int BLOCK_SIZE = sizeof(struct s_block);
void *lista=NULL;

memblock find_block(memblock *actual, int size){
	memblock block = lista;
	while(block && !(!(block->free) && block->size >= size)) {
		*actual = block;
		block = block->next;
	}
	return (block);
}

void split_block(memblock block, int s){
	memblock new;
	new = (memblock)(block + BLOCK_SIZE + s); 
	new->size = block->size - s - BLOCK_SIZE;
	new->next = block->next;
	new->free = 0;
	block->size    = s;
	block->next    = new;
}

memblock fusion ( memblock block){
	if (block->next && block->next ->free ){
		block->size += BLOCK_SIZE + block->next ->size;
		block->next = block->next ->next;
		if (block->next)
			block->next ->prev = block;
	}
	return (block);
}

void imprimir_bloque(memblock block){
	printf("mip : %p\n",block->mip);
	printf("size : %d\n",block->size);
	printf("magic: %x\n",block->magic);
	printf("free : %d\n\n",block->free);
}

memblock find(int p){
	memblock indice;
  	indice = lista;
  	while(indice->prev){
		indice = indice->prev;
	}
	while(indice) {
		if((indice->free)==1 && (indice->size == p)){
			return(indice);
		}
		indice = indice->next;
	}
	return NULL;
}

void mfree(int p){
	memblock block;
	block = find(p);
	//printf(":D\n");
	block->free = 0;
	block->magic = 0x77777777;
	if(block->prev && block->prev ->free)
		block = fusion (block->prev );
	if (block->next)
		fusion (block);
	else{
		if (block->prev)
			block->prev ->next = NULL;
		else
			lista = NULL;
		brk(block);
	}
	printf("inicio bloque : %p\n",sbrk(0));
	printf("Libero memoria\n");
	imprimir_bloque(block);
}



memblock extend_heap(memblock actual, int s){
	
	memblock block;
	block = sbrk(0);
	
	printf("Reservo Memoria\n");
	printf("inicio bloque : %p\n",block);
	void *a = sbrk(BLOCK_SIZE + s);
		
	if( a == (void*)-1)
		return NULL;
	
	
	block->size = s;
	block->magic = 0x12345678;
	block->next = NULL;
	block->prev = NULL;
	block->mip = sbrk(0);
		
	if(actual){
		if(actual->next !=NULL)actual->prev = NULL;
		actual->next = block;
		block->prev = actual;
	}
	block->free = 1;

	imprimir_bloque(block);
	return (block);
}


int i=0;
void *mimalloc(int s){
	memblock block,actual;
	if(lista) {
		
		actual = lista;
		block = find_block(&actual,s);
		if(block) {
			if((block->size - s) >= (BLOCK_SIZE + 4))
				split_block(block,s);
			block->free=1;
		}
		else{
			block = extend_heap(actual,s);
			if(!block)
				return(NULL);
			lista = block;
			printf("lista1->prev: %p\n",block->prev);
		}
		
	}
	else{
		block = extend_heap(NULL,s);
		
		if(!block)
			return(NULL);

		lista = block;
		
	}
	return (block);
}

int main(){

	
	printf("BLOCK size : %d ----------\n", BLOCK_SIZE);
	mimalloc(10);
	mimalloc(20);
	mimalloc(30);
	mfree(20);
	mfree(30);
	//printf("D:\n");
	mimalloc(20);
	mimalloc(30);
	//MostrarLista(lista,DESCENDENTE);
	return 0;
}

