/* Communication ring with non_blocking communication:
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
      MPI_Irecv(&message,  M, MPI_DOUBLE, origin,      tag, MPI_COMM_WORLD, &request_in);
      printf("rank %d/%d hosted on %s received array from %d \n", rank,size, hostname, origin);
    }
	
    MPI_Isend(&message, M, MPI_DOUBLE, (rank+1)%size, tag, MPI_COMM_WORLD, &request_out);
  
    if(rank == 0)
    {
      origin = size - 1;
      MPI_Irecv(&message,  M, MPI_DOUBLE, origin,      tag, MPI_COMM_WORLD, &request_in);
      printf("rank %d/%d hosted on %s received array from %d \n", rank,size, hostname, origin);
    }
  }
  
  /*
   * Do work
   */
  {
    int i;
    double x = 0;

    for(i = 0; i < 100000; ++i)
    {
      int sign = (i%2) ? -1 : 1;
      x += sign * 1.0/(2*i+1);
    }

    //printf("%d: %25.16g\n", rank, 4*x);
  }

  /*
   * Wait for incomming message
   */
  MPI_Wait(&request_in, &status);
  
/*
   * Write output to a file
   */
  {
    FILE* fd = NULL;
    char filename[256];
    snprintf(filename, 256, "output%02d.txt", rank);
    fd = fopen(filename,"w+");

    if(NULL == fd)
    {
      printf("Error opening file \n");
      return 1;
    }

    fprintf(fd, "rank %d received from %d the message:\n", rank, origin);
    for(n = 0; n < N; ++n)
      fprintf(fd, "  %d\n", message_in[n]);

    fclose(fd);
  }

  /*
   * Wait for outgoing message
   */
  MPI_Wait(&request_out, &status);

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
