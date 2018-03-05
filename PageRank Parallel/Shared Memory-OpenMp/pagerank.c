/*
// Parallel Computing CS F422
// Submission for assignment 2
//
// Implementation of PageRank algorithm
//
// Authors - 
//		Manesh Narayan K (2013B1A7542P)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#define MAX_THREADS 8

int main()
{	

	////// The text file will have the number of webpages/nodes as first entry
	int no_of_nodes;
	scanf("%d ",&no_of_nodes);

	////// Dynamic allocation of Adjacency Matrix
	int** graph;
	graph=(int**)malloc(no_of_nodes*sizeof(int*));
	int x;
	for(x=0;x<no_of_nodes;x++)
	{
		graph[x]=(int*)malloc(no_of_nodes*sizeof(int));
	}

	// Iterations
	int i,j;
	
	////// Read Adjacency Matrix
	for(i=0;i<no_of_nodes;i++)
	{
		for(j=0;j<no_of_nodes;j++)
		{
			scanf("%d ",&graph[i][j]);
		}
	}

	// Threshold value for page rank change between iterations
	double epsilon = 0.0001;

	// Damping coefficient
	double damping = 0.85;

	//

	// Pointers to page rank arrays
	double * curPR = ( double*) malloc( sizeof( double) * no_of_nodes);
	double * prevPR = ( double*) malloc( sizeof( double) * no_of_nodes); 
	int * outgoing = ( int*) malloc( sizeof( int) * no_of_nodes);

	// Initializing the PageRank arrays
	for( i = 0; i < no_of_nodes; i++){
		outgoing[i] = 0;
		prevPR[i] = (double)1/(double)no_of_nodes;
	}
	omp_set_num_threads(MAX_THREADS);
	// Calculates the outgoing links
	#pragma omp parallel for shared(outgoing,no_of_nodes,graph,j) private(i)
	for( j = 0; j < no_of_nodes; j++){
		for(i = 0; i < no_of_nodes; i++){
			outgoing[j] = outgoing[j] + graph[i][j];
		}
	}

	int iter;
	for( iter = 0; iter < 500; iter++)
	{
		for( i = 0; i < no_of_nodes; i++){
			curPR[i] = 0;
		}
		//pageRank(curPR,prevPR);
		//////////
		//Computes PageRank for current iteration
		omp_set_num_threads(MAX_THREADS);
		#pragma omp parallel for shared(curPR,prevPR,i,no_of_nodes,graph,epsilon,damping,outgoing) private(j)
		for(i = 0; i < no_of_nodes; i++){
			for(j = 0; j < no_of_nodes; j++){
				if(graph[i][j] == 1){
						curPR[i] = curPR[i] + prevPR[j]/(double)outgoing[j];
				}
				else if(outgoing[j] == 0){
					curPR[i] = curPR[i] + prevPR[j]/(double)no_of_nodes;
				}
			}
			curPR[i] = curPR[i]*damping + (1 - damping)/(double)no_of_nodes;
		}

		//////////
		int flag=0;
		omp_set_num_threads(MAX_THREADS);
		#pragma omp parallel for shared(curPR,prevPR,i,no_of_nodes,epsilon,flag)
		for( i = 0; i < no_of_nodes; i++)
		{	
			//printf("%f\n", fabs(curPR[i] - prevPR[i]));
			if( fabs( curPR[i] - prevPR[i]) > epsilon)
			{
				flag=1;
				//break;
			}
		}
		if(flag==0)
			break;
		// if( i >= no_of_nodes){
		// 	break;
		// }
				
		// Switches the current pagerank and previous pagerank array for next iteration
		double * temp;
		temp = curPR;
		curPR = prevPR;
		prevPR = temp;
	}
	// for(i=0;i<no_of_nodes;i++)
	// 	{
	// 		printf("%f\n",curPR[i]);
	// 	}
	// printf("\nDone %d\n",iter);

}