#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define LEN 1024
#define RANGE 1000
#define EDGE 2
#define chunk_size 4

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



void print_matrice(double[], int, int);

int main(int argc, char** argv)
{
	//set loop chunk size
	int chunk = chunk_size;
	int i, j, m, n;
	double B[LEN * LEN];
	double C[LEN * LEN];
	for(i = 0; i < LEN * LEN; i++)
		B[i] = rand() % RANGE;
	for(i = 0; i < LEN * LEN; i++)
		C[i] = B[i];


#pragma omp parallel shared(B, C, chunk) private(i, j, m, n)
	{
#pragma omp for schedule(static, chunk)
		for(i = EDGE; i < LEN - EDGE; i++)
		{
			for(j = EDGE; j < LEN - EDGE; j++)
			{
				int array_len = (2 * EDGE + 1) * (2 * EDGE + 1);
				double array[array_len];
				int c = 0;


				for(m = i - EDGE; m <= i + EDGE; m++)
				{
					for(n = j - EDGE; n <= j + EDGE; n++)
					{
						array[c] = B[m * LEN + n];
						c++;
					}
				}
				sort(array, array_len);
				C[i * LEN + j] = array[array_len / 2];
			}
		}
	}

/*	//print matrices
	printf("Matrice B:\n");
	print_matrice(B, LEN, LEN);
*/
	printf("Matrice C:\n");
	print_matrice(C, LEN, LEN);

	return EXIT_SUCCESS;
}

void print_matrice(double matrice[], int row, int column)
{
	int i,j;
	for(i = 0; i < row; i++)
	{                   
		for(j = 0; j < column; j++)
		{
			printf("%f ", matrice[i * row + j]);
		}
		printf("\n");
	}       
	printf("\n");       
}
