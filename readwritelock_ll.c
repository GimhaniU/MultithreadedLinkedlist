#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

//struct for linked list node
struct list_node_s
{
	int data;
	struct list_node_s* next;
};


//method initializations
int Member(int value);
int Insert(int value);
int Delete(int value);
void extractArgs(int argc,char* argv[]);/*to extract 5 argument values*/
void populatelinkedlist(); /*to initially add values to linkedlist*/
void initializearray(); /*initialize op_array with needed number of operations*/
void shufflearray(); /*to make random order*/
void* doOperations(void* id); /*to complete m operations in a randomized manner*/

//global variables
struct list_node_s* head_p; /*head pointer*/

int n=0; /*n values for linkedlist nodes*/
int m=0; /*m operations in total*/

double percMember=0; /*percentage of member operations*/
double percInsert=0; /*percentage of insert operations*/
double percDelete=0; /*percentage of delete operations*/

int m_member=0;
int m_insert=0;
int m_delete=0;

int* op_array; /*array to determine which operation to select*/

int sample_size=150;
double mean=0;
double std_deviation;

//to use with threads and rw lock
pthread_rwlock_t rwlock;
int th_count;
volatile int th_completed=0;


int main(int argc, char* argv[])
{
	
	double array_timetaken[sample_size];
	double total_time=0,std_error=0;

	extractArgs(argc,argv);
	int arr[m];
	op_array=arr;
	initializearray();


	//given a set of m operations, try out samples
	for(int i=0;i<sample_size;i++){
		//to measure time
		double start,end,time_taken;

		long th_id; 
                pthread_t* thread_handles;

		populatelinkedlist();		
		shufflearray();

		//allocate memory to threads
	        thread_handles = (pthread_t*) malloc (th_count*sizeof(pthread_t));
		pthread_rwlock_init(&rwlock, NULL); /*initialize rw lock*/

		start=clock();
		for (th_id = 0; th_id < th_count; th_id++)  
		{
		    pthread_create(&thread_handles[th_id], NULL, doOperations , (void*)th_id);  
		}

		for (th_id = 0; th_id < th_count; th_id++) 
		{
		    pthread_join(thread_handles[th_id], NULL); 
		}

        	pthread_rwlock_destroy(&rwlock);
		
		end=clock();

		time_taken=(end-start)/CLOCKS_PER_SEC;
		
		array_timetaken[i]=time_taken;
		total_time+=time_taken;
		head_p=NULL;
	}
	
	/*calculate mean and standard deviation*/	
	mean= total_time/sample_size;
		
	for(int i=0;i<sample_size;i++){
		std_error+=pow(array_timetaken[i] - mean, 2);
	}
	std_deviation=sqrt(std_error/sample_size);
	
	printf("Mean :%.5f\nStd deviation:%.5f\n",mean,std_deviation);

}


void extractArgs(int argc,char* argv[])
{
	if(argc ==7)
	{
		n=atoi(argv[1]);
		m=atoi(argv[2]);
		percMember=atof(argv[3]);
		percInsert=atof(argv[4]);
		percDelete=atof(argv[5]);
		th_count= atoi(argv[6]);
		if(percInsert+percMember+percDelete != 1.0){
			printf("Please check percentages again\n");
			exit(0);		
		}else if(n<0 || m<0){
			printf("Please check n and m values again\n");
			exit(0);
		}		
	}else{
		printf("%s","Please insert arguments in order <numberOfNodes> <numberOfOperations> <percentageOfMember> <percentageOfInsert> <percentageOfDelete>\n");
		exit(0);
	}

	//calculate number of operations for each type
	m_member= m * percMember;
	m_insert=m*percInsert;
	m_delete=m*percDelete;
	
}/*extractArgs*/

void populatelinkedlist(){
	for(int i=0;i<n;i++){
		int r=rand()%65536;
		int result=Insert(r);
		if(!result){
			i--;
		}	
	}		
}/*populate linkedlist*/

void initializearray(){
	int i=0;
	/*fill m elements of array with 1,2,3 to denote delete,insert and member*/
	for(;i<m_delete;i++)
		op_array[i]=1;
	for(;i<m_delete+m_insert;i++)
		op_array[i]=2;
	for(;i<m;i++)
		op_array[i]=3;
}

void shufflearray(){
	int i=0;

	/*shuffle the array: Fisher–Yates_shuffle*/
	for (i = 0; i <m-2; i++) {
		int j=rand()%m;
		while(j<i){
			j=rand()%m;
		}    		
                int tmp = op_array[j];
      		op_array[j] = op_array[i];
    		op_array[i] = tmp;
  	}
	
}

void* doOperations(void* id){
	/*do operation based on value*/
	
	while(th_completed<m){
		
	 	int i=0;
		printf("%ld thread:%i\n",(long)id,th_completed);
		
		if(op_array[i]==1){
			pthread_rwlock_wrlock(&rwlock);
			i=th_completed;
			Delete(rand()%65536);
			th_completed++;
			pthread_rwlock_unlock(&rwlock);	
		}else if(op_array[i]==2){
			pthread_rwlock_wrlock(&rwlock);
			i=th_completed;
			/*if insert was an already done one,do it again */
			int res=Insert(rand()%65536);
			while(!res){
				res=Insert(rand()%65536);			
			}
			th_completed++;
			pthread_rwlock_unlock(&rwlock);	
		}else if(op_array[i]==3){
			pthread_rwlock_rdlock(&rwlock);
			i=th_completed;
			Member(rand()%65536);
			th_completed++;
			pthread_rwlock_unlock(&rwlock);
		}
	}
	
}/*doOperations*/

int Member(int value)
{
	struct list_node_s* curr_p =head_p;

	while(curr_p != NULL && curr_p ->data <value)
		curr_p= curr_p ->next;

	if(curr_p == NULL || curr_p -> data > value){
		return 0;
	}else{
		return 1;
	}
}/*Member*/

int Insert(int value)
{
	struct list_node_s* curr_p = *&head_p;
	struct list_node_s* pred_p = NULL;
	struct list_node_s* temp_p = NULL;

	while(curr_p != NULL && curr_p -> data <value){
		pred_p =curr_p;
		curr_p =curr_p ->next;
	}

	if(curr_p == NULL || curr_p ->data >value){
		temp_p = malloc(sizeof( struct list_node_s));
		temp_p -> data =value;
		temp_p ->next = curr_p;
		if(pred_p == NULL) /*first node*/
			head_p= temp_p;
		else
			pred_p -> next =temp_p;
		return 1;
	}else{  /*if value already inlist return 0*/
		return 0;
	}
}/*Insert*/

int Delete(int value)
{
	struct list_node_s* curr_p =*&head_p;
	struct list_node_s* pred_p = NULL;

	while(curr_p != NULL && curr_p -> data <value){
		pred_p =curr_p;
		curr_p =curr_p ->next;
	}

	if(curr_p != NULL && curr_p -> data == value){
		if(pred_p == NULL){
			*&head_p=curr_p->next;
			free(curr_p);
		}else{
			pred_p->next =curr_p->next;
			free(curr_p);
		}
		return 1;
	}else{
		return 0; /*value not in list*/	
	}
}/*Delete*/


