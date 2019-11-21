all:
	mpicc -lm -o z3 tensorProduct.c

run:
	mpirun -n 4 --use-hwthread-cpus ./z3
