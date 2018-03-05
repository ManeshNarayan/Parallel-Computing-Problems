#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#define NUMTHREADS 8	

FILE* fp;
double instant_factor=0;
double load_factor;
int capacity;
int search_counter=0;
void print_hash();
typedef struct hash_entry{
	int value;
	int status;
	pthread_mutex_t lock;
}hash_entry;

typedef struct data{
	int id;
}data;

int count=0;
pthread_mutex_t hash_lock;
pthread_cond_t count_threshold;
hash_entry* main_Hash;

data thread_array[NUMTHREADS];
void add(int key,hash_entry** HashArray);
hash_entry* HashTable(int C)
{
	hash_entry* HashArray=(hash_entry*) malloc(C*sizeof(hash_entry));
	for(int i=0;i<C;i++)
	{
		HashArray[i].value=0;
		HashArray[i].status=-1;
		pthread_mutex_init(&(HashArray[i].lock),NULL);
	}
	return HashArray;
}

void rehash(hash_entry** HashArray)
{
	while(search_counter==1)
		continue;
	if(instant_factor<=load_factor)
		return;
	search_counter=1;
	//printf("Getting rehashed\n");
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
			int actual_pos=(*HashArray)[i].value%capacity;
			int pos=actual_pos;
			int collisions=0;
			while(((*new)[pos].status!=-1 && (*new)[pos].status!=1) && (*new)[pos].value!=(*HashArray)[i].value)
			{	
				pos=(actual_pos+collisions+collisions*collisions)%capacity;
				//printf("pos add: %d\n",pos);
				collisions++;
			}
			(*new)[pos].value=(*HashArray)[i].value;
			(*new)[pos].status=0;
			instant_factor+=(double)1/(double)capacity;
			//printf("to_be copied    %10d %10d %10d\n",i,(*HashArray)[i].value,(*HashArray)[i].status);
			//add((*HashArray)[i].value,new);
		}
	}
	*HashArray=*new;
	//printf("Completed rehashing\n");
	search_counter=0;
	// if(instant_factor<=load_factor)
	// 	pthread_cond_broadcast(&count_threshold);
}
void add(int key,hash_entry** HashArray)
{
	while(search_counter==1)
		continue;
	// while(instant_factor>load_factor)
	// 	pthread_cond_wait(&count_threshold,&hash_lock);
	int actual_pos=key%capacity;
	int pos=actual_pos;
	int collisions=0;
	int rc;
	//pthread_mutex_lock(&hash_lock);
	

	//printf("entering add %d %d\n",key,search_counter);
	int first=0;
	while(((*HashArray)[pos].status!=-1 && (*HashArray)[pos].status!=1) && (*HashArray)[pos].value!=key)
	{
		if(first==1)
		pthread_mutex_unlock(&((*HashArray)[pos].lock));
		first=1;
		pos=(actual_pos+collisions+collisions*collisions)%capacity;
		//printf("pos add: %d\n",pos);
		rc=pthread_mutex_lock(&((*HashArray)[pos].lock));
		if(rc)
		{
			printf("Error in add lock %d\n",key);
			exit(0);
		}
		//pthread_mutex_lock(&((*HashArray)[pos].lock));
		collisions++;
	}
	if(first==0)
	{
		rc=pthread_mutex_lock(&((*HashArray)[pos].lock));
		if(rc)
		{
			printf("Error in add lock %d\n",key);
			exit(0);
		}
		// /pthread_mutex_lock(&((*HashArray)[pos].lock));
	}
	(*HashArray)[pos].value=key;
	(*HashArray)[pos].status=0;
	instant_factor+=(double)1/(double)capacity;
	//print_hash();
	//search_counter--;
	fprintf(fp,"add %d\n",key);
	pthread_mutex_unlock(&((*HashArray)[pos].lock));
	
	
	//print_hash();
	//printf("releasing add %d %d\n",key,search_counter);
	
	// search_counter++;
	// pthread_mutex_unlock(&hash_lock);
}

int find(int key,hash_entry** HashArray)
{
	while(search_counter==1)
		continue;
	// while(instant_factor>load_factor)
	// 	pthread_cond_wait(&count_threshold,&hash_lock);
	int actual_pos=key%capacity;
	int pos=actual_pos;
	int collisions=0;

	// search_counter++;
	
	//pthread_mutex_lock(&hash_lock);
	//printf("entering find %d %d\n",key,search_counter);
	while((*HashArray)[pos].status!=-1 && ((*HashArray)[pos].value!=key))
	{

		pos=(actual_pos+collisions+collisions*collisions)%capacity;
		collisions++;
	}
	fprintf(fp,"find %d\n",key);
	if((*HashArray)[pos].status==-1)
		
		{
			
			//printf("releasing find %d %d\n",key,search_counter);

			// //print_hash();
			// search_counter--;
			//pthread_mutex_unlock(&hash_lock);

			return -1;
		}

		
	//printf("releasing find %d %d\n",key,search_counter);
	// //print_hash();
	// search_counter--;
	
	//pthread_mutex_unlock(&hash_lock);
	return pos;
}

void delete(int key,hash_entry** HashArray)
{

	while(search_counter==1)
		continue;
	// while(instant_factor>load_factor)
	// 	pthread_cond_wait(&count_threshold,&hash_lock);
	int actual_pos=key%capacity;
	int pos=actual_pos;
	int collisions=0;
	int rc;
	//pthread_mutex_lock(&hash_lock);

	//printf("entering delete %d %d\n",key,search_counter);
	int first=0;
	while((*HashArray)[pos].status!=-1 && (*HashArray)[pos].value!=key)
	{
		if(first==1)
		pthread_mutex_unlock(&((*HashArray)[pos].lock));
		first=1;
		pos=(actual_pos+collisions+collisions*collisions)%capacity;
		rc=pthread_mutex_lock(&((*HashArray)[pos].lock));
		if(rc)
		{
			printf("Error in add lock");
			exit(0);
		}
		//pthread_mutex_lock(&((*HashArray)[pos].lock));
		//printf("pos delete: %d\n",pos);
		collisions++;
	}
	if(first==0)
	{
		rc=pthread_mutex_lock(&((*HashArray)[pos].lock));
		if(rc)
		{
			printf("Error in add lock");
			exit(0);
		}
		//pthread_mutex_lock(&((*HashArray)[pos].lock));
	}
	fprintf(fp,"delete %d\n",key);
	if((*HashArray)[pos].status==-1)
	{

		//printf(" %d does not exist in table\n",key);
		//printf("releasing delete %d %d\n",key,search_counter);
		// //print_hash();
		// search_counter++;

	pthread_mutex_unlock(&((*HashArray)[pos].lock));
		// pthread_mutex_unlock(&hash_lock);
		
		return;
	}
	

	(*HashArray)[pos].status=1;
	(*HashArray)[pos].value=0;
	instant_factor-=(double)1/(double)capacity;
	//print_hash();
	pthread_mutex_unlock(&((*HashArray)[pos].lock));
	//printf("releasing delete %d %d\n",key,search_counter);
	//print_hash();
	//search_counter++;
	//pthread_mutex_unlock(&hash_lock);
}

void print_hash()
{
	int i;
	for(i=0;i<capacity;i++)
	{
		printf("%10d %10d %10d\n",i,main_Hash[i].value,main_Hash[i].status);
	}
}

void* generate_test(void* threadarg)
{
	int i,j;
	int ans;
	data* temp=(data*) threadarg;
	int t=temp->id;
	for(i=0;i<1000000;i++)
	{
		j=rand()%(100000)+1;
		if(t%4==0)
		{
			if(j%100<72)
			{
				//printf("searching %d in thread %d\n",j,t);
				ans=find(j,&main_Hash);
				
			}
			else if(j%100<90)
			{
				//printf("adding %d in thread %d\n",j,t);
				add(j,&main_Hash);
				if(instant_factor>load_factor)
				{  
					//printf("Getting Rehashed\n");
					rehash(&main_Hash);
				}
				//print_hash();
			}
			else
			{
				// printf("deleting %d in thread %d\n",j,t);
				delete(j,&main_Hash);
				//print_hash();
			}
		}
		else if(t%4==1)
		{
			if(j%100<32)
			{
				// printf("searching %d in thread %d\n",j,t);
				ans=find(j,&main_Hash);
			}
			else if(j%100<85)
			{
				// printf("adding %d in thread %d\n",j,t);
				add(j,&main_Hash);
				if(instant_factor>load_factor)
				{  
					//printf("Getting Rehashed\n");
					rehash(&main_Hash);
				}
			}
			else
			{
				// printf("deleting %d in thread %d\n",j,t);
				delete(j,&main_Hash);
			}
		}
		else if(t%4==2)
		{
			if(j%100>65)
			{
				// printf("searching %d in thread %d\n",j,t);
				ans=find(j,&main_Hash);
			}
			else if(j%100>20)
			{
				// printf("adding %d in thread %d\n",j,t);
				add(j,&main_Hash);
				if(instant_factor>load_factor)
				{  
					//printf("Getting Rehashed\n");
					rehash(&main_Hash);
				}
			}
			else
			{
				// printf("deleting %d in thread %d\n",j,t);
				delete(j,&main_Hash);
			}
		}
		else
		{
			if(j%100>30)
			{
				// printf("searching %d in thread %d\n",j,t);
				ans=find(j,&main_Hash);
			}
			else if(j%100>10)
			{
				// printf("adding %d in thread %d\n",j,t);
				add(j,&main_Hash);
				if(instant_factor>load_factor)
				{  
					//printf("Getting Rehashed\n");
					rehash(&main_Hash);
				}
			}
			else
			{
				// printf("deleting %d in thread %d\n",j,t);
				delete(j,&main_Hash);
			}
		}
	}
	pthread_exit(NULL);
}

int main()
{
	struct timeval tv1,tv2;
	gettimeofday(&tv1,NULL);

	pthread_mutex_init(&hash_lock, NULL);
	pthread_cond_init (&count_threshold, NULL);
	pthread_t thread[NUMTHREADS];
	pthread_attr_t join_attr;
	int fail;
	long t;
	void *status;
	FILE* fp2=fopen("test","r");
	fscanf(fp2,"%d %lf\n",&capacity,&load_factor);
	fclose(fp2);
	pthread_attr_init(&join_attr);
	pthread_attr_setdetachstate(&join_attr,PTHREAD_CREATE_JOINABLE);
	
	
	fp=fopen("test1.txt","w");
	fprintf(fp,"%d %lf\n",capacity,load_factor);
	main_Hash=HashTable(capacity);
	int i;
	for(i=0;i<NUMTHREADS;i++)
	{
		thread_array[i].id=i;
	}
	for(t=0;t<NUMTHREADS;t++)
	{
		fail=pthread_create(&thread[t],&join_attr,generate_test,(void*) &thread_array[t]);
		if(fail)
		{
			printf("ERROR: Thread cannot be formed");
			exit(-1);
		}
	}
	

	
              pthread_attr_destroy(&join_attr);

    	for(t=0; t<NUMTHREADS; t++) {
       fail = pthread_join(thread[t], &status);
       if (fail) {
          printf("ERROR: pthread can't be joined\n");
          exit(-1);
          }
       }
	pthread_mutex_destroy(&hash_lock);
	pthread_cond_destroy(&count_threshold);

	fclose(fp);
	gettimeofday(&tv2, NULL);

	printf ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));
	pthread_exit(NULL);
}