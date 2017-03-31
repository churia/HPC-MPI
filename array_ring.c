/* Example originally written by Lucas C Wilcox */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main( int argc, char *argv[])
{
  
  int N = 1;//if not specified, default N=1
  int M = 10;
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

  int *message_out = calloc(M, sizeof(int));
  int *message_in = calloc(M, sizeof(int));

  for(n = 0; n < M; ++n)
    message_out[n] = rank;

  int tag = 99;
  int origin;

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for(n = 0; n < N; ++n){
    if(rank != 0)
      origin = rank - 1;
    else
      origin = size - 1;

    MPI_Irecv(message_in,  M, MPI_INT, origin,      tag, MPI_COMM_WORLD, &request_in);
    MPI_Isend(message_out, M, MPI_INT, (rank + 1)%size, tag, MPI_COMM_WORLD, &request_out);

    /*
     * Do work
    
    {
      int i;
      double x = 0;

      for(i = 0; i < 100000; ++i)
      {
        int sign = (i%2) ? -1 : 1;
        x += sign * 1.0/(2*i+1);
      }

    //  printf("%d: %25.16g\n", rank, 4*x);
    }
   */
    /*
     * Wait for incomming message
     */
    MPI_Wait(&request_in, &status);


    /*
     * Write output to a file
     
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
    }*/
    /*
     * Do work
    
    {
      int i;
      double x = 0;

      for(i = 0; i < 100000; ++i)
      {
        int sign = (i%2) ? -1 : 1;
        x += sign * 1.0/(2*i+1);
      }

    //  printf("%d: %25.16g\n", rank, 4*x);
    } */
    printf("rank %d received from %d\n", rank,origin);
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
  }
  MPI_Finalize();
  return 0;
}
