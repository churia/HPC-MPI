/*non_blocking passing array in ring*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include "util.h"

int main( int argc, char *argv[])
{
  
  int N = 1;//if not specified, default N=1
  int M = 2*1e5;
  int rank, n,size;

  MPI_Status status;
  MPI_Request request_out, request_in;

  char hostname[1024];
  gethostname(hostname, 1024);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(argc==2){
    N=atoi(argv[1]);
  }

  long double *message_out = calloc(M, sizeof(long double));
  long double *message_in = calloc(M, sizeof(long double));

  for(n = 0; n < M; ++n)
    message_out[n] = rank;

  int tag = 99;
  int origin;

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for(n = 1; n <= N; ++n){
    if(rank != 0)
      origin = rank - 1;
    else
      origin = size - 1;

    MPI_Irecv(message_in,  M, MPI_LONG_DOUBLE, origin,      tag, MPI_COMM_WORLD, &request_in);
    MPI_Isend(message_out, M, MPI_LONG_DOUBLE, (rank + 1)%size, tag, MPI_COMM_WORLD, &request_out);

    /*
     * Wait for incomming message
     */
    MPI_Wait(&request_in, &status);
    
    if(n % 10 == 0)
    	printf("Iter %d/%d: rank %d/%d hosted on %s received from %d\n", n,N, rank,size,hostname,origin);
    /*
     * Wait for outgoing message
     */
    MPI_Wait(&request_out, &status);

  }
  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1,time2);
  if (rank == 0) {
    printf("Time elapsed is %f seconds.\n", elapsed);
    printf("Latency: %f comm/sec\n", elapsed/N/size);
    printf("Bandwidth: %f MB/s\n", N*size*M*sizeof(long double)/1e6/elapsed);
  }

  free(message_in);
  free(message_out);
  MPI_Finalize();
  return 0;
}
