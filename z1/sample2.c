/* 
 * Collective communication in a 4-process system. 
 */ 
#include <mpi.h>
#include <stdio.h>
#define BUFSIZE 10

int main(int argc, char** argv)
{ 
// Definicia pomocnych premennych ====================================
  int size, rank;
  int buf[BUFSIZE]={0,0,0,0,0,0,0,0,0,0};
  int n, value;
  float rval;
  MPI_Status status;

// Inicializacia MPI =================================================
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);

// Kontrola poctu procesov ===========================================
if (size==4) { /* Correct number of processes */

  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

// Rozposlanie hodnot na vsetky procesy ==============================
if (rank==0) {
  buf[0]=5; buf[1]=1; buf[2]=8; buf[3]=7; buf[4]=6; 
  buf[5]=5; buf[6]=4; buf[7]=2; buf[8]=3; buf[9]=1;
  printf("\n Broadcasting {5,1,8,7,6,5,4,2,3,1}"); 
  printf("\n ---------------------------------------------");
}
 
  MPI_Bcast(buf,10,MPI_INT,0,MPI_COMM_WORLD);

// Samotne programy ==================================================
if (rank==0) { // Master proces ======================================
  // Vypocet minima
  printf("\n Computing by master and slaves"); 
    value=100; 
    for (n=0;n<BUFSIZE;n++) {
      if (value>buf[n]) { value=buf[n]; }
    }   

  // Spracovanie vysledkov
  printf("\n Minimum %4d  by  master ",value); 
  MPI_Recv(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
  printf("\n Maximum %4d from slave 1",value); 
  MPI_Recv(&value, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
  printf("\n Sum     %4d from slave 2",value); 
  MPI_Recv(&rval, 1, MPI_FLOAT, 3, 0, MPI_COMM_WORLD, &status);
  printf("\n Average %4.2f from slave 3\n",rval); 
 
} else if (rank==1) { // Vypocet Maxima ==============================
    value=0; 
    for (n=0;n<BUFSIZE;n++) {
      if (value<buf[n]) { value=buf[n]; }
    }   
    MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
} else if (rank==2) { // Vypocet Sum =================================
    value=0; 
    for (n=0;n<BUFSIZE;n++) {
      value=value+buf[n]; 
    }   
/* send sum to master */
    MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
/* send sum to slave 3 */
    MPI_Send(&value, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
} else if (rank==3) { // Vypocet Avg =================================
    MPI_Recv(&value, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
    rval= (float) value / BUFSIZE; 
    MPI_Send(&rval, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
}
} 

// Ukoncenie MPI =====================================================
MPI_Finalize();
return(0);

}
