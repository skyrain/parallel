#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 17
#define N 256
#define RANGE 1000
#define EDGE 1

void print_matrice(double **matrice)
{
        int i,j;
        for(i = 0; i < N; i++)
        {
                for(j = 0; j < N; j++)
                {
                        printf("%f ", matrice[i][j]);
                }
                printf("\n");
        }
        printf("\n");
}

void sort(double array[], int len)
{	
	int i,j;
	for(i = 0; i < len - 1; i++)
	{
		for(j = i + 1; j < len; j++)
		{
			double tmp;
			if(array[j] < array[i])
			{
				tmp = array[i];
				array[i] = array[j];
				array[j] = tmp;
			}
		}		

	}
}


int main(int argc, char *argv[])
{
	double **B, **C, *tmp;
	int PID, p_size,i, j, k;
	int row_chunk, offset, chunk;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &p_size); 
	MPI_Comm_rank(MPI_COMM_WORLD, &PID);

	tmp = (double *) malloc (sizeof(double ) * N * N);
	B = (double **) malloc (sizeof(double *) * N);
	for (i = 0; i < N; i++)
		B[i] = &tmp[i * N];
	
	if (PID == 0) 
	{
		tmp = (double *) malloc (sizeof(double) * N * N);
		C = (double **) malloc (sizeof(double *) * N);
		for (i = 0; i < N; i++)
			C[i] = &tmp[i * N];

		//generate matrice B, if is master process
		for (i = 0; i < N; i++)
		 {
			for (j = 0; j < N; j++)
			 {
				B[i][j] = rand() % RANGE;
			}
		}
/*		//print matrice  B
		printf("matrice B: \n");
		print_matrice(B);
*/	}
	else 
	{
		tmp = (double *) malloc (sizeof(double) * N * N / p_size);
		C = (double **) malloc (sizeof(double *) * N / p_size);
		for (i = 0; i < N / p_size; i++)
			C[i] = &tmp[i * N];
	}

	row_chunk = N / p_size;
	//Broadcast B
	MPI_Bcast(B[0], N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if(PID == 0)
		chunk = N - (p_size -1) * row_chunk;	
	else
		chunk = row_chunk;

	//initialize C
	for (i = 0; i < chunk; i++) 
	{
		for (j = 0; j < N; j++)
		{
			C[i][j] = 0.0;
		}
	}



	
	//calculation
	if(PID == 0)
	{
		for (i = EDGE; i < chunk; i++) 
		{
			for (j = EDGE; j < N - EDGE; j++)
			{
				int array_len = (2 * EDGE + 1) * (2 * EDGE + 1);
				double array[array_len];
				int m, n;
				int c = 0;
				for(m = i - EDGE; m <= i + EDGE; m++)
				{
					for(n = j - EDGE; n <= j + EDGE; n++)
					{
						array[c] = B[m][n];
						c++;
					}
					
				}
				sort(array, array_len);
				C[i][j] = array[array_len / 2];
			}
		}
	}
	else if(PID == p_size - 1)
	{
		for (i = N - chunk; i < N - EDGE; i++) 
		{
			for (j = EDGE; j < N - EDGE; j++)
			{
				int array_len = (2 * EDGE + 1) * (2 * EDGE + 1);
				double array[array_len];
				int m, n;
				int c = 0;
				for(m = i - EDGE; m <= i + EDGE; m++)
				{
					for(n = j - EDGE; n <= j + EDGE; n++)
					{
						array[c] = B[m][n];
						c++;
					}
					
				}
				sort(array, array_len);
				C[i - (N - chunk)][j] = array[array_len / 2];
			}
		}
	}
	else
	{
		int s_i = N - (p_size - 1) * row_chunk + chunk * (PID -1);
		for (i = s_i; i < s_i + chunk; i++) 
		{
			for (j = EDGE; j < N - EDGE; j++)
			{
				int array_len = (2 * EDGE + 1) * (2 * EDGE + 1);
				double array[array_len];
				int m, n;
				int c = 0;
				for(m = i - EDGE; m <= i + EDGE; m++)
				{
					for(n = j - EDGE; n <= j + EDGE; n++)
					{
						array[c] = B[m][n];
						c++;
					}
					
				}
				sort(array, array_len);
				C[i - s_i][j] = array[array_len / 2];
			}
		}
	}	

	// master get result from slaves
	if (PID == 0) 
	{
		offset = chunk; 
		for (i = 1; i < p_size; i++) 
		{
			MPI_Recv(C[offset], row_chunk * N, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			offset += row_chunk;
		}
	}
	else
	{ 
		MPI_Send(C[0], row_chunk * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
	}
	// print result matrice C
	if (PID == 0)
	{
		//fill matrice C 's edge
		for(i = 0; i < N; i++)	
		{
			for(j = 0; j < N; j++)
			{
				if(i < EDGE || i >= N - EDGE || j < EDGE || j >= N - EDGE)
					C[i][j] = B[i][j];
			}
		}

		printf("matric C: \n");
		print_matrice(C);
	}

	MPI_Finalize();
	return 0;
}

