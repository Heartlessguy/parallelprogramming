//? LAB1-20i pt.3
//? Alexandr Komnantyy
//! Using BCAST/REDUCE & PACK/UNPACK

#include <stdio.h>
#include <math.h>
#include "mpi.h"

/* Prototype the function that we'll use below. */
static double f(double, double);
static double fi(double, double);
int main(int argc, char *argv[])
{
    double a = -1.0, b = 0.8, c = 1.2, Eps;
    int done = 0;
    int i, n, rank, size, Ierr, namelen;
    double Sum, Gsum, Isum, time1, time2, a1, b1, x, F;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status stats;

    /* Normal MPI startup */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    //! захламляет stdout 
    //! printf("Process %d of %d on %s\n", rank, size, processor_name);
    char buffer[128];
    int bufpos;

    if (rank == 0)
    {
        printf("Введите границу a\n");
        scanf("%lf", &a);
        printf("Введите границу b\n");
        scanf("%lf", &b);
        printf("Введите c\n");
        scanf("%lf", &c);
        bufpos = 0;
        MPI_Pack(&a, 1, MPI_DOUBLE, &buffer, 128, &bufpos, MPI_COMM_WORLD);
        MPI_Pack(&b, 1, MPI_DOUBLE, &buffer, 128, &bufpos, MPI_COMM_WORLD);
        MPI_Pack(&c, 1, MPI_DOUBLE, &buffer, 128, &bufpos, MPI_COMM_WORLD);
        MPI_Bcast(&buffer, 128, MPI_PACKED, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Bcast(&buffer, 128, MPI_PACKED, 0, MPI_COMM_WORLD);
        bufpos = 0;
        MPI_Unpack(&buffer, 128, &bufpos, &a, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(&buffer, 128, &bufpos, &b, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(&buffer, 128, &bufpos, &c, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    }

    Eps = 1.0;

    while (!done)
    {
        if (rank == 0)
        {
            printf("Enter Eps ( > 0.1 - quit) ");
            fflush(stdout);
            scanf("%lf", &Eps);
            printf("%10.9lf\n", Eps);
            time1 = MPI_Wtime();
        }
        /* Sending Eps to over nodes */
        MPI_Bcast(&Eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (Eps > 0.1)
        {
            done = 1;
        }
        else
        {
            /* Each process define his integration limit. and a number of intervals */
            a1 = a + (b - a) * rank / size;
            b1 = a1 + (b - a) / size;
            // n = 0.35/(Eps*size);
            n = 1.0 / (Eps * size);
            /* Calculating of a local sum by each process */
            Sum = 0.0;
            for (i = 1; i <= n; i++)
            {
                x = a1 + (b1 - a1) * (i - 0.5) / n;
                Sum += f(x, c);
            }
            printf("Process %d SUMM %.16f \n", rank, Sum / (n * size) * (b - a));
            MPI_Reduce(&Sum, &Gsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            if (rank == 0)
            {
                Gsum = Gsum / (n * size) * (b - a);
                time2 = MPI_Wtime();
                printf("\nIntegral of function from %5.2f to %5.2f..\n", a, b);
                printf("Eps %9.7f. Integral = %18.16f, error = %18.16f.\n",
                        Eps, Gsum, fi(b, c) - fi(a, c) - Gsum);
                printf("Time calculation = %f seconds.\n\n", time2 - time1);
                fflush(stdout);
            }
        }
    }
    
    /* All done */
    MPI_Finalize();
    return 0;
}

/* Define function  */
static double f(double a, double c)
{
    return a * sinh(a * c);
}
static double fi(double a, double c)
{
    return (a * cosh(a * c) / c) - (sinh(a * c) / pow(c, 2));
}