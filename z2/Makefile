all:
	mpicc -lm -o z2 letterCounter.c

run:
	mpirun -n 4 --use-hwthread-cpus ./z2
