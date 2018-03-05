all:
	mpicc pagerank.c -lm -o pagerank
	time mpirun -np 1 ./pagerank
	time mpirun -np 2 ./pagerank
	time mpirun -np 4 ./pagerank


clean:
	rm -fv ./pagerank
