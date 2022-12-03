/*
Created by Marc Bolinches
*/

#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <mpi.h>

#ifdef _DOUBLE_
#define Ffloat double
#else
#define Ffloat float
#endif

#ifndef SIMD_LENGTH
#define SIMD_LENGTH 4
#endif

void copy2file(Ffloat *a, int N);

using namespace std;

int main(int narg, char **argv)
{
  // initialize variables
  size_t Narray = 1;

  // mpi vars
  int rank, worldSize;
  MPI_Init(&narg, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  // read imput
  if( narg < 2)
  {
    cout<< "Wrong number of arguments. Use as follows: " << endl;
    cout<< "    triad.p arrayLength " << endl;
    cout<< "where " << std::endl;
    cout << "  arrayLength is the length of the array to be used" << endl;
    return 1;
  }else 
  {
    Narray = stoi(argv[1]);
  }

  // create and allocate arrays
  Ffloat **a  = new Ffloat*[Narray];
  Ffloat **b  = new Ffloat*[Narray];
  Ffloat **c  = new Ffloat*[Narray];

  for( size_t i = 0; i < Narray; i++)
  {
    a[i] = new Ffloat[Narray];
    b[i] = new Ffloat[Narray];
    c[i] = new Ffloat[Narray];
  }

  // initialize arrays (this is done in an optimal way)
  for( size_t i = 0; i < Narray; i++)
    for( size_t j = 0; j < Narray; j++)
    {
      a[i][j] = rand();
      b[i][j] = rand();
      c[i][j] = rand();
    }

  // repeat excercise "repeat" times and average times
  int repeat = 20;
  double avgTime = 0.;
  for(int n = 0; n < repeat; n++)
  {
    double time = MPI_Wtime();
    
    // operations that need to be measured
    for( size_t i = 0; i < Narray; i+=4)
      for(size_t j = 0; j < Narray; j++)
      {
        Ffloat tmp = 0.;
        #pragma omp simd
        for(size_t k = 0; k < SIMD_LENGTH; k++)
        for(size_t ii = 0; ii < Narray; ii+=SIMD_LENGTH)
          tmp += a[i][ii+k]*b[ii+k][j];
        c[i][j] = tmp;
      }

    avgTime += MPI_Wtime() - time;

    copy2file(c[0], Narray);

    // change arrays
    for( size_t i = 0; i < Narray; i++)
      for( size_t j = 0; j < Narray; j++)
      {
        a[i][j] = rand();
        b[i][j] = rand();
        c[i][j] = rand();
      }
  }

  avgTime /= double(repeat);

  if( rank == 0 )
    cout << "GFlops/sec " << (double(Narray*Narray*Narray*Narray + Narray*Narray*Narray))/avgTime*1e-6 << endl;

  for( size_t i = 0; i < Narray; i++)
    delete[] a[i], b[i], c[i];
  delete[] a, b, c;

  MPI_Finalize();
  return 0;
}


/*
Auxiliary functions
*/
void copy2file(Ffloat *a, int N)
{
  ofstream file;
  file.open("dummyFile.txt");
  
  for(int i = 0; i < N; i+=10)
  {
    file << a[i] << endl;
  }

  file.close();
}
