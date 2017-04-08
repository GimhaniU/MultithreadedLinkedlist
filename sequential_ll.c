#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

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
void doOperations(); /*to complete m operations in a randomized manner*/

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
		populatelinkedlist();
		shufflearray();

		//to measure time
		double start,end,time_taken;

		start=clock();
		doOperations();
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
	if(argc ==6)
	{
		n=atoi(argv[1]);
		m=atoi(argv[2]);
		percMember=atof(argv[3]);
		percInsert=atof(argv[4]);
		percDelete=atof(argv[5]);
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

void doOperations(){
	
	/*do operation based on value*/
	for(int i=0;i<m;i++){
		if(op_array[i]==1){
			Delete(rand()%65536);	
		}else if(op_array[i]==2){
			/*if insert was an already done one,do it again */
			int res=Insert(rand()%65536);
			while(!res){
				res=Insert(rand()%65536);			
			}
		}else if(op_array[i]==3){
			Member(rand()%65536);
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


