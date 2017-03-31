/* Example originally written by Lucas C Wilcox */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main( int argc, char *argv[])
{
  int rank, n,size;
  MPI_Status status;
  MPI_Request request_out, request_in;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int N = 10;

  int *message_out = calloc(N, sizeof(int));
  int *message_in = calloc(N, sizeof(int));

  for(n = 0; n < N; ++n)
    message_out[n] = rank;

  int tag = 99;
  int origin, destination;

  if(rank != 0)
  {
    destination = (rank + 1)%size;
    origin = rank - 1;
  }
  else
  {
    destination = rank + 1;
    origin = size - 1;
  }

  MPI_Irecv(message_in,  N, MPI_INT, origin,      tag, MPI_COMM_WORLD, &request_in);
  MPI_Isend(message_out, N, MPI_INT, destination, tag, MPI_COMM_WORLD, &request_out);

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

  //  printf("%d: %25.16g\n", rank, 4*x);
  }

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
   */
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
  printf("rank %d received from %d\n", rank,origin);
  /*
   * Wait for outgoing message
   */
  MPI_Wait(&request_out, &status);


  MPI_Finalize();
  return 0;
}
