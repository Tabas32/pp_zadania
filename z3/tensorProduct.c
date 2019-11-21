#include <mpi.h>
#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include <string.h>
#include <errno.h>


void printMatrix(int** mat, int h, int w);
int** scalarProduct(int**, int, int, int);
void fillLoc(int* loc, int start, int end, int val);
int** alloc2DMatrix(int rows, int cols);
void free2DMatrix(int** mat);
void matrixIntegration(int** MAT1, int** mat2, int R, int C, int w, int h);

int main(int argc, char** argv) {
	int n = 3;
	int** X = (int**) malloc(n*sizeof(int*));
	int** Y = (int**) malloc(n*sizeof(int*));

	for(int i = 0; i < n; i++) {
		X[i] = (int*) malloc(n*sizeof(int));
		Y[i] = (int*) malloc(n*sizeof(int));
		for(int j = 0; j < n; j++) {
			X[i][j] = i*n + j;
			Y[i][j] = n+1+j;
		}
	}

	
//===================================================================

	int size, rank;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

	if(size <= 1) {
		perror("Not enought processes");
		return 1;
	}
	if(rank == 0) {
		int inputL = n * n;
		int chunkL = (int)ceil((double)inputL/ (size - 1));
		int* calcLoc = (int*) malloc(inputL * sizeof(int));

		for(int i = 1; i < size; i++) {
			int b = (i-1) * chunkL;
			int e = i * chunkL - 1;
			e = e < inputL ? e : inputL - 1;

			int info[4] = {b / n, b % n, e / n, e % n};
			fillLoc(calcLoc, info[0] * n + info[1], info[2] * n + info[3], i);
			MPI_Send(info, 4, MPI_INT, i, 1, MPI_COMM_WORLD);
		}

		int** resultMatrix = alloc2DMatrix(inputL, inputL);
		for(int i = 0; i < inputL; i++) {
			int** recvMat = alloc2DMatrix(n,n);
			MPI_Recv(&(recvMat[0][0]), inputL, MPI_INT, calcLoc[i], i, MPI_COMM_WORLD, &status);
			matrixIntegration(resultMatrix, recvMat, (i / n) * n, (i % n) * n, n, n);
			free2DMatrix(recvMat);
		}

		printMatrix(X, n, n);
		printf("\n----------------------\n");
		printMatrix(Y, n, n);
		printf("\n----------------------\n");
		printMatrix(resultMatrix, inputL, inputL);
		printf("\n");

		free2DMatrix(resultMatrix);
		free(calcLoc);

	} else {
		int info[4];	
		MPI_Recv(info, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		for(int i = info[0]; i <= info[2]; i++) {
			int j = i == info[0] ? info[1] : 0;
			int jEnd = i == info[2] ? info[3] : n - 1;
			for(j; j <= jEnd; j++) {
				int** sProduct = scalarProduct(Y, n, n, X[i][j]);
				int flag = i * n + j;
				MPI_Send(&(sProduct[0][0]), n*n, MPI_INT, 0, flag, MPI_COMM_WORLD);
				free2DMatrix(sProduct);
			}
		}

	}

	MPI_Finalize();
	
	return 0;
}

int** scalarProduct(int** mat, int matH, int matW, int scal) {
	int** retMat = alloc2DMatrix(matH, matW);
	for(int i = 0; i < matH; i++) {
		for(int j = 0; j < matW; j++) {
			retMat[i][j] = mat[i][j] * scal;
		}
	}

	return retMat;
}

void fillLoc(int* loc, int start, int end, int val) {
	for(int i = start; i <= end; i++) {
		loc[i] = val;
	}
}

int** alloc2DMatrix(int rows, int cols) {
	int* data = (int*) malloc(rows * cols * sizeof(int));
	int** array = (int**) malloc(rows * sizeof(int*));
	for(int i = 0; i < rows; i++) {
		array[i] = &(data[cols * i]);
	}
	return array;
}

void free2DMatrix(int** mat) {
	free(mat[0]);
	free(mat);
}

void printMatrix(int** mat, int h, int w) {
	printf("[");
	for(int i = 0; i < h; i++) {
		printf("[");
		for(int j = 0; j < w; j++) {
			printf("%d,", mat[i][j]);
		}
		printf("],");
	}
	printf("]");
}

void matrixIntegration(int** MAT1, int** mat2, int R, int C, int w, int h) {
	for(int i = 0; i < w; i++) {
		for(int j = 0; j < h; j++) {
			MAT1[i + R][j + C] = mat2[i][j];
		}
	}
}
