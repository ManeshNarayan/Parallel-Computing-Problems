#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

double instant_factor=0;
double load_factor;
int capacity;

typedef struct hash_entry{
	int value;
	int status;
}hash_entry;

hash_entry* main_Hash;

void add(int key,hash_entry** HashArray);
hash_entry* HashTable(int C)
{
	hash_entry* HashArray=(hash_entry*) malloc(C*sizeof(hash_entry));
	for(int i=0;i<C;i++)
	{
		HashArray[i].value=0;
		HashArray[i].status=-1;
	}
	return HashArray;
}

void rehash(hash_entry** HashArray)
{
	int old_capacity=capacity;
	capacity=old_capacity*2;
	hash_entry** new=(hash_entry**) malloc(sizeof(hash_entry*));
	*new=HashTable(capacity);
	instant_factor=0;
	int i;
	//printf("old_capacity %d\n",old_capacity);
	
	for(int i=0;i<old_capacity;i++)
	{
		if((*HashArray)[i].status==0)
		{
			//printf("to_be copied    %10d %10d %10d\n",i,(*HashArray)[i].value,(*HashArray)[i].status);
			add((*HashArray)[i].value,new);
		}
	}
	*HashArray=*new;
}
void add(int key,hash_entry** HashArray)
{
	int actual_pos=key%capacity;
	int pos=actual_pos;
	int collisions=0;
	while(((*HashArray)[pos].status!=-1 && (*HashArray)[pos].status!=1) && (*HashArray)[pos].value!=key)
	{
		pos=(actual_pos+collisions+collisions*collisions)%capacity;
		//printf("pos add: %d\n",pos);
		collisions++;
	}
	(*HashArray)[pos].value=key;
	(*HashArray)[pos].status=0;
	instant_factor+=(double)1/(double)capacity;
	if(instant_factor>load_factor)
	{  
		//printf("Getting Rehashed\n");
		rehash(HashArray);
	}
}

int find(int key,hash_entry* HashArray)
{
	int actual_pos=key%capacity;
	int pos=actual_pos;
	int collisions=0;
	while(HashArray[pos].status!=-1 && (HashArray[pos].value!=key))
	{
		pos=(actual_pos+collisions+collisions*collisions)%capacity;
		collisions++;
	}
	if(HashArray[pos].status==-1)
		return -1;

	return pos;
}

void delete(int key,hash_entry* HashArray)
{
	int actual_pos=key%capacity;
	int pos=actual_pos;
	int collisions=0;
	while(HashArray[pos].status!=-1 && HashArray[pos].value!=key)
	{
		pos=(actual_pos+collisions+collisions*collisions)%capacity;
		//printf("pos delete: %d\n",pos);
		collisions++;
	}
	if(HashArray[pos].status==-1)
	{
		//printf(" %d does not exist in table\n",key);
		return;
	}
	HashArray[pos].status=1;
	HashArray[pos].value=0;
	instant_factor-=(double)1/(double)capacity;
}

void print_hash()
{
	int i;
	for(i=0;i<capacity;i++)
	{
		printf("%10d %10d %10d\n",i,main_Hash[i].value,main_Hash[i].status);
	}
}

int main()
{
	struct timeval tv1,tv2;
	gettimeofday(&tv1,NULL);
	FILE* fp=fopen("test1.txt","r");
	fscanf(fp,"%d %lf\n",&capacity,&load_factor);
	main_Hash=HashTable(capacity);
	int q;
	char operation[30];
	int value;
	//scanf("%d\n",&q);
	while(fscanf(fp,"%s %d\n",operation,&value)==1)
	{
		//printf("query no.:%d\n",q);
		
		
		if(strcmp(operation,"add")==0)
		{
			add(value,&main_Hash);
			//print_hash();
		}
		if(strcmp(operation,"search")==0)
		{
			int ans;
			ans=find(value,main_Hash);
			//printf("Found %d at position %d\n",value,ans);
		}
		if(strcmp(operation,"delete")==0)
		{
			delete(value,main_Hash);
			//print_hash();
		}
	}
	fclose(fp);
	gettimeofday(&tv2, NULL);

	printf ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));
	exit(0);
}