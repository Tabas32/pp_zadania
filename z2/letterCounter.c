#include <mpi.h>
#include <stdio.h> 
#include <math.h>
#include <string.h>
#include <errno.h>

void fillWithSpaces(int, char*);
int countChar(char, int, char*);

int main(int argc, char** argv) {
// Inicializacia premennych ==========================================
	int size, rank;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

// Kontrola poctu procesov ===========================================
	if(size <= 1) {
		perror("Not enought processes");
		return 1;
	}

	if(rank == 0) { // Master ========================================
		// Vstup hladaneho znaku
		char search;
		printf("Character to serach for:\n");
		scanf("%c", &search);

		// Vstup velkosti retazca
		int inputLength;
		printf("Input size:\n");
		scanf("%d", &inputLength);
		int chunkLength = (int)ceil((double)inputLength / (size - 1));

		// Vstup retazca
		inputLength = chunkLength * (size - 1);
		char inputString[inputLength];
		fillWithSpaces(inputLength, inputString);
		printf("Input string:\n");
		for(int i = 0; i < inputLength; i++) {
			inputString[i] = getchar();
		}
		//scanf("%s", inputString);

		// Rozdelenie retazca a odoslanie na ostatne procesy
		for(int i = 1; i < size; i++) {
			char buff[chunkLength];
			memcpy(buff, &inputString[(i - 1) * chunkLength], chunkLength);
			MPI_Send(&chunkLength, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&search, 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
			MPI_Send(buff, chunkLength, MPI_CHAR, i, 3, MPI_COMM_WORLD);
		}

		// Spracovanie vysledkov z ostatnych procesov
		int count = 0;
		for(int i = 1; i < size; i++) {
			int tempCount;
			MPI_Recv(&tempCount, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			count += tempCount;
		}

		printf("Char %c is in input string %d times\n", search, count);

	} else { // Slaves ===============================================
		// Vstupy
		int strLength;	
		MPI_Recv(&strLength, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		char c;
		MPI_Recv(&c, 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);

		char str[strLength];
		MPI_Recv(str, strLength, MPI_CHAR, 0, 3, MPI_COMM_WORLD, &status);

		// Vypocet a odoslanie na mastra
		int count = countChar(c, strLength, str);	
		MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

// Pomocna f-cia na inicializaciu retazca
// Naplni retazec str, velkosti size, znakom medzery
void fillWithSpaces(int size, char* str) {
	for (int i = 0; i < size; i++) {
		str[i] = ' ';
	}
}


// Vypocet vyskytu znaku c v retazci str o velkosit strLen
int countChar(char c, int strLen, char* str) {
	int counter = 0;
	for(int i = 0; i < strLen; i++) {
		if(str[i] == c) counter++;
	}
	return counter;
}
