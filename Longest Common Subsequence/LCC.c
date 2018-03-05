/*
** Submission for Exercise 1, Assignment-1
** Parallel Computing CS F422
** Author Names : Manesh Narayan K (2013B1A7542P), Kunal Todi (2013B3A7)
** Library used : OMP
**
**
** Assumptions : Input files don't contain more than 200 lines
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>


#define DIAG 0
#define UP 1
#define LEFT 2



// Record to hold the contents of a file
struct record{
//	int line[1000];
	char* str[200];
	int size;
};

// Holds the K largest LCS values and their corresponding file names
struct set{
	int lcsLen;
	int i;
	int j;
};
struct set* Kset;

// Pointer to the variable storing the delimiter.
// Can be set to newline
char delim = '.';

// Holds the input parameters
int N;
int K;


// Populates a record rec with the lines in "File"i".txt"
void populate(struct record* rec, int i){

	
	int j;
	for( j = 0; j < 200; j++){
//		rec->line[j] = -1;
		rec->str[j] = NULL;
		rec->size = 0;
	}
	
	// Opening the file to be compared
	FILE* fp;
//	int filepos = 0;
	char buff[500];
	char filename[10];
	sprintf( filename, "File%i.txt", i);
	fp = fopen( filename, "r");
	
	// Loading lines from file
	while( ( fgets( buff, 500, fp)) != NULL){

//		rec->line[filepos] = filepos;
		rec->str[rec->size] = ( char*) malloc( strlen(buff) * sizeof( char) );

		strcpy( rec->str[rec->size], buff);
		rec->size++;
	

	}
	return;

}


void printLCS(int* Dir, struct record r1, int i, int j, int ncol){

	if( i == 0 || j == 0){
		return;
	}
	if( Dir[i*(ncol+1) + j] == DIAG){
		printLCS(Dir, r1, i-1, j-1, ncol);
		printf("\n%s", r1.str[i-1]);
	}
	else{
		if( Dir[i*(ncol+1) + j] == UP){
			printLCS(Dir, r1, i-1, j, ncol);
		}	
		else{
			printLCS(Dir, r1, i, j-1, ncol);
		}
	}
	return;

}

// Computes the length of LCS and returns it. If print is set to 1, it also prints the LCS
int lcs(struct record r1, struct record r2, int print){

	int i,j;
	int nrow = r1.size;
	int ncol = r2.size;
	int max = 0;


	// Array for storing the computation of LCS
	int* Arr = ( int*) malloc( ( nrow + 1) * ( ncol + 1) * sizeof( int));
	int* Dir = ( int*) malloc( ( nrow + 1) * ( ncol + 1) * sizeof( int));

	// Initializing border values
	for( i = 0; i < nrow + 1; i++){

		Arr[i*(ncol+1)] = 0;
		
	}
	for( i = 0; i < ncol + 1; i++){

		Arr[i] = 0;
		
	}

	for( i = 1; i < nrow + 1; i++){
		
		for( j = 1; j < ncol + 1; j++){

			if( strcmp( r1.str[i-1], r2.str[j-1]) == 0){	// i-1 and j-1 because these are meant for Arr which is padded
				// Match found. Incrementing and updating value
				Arr[i*(ncol+1) + j] = Arr[(i-1)*(ncol+1) + (j-1)] + 1;
				Dir[i*(ncol+1) + j] = DIAG;
				//printf("Match %d %d %d %s %s\n", i, j, Dir[i*(ncol+1) + j], r1.str[i-1], r2.str[j-1]);
			}

			// Match not found. Copying value from above/left
			else{
				// Copy from left
				if( Arr[i*(ncol+1) + j-1] > Arr[(i-1)*(ncol+1) + j]){
					Arr[i*(ncol+1) + j] = Arr[i*(ncol+1) + j-1];
					Dir[i*(ncol+1) + j] = LEFT;
					//printf("Left %d %d %d\n", i, j, Dir[i*(ncol+1) + j]);
				}
				// Copy from above
				else{
					Arr[i*(ncol+1) + j] = Arr[(i-1)*(ncol+1) + j];	
					Dir[i*(ncol+1) + j] = UP;
					//printf("Up %d %d %d\n", i, j, Dir[i*(ncol+1) + j]);
				}
			}

		}

	}

	if(print == 1){
		printLCS(Dir, r1, nrow, ncol, ncol);
		//printf("\n");
	}

	max = Arr[ ((nrow+1)*(ncol+1)) - 1];

	return max;
}


// Checks if the array is empty
int getmin(){
	int i = 0;
	int min = Kset[0].lcsLen;
	int pos = 0;
	while(i < K){
		if( Kset[i].lcsLen == -1){
			return i;
		}
		if( Kset[i].lcsLen < min){
			min = Kset[i].lcsLen;
			pos = i;
		}
		i++;
	}
	return pos;
}



void sort(struct record* r){
	int i, j;
	char temp[500];
	//printf("Sort\n");
	for (i = 0; i < r->size - 1 ; i++){
		for (j = i + 1; j < r->size; j++){
            if (strcmp(r->str[i], r->str[j]) > 0){
                strcpy(temp, r->str[i]);
                strcpy(r->str[i], r->str[j]);
                strcpy(r->str[j], temp);
            }
        }
    }
}

void intersection(struct record* r1, struct record* r2){
	int i,j;
	i = 0;
	j = 0;
	while(( i < r1->size) && (j < r2->size)){
		//printf("Intersection\n");
		if (strcmp(r1->str[i], r2->str[j]) == 0){
			i++;
			j++;
		}
		else if(strcmp(r1->str[i], r2->str[j]) > 0){
			j++;
		}
		else{
			strcpy(r1->str[i],"~");
			r1->size--;
			i++;
		}
	}

}



void main(int argc,char* argv[]){

	//Exit condition
	if(argc!=3){
		
		printf("Incorrect argument number\n");
		return;

	}

	N = atoi(argv[1]);
	K = atoi(argv[2]);

	// List to hold the files to be compared finally
	int* Filelist = ( int*) malloc( (2*K+1)*sizeof(int));
	memset(Filelist,'\0',(2*K+1));
	
	// Variables for loop iterations
	int i, j;

	// To hold the thread ID
	int tid;
	
	// Structure to hold the k sets
	Kset = ( struct set*) malloc( K * sizeof(struct set));
	for( i = 0; i < K; i++){
		Kset[i].lcsLen = -1;
	}

	// Holds the file read
	//////////////////
	struct record fr1;
	struct record fr2;

	// Loop to iterate through the files. To be replaced in parallel
	for(int i = 0; i < N; i++){
		for(int j = i + 1; j < N; j++){
					
			populate(&fr1,i);
			populate(&fr2,j);
			int lcsLen = 0;
			//printf("\n\n%d %d\n", i, j);
			lcsLen = lcs( fr1, fr2, 0);
			struct set temp;
			temp.lcsLen = lcsLen;
			temp.i = i;
			temp.j = j;

			/////////////////////////
			// Put a lock here in parallel
			int k = getmin();
			if(Kset[k].lcsLen < temp.lcsLen){
				Kset[k] = temp;
			}
			// Close lock
			/////////////////////////


		}
	}

	int noFiles;
	// Loop to retain the file numbers in the top K pairs
	for( i = 0; i < K; i++){
		int flag1=0;
		int flag2=0;
		for( j = 0; Filelist[j] != '\0' ; j++){
			if(Filelist[j] == Kset[i].i){
				flag1 =1;
			}
			if(Filelist[j] == Kset[i].j){
				flag2 =1;
			}
		}
		if( flag1 == 0){
			noFiles = j;
			Filelist[j++] = Kset[i].i;
		}
		if( flag2 == 0){
			noFiles = j;
			Filelist[j++] = Kset[i].j;
		}

	}

	// Loop to get LCS for top K' files
	// To be included in parallel
	for(i = 0; i <= noFiles; i++){
		for(j = i + 1; j <= noFiles; j++){
			populate(&fr1,Filelist[i]);
			populate(&fr2,Filelist[j]);
			int lcsLen = 0;
			//printf("\n%d %d",i,j);
			lcsLen = lcs( fr1, fr2, 1);

		}
	}

	
	struct record* combine = (struct record*) malloc( (noFiles/2 + 1)*sizeof(struct record));

	// To be included in parallel
	for(j = 0; j <= noFiles/2; j++){
		populate(&fr1,Filelist[j]);
		populate(&fr2,Filelist[noFiles - j]);
		sort(&fr1);
		sort(&fr2);
		intersection(&fr1,&fr2);
		combine[j] = fr1;
	}

	noFiles = noFiles/2;
	// To be included in parallel
	for(i = 0; noFiles != 1; i = i++){
		for( j = 0; j <= noFiles/2; j++){
			fr1 = combine[j];
			fr2 = combine[noFiles-j];
			intersection(&fr1,&fr2);
			combine[j] = fr1;
		}
		noFiles = noFiles/2;
	}


	return;
}