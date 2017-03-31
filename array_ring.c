/* Communication ping-pong:
 * pass a 2MB message in a ring
 * 0 -> 1 -> 2 -> 3 -> .... -> 0
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include "util.h"

int main( int argc, char *argv[])
{
  int N = 1;//if not specified, default N=1
  int M = 10;
  int i,rank, size, tag, origin;
  double *message; 
  MPI_Request request_out, request_in;
  MPI_Status status;

  char hostname[1024];
  gethostname(hostname, 1024);
  tag = 99;

  //initialize array
  message = calloc(M,sizeof(double));
  for(i=0;i<M;i++){
    message[i]=rank;
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(argc==2){
    N=atoi(argv[1]);
  }

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for(i = 0; i < N; i++){
    if(rank !=0){
      origin = rank -1;
      MPI_Recv(&message,  M, MPI_DOUBLE, origin,      tag, MPI_COMM_WORLD, &status);
      printf("rank %d/%d hosted on %s received array from %d \n", rank,size, hostname, origin);
    }	
    MPI_Send(&message, M, MPI_DOUBLE, (rank+1)%size, tag, MPI_COMM_WORLD);
  
    if(rank == 0)
    {
      origin = size - 1;
      MPI_Recv(&message,  M, MPI_DOUBLE, origin,      tag, MPI_COMM_WORLD, &status);
      printf("rank %d/%d hosted on %s received array from %d \n", rank,size, hostname, origin);
    }
  }

  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1,time2);
  if (rank == 0) {
    printf("Time elapsed is %f seconds.\n", elapsed);
    printf("Latency: %f comm/sec.\n", elapsed/N/size);
    printf("Bandwidth: %f MB/s\n",  N*size*2/elapsed);
    free(message);
  }

  MPI_Finalize();
  return 0;
}
