/* Communication ping-pong:
 * Exchange between messages between mpirank
 * 0 <-> 1, 2 <-> 3, ....
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int main( int argc, char *argv[])
{
  int N = 1;//if not specified, default N=1
  int i,rank, size, tag, origin;
  MPI_Status status;

  char hostname[1024];
  gethostname(hostname, 1024);
  tag = 99;
  char message[2048]; 

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(argc==2){
    N=atoi(argv[1]);
  }

  for(i=0;i<N;i++){
    if(rank !=0){
      origin = rank -1;
      MPI_Recv(&message,  1, MPI_BYTE, origin,      tag, MPI_COMM_WORLD, &status);
      printf("rank %d/%d hosted on %s received from %d \n", rank,size, hostname, origin);
    }	
    MPI_Send(&message, 1, MPI_BYTE, (rank+1)%size, tag, MPI_COMM_WORLD);
  
    if(rank == 0)
    {
      origin = size - 1;
      MPI_Recv(&message,  1, MPI_BYTE, origin,      tag, MPI_COMM_WORLD, &status);
      printf("rank %d/%d hosted on %s received from %d \n", rank,size, hostname, origin);
    }
  }

  MPI_Finalize();
  return 0;
}
