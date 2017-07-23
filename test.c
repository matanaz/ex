#include <stdio.h>
#include "wish_mem.h"


#define POOL_NUMBER 28
#define RETRIES 2000

int i=0,j=0;
void* data[POOL_NUMBER][RETRIES];
void* data2[POOL_NUMBER][RETRIES];

static void test1(void)
{
	for(i=0;i<POOL_NUMBER;++i)
	{
		for(j=0;j<RETRIES;++j){
			void* data = allocateEdge(1<<(i+4));
			void* data2 = allocateVertex((1<<j) +i*j);
			if (data) freeEdge(data);
			if(data2) freeVertex(data2);
		}
	}
} 

static void test2(void)
{
	for(i=0;i<POOL_NUMBER;++i)
	{
		for(j=0;j<RETRIES;++j)
			data[i][j] = allocateEdge(1<<(i+4));
		for(j=0;j<RETRIES;++j)
			data2[i][j] = allocateVertex((1<<j) +i*j);

	}
	for(i=0;i<POOL_NUMBER;++i)
	{
		for(j=0;j<RETRIES;++j)
			if (data[i][j]) freeEdge(data[i][j]);
		for(j=0;j<RETRIES;++j)
			if (data2[i][j]) freeVertex(data2[i][j]);
	}
}

static void test3(void)
{	
	for(i=POOL_NUMBER-1;i>=0;--i)
	{
		for(j=0;j<RETRIES;++j)
			data[i][j] = allocateEdge(1<<(i+4));
		for(j=0;j<RETRIES;++j)
			data2[i][j] = allocateVertex((1<<j) -i*j);

	}
	for(i=0;i<POOL_NUMBER;++i)
	{
		for(j=0;j<RETRIES;++j)
			if (data[i][j]) freeEdge(data[i][j]);
		for(j=0;j<RETRIES;++j)
			if (data2[i][j]) freeVertex(data2[i][j]);
	}
}	

static void test4(void)
{
	for(j=0;j<RETRIES;++j){
		for(i=0;i<POOL_NUMBER;++i){
			data[i][j] = allocateEdge(1<<(i+4));
			data2[i][j] = allocateVertex((1<<j) +i*j);
		}
	}
	for(i=0;i<POOL_NUMBER;++i)
	{
		for(j=0;j<RETRIES;++j)
			if (data[i][j]) freeEdge(data[i][j]);
		for(j=0;j<RETRIES;++j)
			if (data2[i][j]) freeVertex(data2[i][j]);
	}
}

static void test5(void)
{
	for(j=0;j<RETRIES;++j){
		for(i=0;i<POOL_NUMBER;++i){
			data[i][j] = allocateEdge(1<<(i+4));
			if (data[i][j]) freeEdge(data[i][j]);
			data2[i][j] = allocateVertex((1<<j) +i*j);
		}
	}
	for(i=0;i<POOL_NUMBER;++i)
	{
		for(j=0;j<RETRIES;++j)
			if (data2[i][j]) freeVertex(data2[i][j]);
	}
}

int main()
{
	printf("Starting wish mem tests...\n");
	test1();
	test2();
	test3();
	test4();
	test5();
	printf("Finish wish mem tests...\n");
	return 0;
}