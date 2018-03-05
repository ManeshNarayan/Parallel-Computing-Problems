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
#include <stdbool.h>
#include <mpi.h>





int main(int argc, char * argv[])
{	
	// Initialize MPI environment
	MPI_Init(&argc, &argv);

	 // Get the number of processes
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // Get the rank of the process
    int procID;
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);

	// Total number of webpages
	int no_of_nodes;

	// Iteration variables
	int i,j,dummy;
	
	// Threshold value for page rank change between iterations
	double epsilon = 0.00000001;

	// Damping coefficient
	double damping = 0.85;

	// Array for calculating the outgoing edges
	int * outgoing;

	// Pointers to page rank arrays
	double * curPR;
	double * prevPR;

	// Storage of original adjacency matrix
	int * buf;

	// Variables for checks
	int flag = 1;
	int cont;

	// File pointer 
	FILE * fp;

	//Get parameters
	if(procID == 0){

		// Dynamic allocation of Adjacency Matrix and loading
		fp = fopen("Data.txt","r+");
		fscanf(fp,"%d",&no_of_nodes);
		
	}

	// Broadcasts parameters
	MPI_Bcast(&no_of_nodes,1,MPI_INT,0,MPI_COMM_WORLD);	

	MPI_Barrier(MPI_COMM_WORLD);
	curPR = ( double*) malloc( sizeof( double) *ceil((double)no_of_nodes/(double)nprocs));
	prevPR = ( double*) malloc( sizeof( double) *no_of_nodes); 
	outgoing = ( int*) malloc( sizeof( int) * no_of_nodes);

	// Initialization
	for( i = 0; i < no_of_nodes; i++){
		outgoing[i] = 0;
		prevPR[i] = (double)1/(double)no_of_nodes;
	}

	// Master loads into memory
	if(procID == 0){
		buf = (int *)malloc( sizeof(int)*no_of_nodes*no_of_nodes);
		for(i = 0; i < no_of_nodes*no_of_nodes; i++){
				fscanf(fp,"%d",&dummy);
				buf[i] = dummy;
				outgoing[i%no_of_nodes] = outgoing[i%no_of_nodes] + buf[i];
		}
		fclose(fp);
	}

	// Broadcasts outgoing path count
	MPI_Bcast(outgoing,no_of_nodes, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	// Scatter Adjacency Matrix
	int * subMat = (int*) malloc ( sizeof(int) * ceil(((double)no_of_nodes/(double)nprocs)) * no_of_nodes);
	MPI_Scatter(buf,ceil(((double)no_of_nodes/(double)nprocs)) * no_of_nodes, MPI_INT, subMat,ceil(((double)no_of_nodes/(double)nprocs)) * no_of_nodes, MPI_INT, 0, MPI_COMM_WORLD);

	int iter;
	for( iter = 0; iter < 1000; iter++)
	{

		for( i = 0; i < ceil((double)no_of_nodes/(double)nprocs); i++){
			curPR[i] = 0;
		}
	
		//Computes PageRank for current iteration
		for(i = 0; i < ceil((double)no_of_nodes/(double)nprocs); i++){
			for(j = 0; j < no_of_nodes; j++){
				if(subMat[i*no_of_nodes + j] == 1){
						curPR[i] = curPR[i] + prevPR[j]/(double)outgoing[j];
				}
				else if(outgoing[j] == 0){
					curPR[i] = curPR[i] + prevPR[j]/(double)no_of_nodes;
				}
			}
			// Includes the damping factor and the subsequent random jump
			curPR[i] = curPR[i]*damping + (1 - damping)/(double)no_of_nodes;
		}
	
		// Checks for convergence of pagerank values in local process
		for( i = 0; i < ceil((double)no_of_nodes/(double)nprocs); i++)
		{	
			if( fabs( curPR[i] - prevPR[i + procID*(int)ceil((double)no_of_nodes/(double)nprocs)]) > epsilon){
				flag = 0;
				break;
			}
		}
		

		// Checks for convergence of pagerank values globally across processes
		MPI_Allreduce(&flag, &cont, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
		if(cont == 1){
			break;
		}

		//MPI_Gather(curPR,ceil((double)no_of_nodes/(double)nprocs),MPI_DOUBLE,prevPR, ceil((double)no_of_nodes/(double)nprocs), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		//MPI_Bcast(prevPR,no_of_nodes, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		// Gathers the current pageranks generated and sends them to all machines to act as prevPR for next iter.
		MPI_Allgather(curPR, ceil((double)no_of_nodes/(double)nprocs), MPI_DOUBLE, prevPR, ceil((double)no_of_nodes/(double)nprocs), MPI_DOUBLE, MPI_COMM_WORLD);

	}

	MPI_Finalize();
}