/* 
 * Transmit a message in a 3-process system. 
 */ 
#include <mpi.h>
#include <stdio.h>

#define BUFSIZE 10
int main(int argc, char** argv){
{ 
// Definicia pomocnych premennych ====================================
  int size, rank;
  int slave;
  int buf[BUFSIZE];
  int n, value;
  float rval;
  MPI_Status status;

// Inicializacia MPI =================================================
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);

// Kontrola poctu procesov ===========================================
if (size==3) {/* Correct number of processes */}

  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

// Samotne programy ==================================================
/*
	* Rank 0 = Master
	* ostatne ranky = Slaves
*/
if (rank==0) { // Master proces ======================================
  // Hodnoty pre vypocty
  buf[0]=5; buf[1]=1; buf[2]=8; buf[3]=7; buf[4]=6; 
  buf[5]=5; buf[6]=4; buf[7]=2; buf[8]=3; buf[9]=1;

  // Odosielanie hodnot na ostatne procesy
  printf("\n Sending the values {5,1,8,7,6,5,4,2,3,1}"); 
  printf("\n -----------------------------"); 
  for (slave=1;slave < size;slave++) {
    printf("\n from master %d to slave %d",rank,slave); 
    MPI_Send(buf, 10, MPI_INT, slave, 1, MPI_COMM_WORLD);
  }

  // Spracovanie vysledkov
  printf("\n\n Receiving the results from slaves"); 
  printf("\n ---------------------------------"); 
  MPI_Recv(&value, 1, MPI_INT, 1, 11, MPI_COMM_WORLD, &status);
  printf("\n Minimum %4d from slave 1",value); 
  MPI_Recv(&value, 1, MPI_INT, 2, 21, MPI_COMM_WORLD, &status);
  printf("\n Sum     %4d from slave 2",value); 
  MPI_Recv(&value, 1, MPI_INT, 1, 12, MPI_COMM_WORLD, &status);
  printf("\n Maximum %4d from slave 1",value); 
  MPI_Recv(&rval, 1, MPI_FLOAT, 2, 22, MPI_COMM_WORLD, &status);
  printf("\n Average %4.2f from slave 2\n",rval); 

} else {
  if (rank==1) { // Min/Max proces ===================================
	// Cakanie na hodnoty od mastra
    MPI_Recv(buf, 10, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

	// Vypocet minima
    value=100; 
    for (n=0;n<BUFSIZE;n++) {
      if (value>buf[n]) { value=buf[n]; }
    }   
    MPI_Send(&value, 1, MPI_INT, 0, 11, MPI_COMM_WORLD);

	// Vypocet maxima
    value=0; 
    for (n=0;n<BUFSIZE;n++) {
      if (value<buf[n]) { value=buf[n]; }
    }   
    MPI_Send(&value, 1, MPI_INT, 0, 12, MPI_COMM_WORLD);

  } else { // Sum/Avg proces =========================================
	// Hodnoty od mastra
    MPI_Recv(buf, 10, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

	// Vypocet Sum
    value=0; 
    for (n=0;n<BUFSIZE;n++) {
      value=value+buf[n]; 
    }   
    MPI_Send(&value, 1, MPI_INT, 0, 21, MPI_COMM_WORLD);

	// Vypocet Avg
    rval= (float) value / BUFSIZE; 
    MPI_Send(&rval, 1, MPI_FLOAT, 0, 22, MPI_COMM_WORLD);
  } 
}
}

// Ukoncenie MPI =====================================================
MPI_Finalize();
return(0);
}
