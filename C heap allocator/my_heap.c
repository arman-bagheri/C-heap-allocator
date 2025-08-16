#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MIN_HEAP_DATA sizeof(heap_node)-sizeof(size_t)

typedef struct heap_node {
    size_t size;
    struct heap_node* next;
} heap_node;


void* heap_bound;
heap_node* heap;

void remove_node(heap_node *node){
	heap_node* counter = heap;
	if(counter==node){
		heap = heap->next;
		return;
	}
	if(node==NULL) return;
	while(counter!=NULL){
		if(counter->next==node){
			counter->next = counter->next->next;
			return;
		}
		counter = counter->next;
	}
}

void coalesce(heap_node* node){
	//coalesce after
	if((heap_node*)((char*)node + node->size)==node->next){
		node->size += node->next->size;
		node->next = node->next->next;
	}
	//coalesce before
	if(node==heap) return;  //if the first node
	heap_node* before = heap;
	while(before->next!=node)
		before = before->next;
	if(((heap_node*)((char*)before + before->size))==node){
		before->size += node->size;
		before->next = node->next;
	}
}

void* my_malloc(size_t n){	// check heap nodes, if enough space, allocate and insert nodes for remaining space, if not, sbrk
	if(n==0) return NULL;
	if(n<MIN_HEAP_DATA){
		n = MIN_HEAP_DATA;
	}
	void* ptr;
	heap_node* nodes;
	if(heap==NULL){
		ptr = (void*)((char*)sbrk(n+sizeof(size_t)) + sizeof(size_t));
		if(ptr==(void*)-1){
			return NULL;
		}
		*(size_t*)((char*)ptr - sizeof(size_t)) = n;  // size of allocated blocks is the size of the block - the size_t
		return ptr;
	}
	else{
		nodes = heap;
		
		while(1){
			if(nodes->size>=n+sizeof(size_t)){
				if(nodes->size-n-sizeof(size_t)<sizeof(heap_node)){
					ptr = (void*)((char*)nodes + sizeof(size_t));
					*(size_t*)((char*)ptr - sizeof(size_t)) = n;
					remove_node(nodes);
					return ptr;
				}
				else{
					nodes->size -= n + sizeof(size_t);
					ptr = (void*)((char*)nodes + nodes->size + sizeof(size_t));
					*(size_t*)((char*)ptr - sizeof(size_t)) = n;
					return ptr;
				}
			}
			if(nodes->next==NULL) break;
			nodes = nodes->next;
		}
		ptr = (void*)((char*)sbrk(n+sizeof(size_t)) + sizeof(size_t));
		if(ptr==(void*)-1){
			return NULL;
		}
		*(size_t*)((char*)ptr - sizeof(size_t)) = n;
		return ptr;
	}
}

void my_free(void* segment){
	heap_node* before_node = heap;
	heap_node* node = (heap_node*)((char*)segment - sizeof(size_t));
	node->size = (*(size_t*)((char*)segment - sizeof(size_t))) + sizeof(size_t);
	node->next = NULL;
	if(before_node==NULL){
		heap = node;
		return;
	}

	if(before_node>node){
		node->next = before_node;
		heap = node;
	}
	else{
		while (before_node->next != NULL){
			if(before_node->next>node) break;
   			before_node = before_node->next;
		}
	
		node->next = before_node->next;
		before_node->next = node;
		}
	coalesce(node);
}



void printHeap(){
	heap_node* counter=heap;
	printf("my heap is:\n");
	while(counter!=NULL){
		printf("\nMy address is :%p\nMy size is: %ld\nMy next block is: %p\n\n",counter,counter->size,counter->next);
		counter=counter->next;
	}
}

int main(){
	heap = NULL;
	void* ptr = my_malloc(sizeof(int));
	void* p1 = my_malloc(18);
	void* p2 = my_malloc(20);

	my_free(ptr);
	printHeap();

	
	printf("%ld\n",MIN_HEAP_DATA);
	printf("%ld\n",sizeof(int));
	printf("%ld\n",sizeof(size_t));
	printf("%ld\n",sizeof(heap_node));
	printHeap();

	my_free(p2);
	printHeap();

	my_free(p1);
	printHeap();
	return 0;
}