#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
memblock lista=NULL;

void addBlock(memblock block){
	if(lista == NULL)
	{
		lista = block;
		block->next = NULL;
		block->prev = NULL;
	}
	else{
		memblock actual = lista;
		while(actual->next != NULL){
			actual = actual->next;
		}
		actual->next = block;
		block->prev = actual;
	}
}

void extendHeap(int n){
	void* head = sbrk(0);
	void* tmp = sbrk(n + BLOCK_SIZE);
	void* ptr = head + BLOCK_SIZE;
	memblock block = (memblock)head;
	block->size = n;
	block->prev = NULL;
	block->next = NULL;
	block->mip = ptr;
	block->magic = 0x77777777;
	block->free = 1;
	addBlock(block);
}

void split_block(memblock block, int s){
	if(block->size - s > BLOCK_SIZE){
		memblock new = (memblock)(block->mip + s);
		new->size = block->size - s - BLOCK_SIZE;
		new->next = block->next;
		new->prev = block;
		new->free = 1;
		if(block ->next != NULL){
			block->next->prev = new;
		} 
		block->next = new;
		block->size = s;
		new->magic = 0x77777777;
		new->mip = (void*)new + BLOCK_SIZE;
	}
	
}

memblock fusion ( memblock block){
	if (block->next != NULL && block->next->free ){
		block->size += BLOCK_SIZE + block->next->size;
		block->next = block->next ->next;
		if (block->next)
			block->next ->prev = block;
	}
	return (block);
}


void mifree(void* ptr){
	memblock actual = lista;
	while(actual != NULL){
		if(actual->mip == ptr){
			actual->free = 1;
			actual->magic = 0x77777777;
			if(actual->next != NULL)
				fusion(actual);
			if(actual->prev != NULL && actual->prev->free == 1)
				fusion(actual->prev);
			if(actual->next == NULL)
				brk(actual->mip + actual->size);
			return;
		}
		actual = actual->next;
	}
}


memblock find_block2(int s){
	memblock actual = lista;
	while(actual != NULL){
		if(actual->size >= s && actual->free == 1){
			return actual;
		}
		actual = actual->next;
	}
	return NULL;
}

//first fit
void* mimalloc(int size){
	memblock block = find_block2(size);
	if(block == NULL){
		extendHeap(size);
		block = find_block2(size);

	}
	block->free = 0;
	block->magic = 0x12345678;
	split_block(block, size);
	return block->mip;
}

void imprimirBloque(memblock block){
	puts("----------------");
	printf("mip : %p\n",block->mip);
	printf("size : %d\n",block->size);
	printf("magic: %x\n",block->magic);
	printf("free : %d\n\n",block->free);
	puts("----------------");
}

int tamanoHeap(){
	int a = 0;	
	memblock actual = lista;
	while(actual != NULL){
		a += actual->size;
		actual = actual->next;
	}
return a;
}

void imprimirHeap(){
	int a = 0;	
	memblock actual = lista;
	while(actual != NULL){
		imprimirBloque(actual);
		actual = actual->next;
	}
}


int main(char argc, char * argv[]){

	int tam = atoi(argv[1]);
	extendHeap(tam);
	printf("Tamaño del inicial del heap : %d\n",tam);
	printf("Tamaño del header: %d\n", BLOCK_SIZE);

	void* ptr1 = mimalloc(20);
	void* ptr2 = mimalloc(20);
	void* ptr3 = mimalloc(20);
	void* ptr4 = mimalloc(25);
	void* ptr5 = mimalloc(10);
	void* ptr6 = mimalloc(32);

	printf("Estado 1\n");
	imprimirHeap();	

	mifree(ptr1);
	mifree(ptr2);
	mifree(ptr3);
	mifree(ptr4);

	printf("Estado 2\n");
	imprimirHeap();

	ptr1 = mimalloc(30);
	ptr2 = mimalloc(15);
	ptr3 = mimalloc(10);
	ptr4 = mimalloc(25);
	
	printf("Estado 3\n");
	imprimirHeap();

	mifree(ptr1);
	mifree(ptr2);
	mifree(ptr3);
	mifree(ptr4);
	mifree(ptr5);
	mifree(ptr6);

	printf("Estado 4\n");
	imprimirHeap();

	printf("Tamaño del final del heap : %d\n",tamanoHeap());
	return 0;
}

