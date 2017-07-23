#include <stdlib.h>
#include <stdio.h>
#include "wish_mem.h"

#define ALLOC_MIN_SIZE 4 // power of 2 units
#define ALLOC_MAX_VERTEX_SIZE 24 // power of 2 units
#define ALLOC_MAX_EDGE_SIZE 32 // power of 2 units
#define NUM_OF_POOLS (ALLOC_MAX_EDGE_SIZE -	ALLOC_MIN_SIZE + 1)
#define MIN_THRESHOLD 3
#define GET_NEXT_POWER_OF_2_EXPONENT(_size_) ((sizeof(unsigned int)<<3) - \
		__builtin_clz((_size_)-1))  
#define GET_POOL_INDEX(_size_) (GET_NEXT_POWER_OF_2_EXPONENT((_size_)) - \
		ALLOC_MIN_SIZE)


struct pool_element{
	struct pool_element* next;
	char pool_index;
};

struct buf_pool{
	unsigned int free_num;
	unsigned int busy_num;
	unsigned int busy_threshold;
	struct pool_element* free_el_list;
};  

struct buf_pool pools [NUM_OF_POOLS];
unsigned long long current_allocated_buffers;

static struct pool_element* pool_pop_free_element(unsigned int pool_index)
{
	struct buf_pool* pool = pools + pool_index;
	struct pool_element* ret = pool->free_el_list;
	if(ret){
		pool->free_el_list = pool->free_el_list->next;
		pool->free_num--;
	}
	return ret;
}

static void pool_push_free_element(struct pool_element* el)
{
	struct buf_pool* pool = pools + (int)el->pool_index;
	el->next = pool->free_el_list;
	pool->free_el_list = el;
	pool->free_num++;
}

static void* pool_free_and_alloc(unsigned int size, unsigned int pool_index)
{
	for(; pool_index < NUM_OF_POOLS; ++pool_index){
		struct pool_element* ret = pool_pop_free_element(pool_index);
		if (ret){
			printf("free and alloc\n");
			free(ret);
			return malloc(size);
		}
	}
	return NULL;
}

static void free_all_pool_buffers(void)
{
	int pool_index=0;
	struct pool_element* element;
	for(; pool_index < NUM_OF_POOLS; ++pool_index){
		while((element=pool_pop_free_element(pool_index))){
			printf("Free(all) edge address %p\n",element);
			free(element);
		}
	}
}

static void update_pool_busy_threshold(struct buf_pool* pool)
{
	pool->busy_threshold = (((pool->busy_threshold) << 1) +	pool->busy_num)/3;
	pool->busy_threshold = pool->busy_threshold < MIN_THRESHOLD ? 
		MIN_THRESHOLD : pool->busy_threshold;
}

void* allocateEdge(unsigned int size)
{
	struct pool_element* element;
	char pool_index = GET_POOL_INDEX(size);
	struct buf_pool* pool = pools + (int)pool_index;
	if (!(element=pool_pop_free_element(pool_index))){
		element = malloc(sizeof(struct pool_element) + size);
		if(!element && !(element = pool_free_and_alloc(
			sizeof(struct pool_element) + size,pool_index+1)))
			return NULL;
		printf("Allocate edge address %p size %lu\n",element,sizeof(struct pool_element) + size);
		element->pool_index = pool_index;
	}
	pool->busy_num++;
	update_pool_busy_threshold(pool);
	current_allocated_buffers++;
	return (void*)(element+1);
}

void freeEdge(void* data)
{
	struct pool_element* element = (struct pool_element*)data - 1;
	struct buf_pool* pool = pools + (int)element->pool_index;
	pool->busy_num--;
	update_pool_busy_threshold(pool);
	if(pool->busy_num + pool->free_num > pool->busy_threshold){
		printf("Free edge address %p\n",element);
		free(element);
	}else{
		pool_push_free_element(element);
	}
	if(--current_allocated_buffers == 0)
		free_all_pool_buffers();
}

void* allocateVertex(unsigned int size)
{
	void* ret = malloc(size);
	if(! ret){
		if((ret=pool_free_and_alloc(size,
			GET_POOL_INDEX(size-sizeof(struct pool_element)))))
			current_allocated_buffers++;
	}else
		current_allocated_buffers++;
	return ret;
}

void freeVertex(void* data)
{
	free(data);
	if(--current_allocated_buffers == 0)
		free_all_pool_buffers();
}