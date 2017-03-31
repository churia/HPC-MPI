/* Communication ping-pong:
 * pass a 2MB message in a ring
 * 0 -> 1 -> 2 -> 3 -> .... -> 0
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include "util.h"
#define M 1024*1024
int main( int argc, char *argv[])
{
  int N = 1;//if not specified, default N=1
  int i,rank, size, tag, origin;
  short *message; 
  MPI_Status status;

  char hostname[1024];
  gethostname(hostname, 1024);
  tag = 99;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(argc==2){
    N=atoi(argv[1]);
  }

  message = (short *) malloc(sizeof(short)*M);
  for(i = 0; i < M; i++){
    message[i]=1;
  }

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for(i = 0; i < N; i++){
    if(rank !=0){
      origin = rank -1;
      MPI_Recv(&message,  1, MPI_SHORT, origin,      tag, MPI_COMM_WORLD, &status);
      printf("rank %d/%d hosted on %s received from %d \n", rank,size, hostname, origin);
    }	
    MPI_Send(&message, 1, MPI_SHORT, (rank+1)%size, tag, MPI_COMM_WORLD);
  
    if(rank == 0)
    {
      origin = size - 1;
      MPI_Recv(&message,  1, MPI_SHORT, origin,      tag, MPI_COMM_WORLD, &status);
      printf("rank %d/%d hosted on %s received from %d \n", rank,size, hostname, origin);
    }
  }

  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1,time2);
  if (mpirank == 0) {
    printf("Time elapsed is %f seconds.\n", elapsed);
    printf("%f communication/sec", elapsed/N/size);
    printf("%d: %f GB/s\n", sizeof(short), N*size*sizeof(short)/elapsed);
  }

  free(message)

  MPI_Finalize();
  return 0;
}
