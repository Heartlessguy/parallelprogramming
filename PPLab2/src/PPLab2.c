//? LAB2-ex5
//? Alexandr Komnantyy
//! Using  SCATTER/GATHER
//? for more info see ../PPlab2.MD

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define m 10 // array length
#define dp 100.0f // arr elements rand range


// Function for init input vector in #dp range
void initvec(int m1, float *arr1)
{
    int i, j;
    float sum;
    for (i = 0; i < m1; i++)
    {
        arr1[i] = (float)rand() / RAND_MAX * dp * 2.0f - dp;
    }
}

// Function to print out vector
void prnvec(int m1, float *arr, char *zag)
{
    int i;
    printf("%s\n", zag);
    for (i = 0; i < m1; i++)
        printf("%10.1f", arr[i]);
    printf("\n");
}

// Function to calculate math expectation
float Calc_MO(int count, float *arr)
{
    int i;
    float sum = 0.0;
    for (i = 0; i < count; i++)
    {
        sum = sum + arr[i];
    }
    return sum / count;
}

// Function to calculate mean-sq(standard) deviation
float Calc_SO(int count, float *arr, float mo)
{
    int i;
    float sum = 0.0;
    for (i = 0; i < count; i++)
    {
        sum = sum + arr[i] * arr[i];
    }
    sum = sum / count;
    return sqrt(sum - mo * mo);
}

// Entry point
int main(int argc, char **argv)
{
    
    srand(time(NULL));

    int rank, size, count, scol, ost, i, nach;
    double time1, time2;
    MPI_Status status;
    float *initial, *resultmo, *resultso, *partial;
    int *rcount, *displs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    count = m / size;
    ost = m % size;
    if (rank == 0)
    {
        printf("Numbers of proccesses %d. \nElements in vector %d.\n", size, m);
        initial = (float *)malloc(m * sizeof(float));
        resultmo = (float *)malloc(size * sizeof(float));
        resultso = (float *)malloc(size * sizeof(float));
        initvec(m, initial);
        prnvec(m, initial, "Initial vector");
        time1 = MPI_Wtime();
        rcount = (int *)malloc(m * sizeof(int));
        displs = (int *)malloc(m * sizeof(int));
        for (i = 0; i < size; i++)
        {
            scol = i < ost ? count + 1 : count;
            rcount[i] = scol;
            nach = i * scol + (i >= ost ? ost : 0);
            displs[i] = nach;
        }
        for (i = 0; i < size; i++)
            printf("%8d", rcount[i]);
        printf(" rcounts \n");
    } 
    //End of work process 0

    scol = rank < ost ? count + 1 : count;
    nach = rank * scol + (rank >= ost ? ost : 0);
    partial = (float *)calloc(scol, sizeof(float));
    MPI_Scatterv(initial, rcount, displs, MPI_FLOAT, partial,
                 scol, MPI_FLOAT, 0, MPI_COMM_WORLD);
    printf("Process %d Scol %d\n", rank, scol);
    prnvec(scol, partial, "Partial vector");
    printf("\n");
    float mo = 0, so = 0;
    mo = Calc_MO(scol, partial);
    so = Calc_SO(scol, partial, mo);
    MPI_Gather(&mo, 1, MPI_FLOAT, resultmo, 1,
               MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Gather(&so, 1, MPI_FLOAT, resultso, 1,
               MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        prnvec(size, resultmo, "vector M.O.");
        prnvec(size, resultso, "vector S.O.");
        time2 = MPI_Wtime();
        printf("\nTime parallel calculation = %f s.\n", time2 - time1);
        mo = Calc_MO(size, resultmo);
        so = Calc_SO(size, resultso, mo);
        printf("M.O. = %6.1f, S.O. = %6.1f\n", mo, so);
        free(initial);
        free(resultmo);
        free(resultso);
    }
    free(partial);
    MPI_Finalize();
    return 0;
}
