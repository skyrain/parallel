#include<stdio.h>
#include<stdlib.h>
#include <mpi.h>

#define LEN 250
#define RANGE 10000

void master(int, int);
void slave(int, int);
void initialize_array(int[], int);
void getRandomArray(int[], int);
void print_matrice(int[], int, int);

int main(int argc, char** argv)
{
    //Initialize MPI
    MPI_Init(NULL,NULL);

    //get number of processes
    int p_size;
    MPI_Comm_size(MPI_COMM_WORLD,&p_size);

    //get this process ID
    int PID;
    MPI_Comm_rank(MPI_COMM_WORLD,&PID);

    //exit if number of processes < 2
    if(p_size < 2)
    {
        printf("Not enough Processes assgined\n");
        return EXIT_FAILURE;
    }
    else if (PID == 0)
        master(PID, p_size);
    else
        slave(PID, p_size);

    MPI_Finalize();

    return EXIT_SUCCESS;
}

void master(int PID, int p_size)
{
    int A[LEN * LEN];
    getRandomArray(A, LEN * LEN);
    int B[LEN * LEN];
    getRandomArray(B, LEN * LEN);
    MPI_Bcast(A, LEN * LEN, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, LEN * LEN, MPI_INT, 0, MPI_COMM_WORLD);

    //C is the result matrice
    int C[LEN * LEN];
    int cColumn[LEN];
    //initialize matrice C and cLine
    initialize_array(C, LEN * LEN);
    initialize_array(cColumn, LEN);

    MPI_Status status;
    //C has LEN columns
    int i;
    for(i = 0;i < LEN;i++)
    {
        //accept data from each process
        int j;
        for(j = 1;j < p_size;j++)
        {
            MPI_Recv(cColumn, LEN, MPI_INT, j, i, MPI_COMM_WORLD, &status);
            //construct the  result C matrice
            int k;
            for(k = 0; k < LEN;k++)
                C[k * LEN + i] = C[k * LEN + i] + cColumn[k];
        }
    }

    //print matrices
    printf("Matrice A:\n");
    print_matrice(A, LEN, LEN);
    printf("Matrice B:\n");
    print_matrice(B, LEN, LEN);
    printf("Matrice C:\n");
    print_matrice(C, LEN, LEN);

}

void slave(int PID, int p_size)
{
    int A[LEN * LEN];
    int B[LEN * LEN];
    MPI_Bcast(A, LEN * LEN, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, LEN * LEN, MPI_INT, 0, MPI_COMM_WORLD);

    //C is the result matrice
    int C[LEN * LEN];
    int cColumn[LEN];

    MPI_Status status;

    //allocate certain rows of A to this procedure
    int count = 0;
    int i;
    for(i = PID -1; i < LEN; i += p_size - 1)
        count ++;

    int* indexes = (int* )malloc(sizeof(int) * count);

    count = 0;
    for(i = PID -1; i < LEN; i += p_size -1)
    {
        indexes[count] = i;
        count++;
    }
    //loop by column of B
    for(i = 0;i < LEN;i++)
    {

        initialize_array(cColumn, LEN);

        if(count > 0)
        {
            int j;
            for(j = 0; j < count; j++)
            {
                int k;
                for(k = 0; k < LEN; k++)
                {
                    cColumn[indexes[j]] += A[indexes[j] * LEN + k] * B[k * LEN + i];
                }
            }
            MPI_Send(cColumn, LEN, MPI_INT, 0, i, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(cColumn, LEN, MPI_INT, 0, i, MPI_COMM_WORLD);
        }
    }
}

/*
 ***************method******************
 */
/******** Initialize array********************/
void initialize_array(int array[], int len)
{
    int i;
    for(i = 0; i < len; i++)
        array[i] = 0;
}

void getRandomArray(int array[], int len)
{
    int i;
    for(i = 0; i < len; i++)
        array[i] = rand() % RANGE;
}

void print_matrice(int matrice[], int row, int column)
{
    int i,j;
    for(i = 0; i < row; i++)
    {
        for(j = 0; j < column; j++)
        {
            printf("%d ", matrice[i * row + j]);
        }
        printf("\n");
    }
    printf("\n");
}

