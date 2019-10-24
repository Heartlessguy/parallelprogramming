/* extvec.c -- parallel program for calculate
 *               max, min (with indexes) and  average of array
 * Input      -- Input array is formed in program
 *                from random numbers
 * Author     -- Fefelov N.P., TUSUR, kafedra ASU
 * Last modification  -- 24.10.15
*/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define m 131 //20000
#define dp 100.0f

/* Function for init input vector  */
void initvec(int m1, float *arr1)
{
    int i;
    //        for(i=0; i<m1; i++)     arr1[i] = (float)(i+1)*0.1f;
    for (i = 0; i < m1; i++)
        arr1[i] = (float)rand() / RAND_MAX * dp * 2.0f - dp;
}

/* Function for output vector   */
void prnvec(int m1, float *arr, char *zag)
{
    int i;
    printf("%s\n", zag);
    for (i = 0; i < m1; i++)
        printf("%10.1f", arr[i]);
    printf("\n");
}
/* Function for output last element vector */
void prnvecl(int m1, float *arr, char *zag)
{
    int i;
    printf("%s %10.3f\n", zag, arr[m1 - 1]);
}

/* Function for sequential calculate  */
/* void mdd (int m1, float *arr1, myst* maxim)
    { int i, ind;
      float max1;
        max1 = arr1[0]; ind = 0;
        for (i=1; i < m1; i++)
            if (max1 < arr1[i]) {max1 = arr1[i]; ind = i;};
        *maxim.val = max1; *maxim.pos = ind; */
/*          *max1 = 0; *imax1 = m;
        for (j = 0; j < m1; j++) *max1 = *max1 + arr1[j];
    }  */

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Status status;
    int nach, count, i, scol, ost;
    float va[m], *vb;
    float max1, min1;
    float lsum, gsum, avg;
    int indx1, indi1;
    double time1, time2;
    int *displs, *rcounts;
    // MPI_FLOAT_INT maxim, minim;
    struct myst
    {
        float val;
        int pos;
    };
    struct myst maxim, minim, omax, omin;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    count = m / size;
    ost = m % size;

    if (rank == 0) /* Process 0 - master  */
    {

        printf("Numbers of proccesses %d. \nElements in vector %d.\n", size, m);

        /* Init vector vA  */
        initvec(m, va);

        /* Output last element of vector A */
        //        prnvecl(m, va,"Vector A");

        time1 = MPI_Wtime(); //Time begining calculating of programm

        /* Creation auxiliary arrays for data communication */
        displs = (int *)malloc(size * sizeof(int));
        rcounts = (int *)malloc(size * sizeof(int));

        for (i = 0; i < size; i++)
        {
            scol = i < ost ? count + 1 : count;
            rcounts[i] = scol;
            nach = i * scol + (i >= ost ? ost : 0);
            displs[i] = nach;
        }

        for (i = 0; i < size; i++)
            printf("%8d", rcounts[i]);
        printf(" rcounts \n");
        for (i = 0; i < size; i++)
            printf("%8d", displs[i]);
        printf(" displs \n");

    } /* End of work process 0  */

    /* Calculating parts of vector vA for rank */
    scol = rank < ost ? count + 1 : count;
    /* Offset (in strings) part for rank in vector vA */
    nach = rank * scol + (rank >= ost ? ost : 0);

    /* Storage for part of array in rank */
    vb = (float *)calloc(scol, sizeof(float));

    /* Sending the parts of the vector vA to processes */
    MPI_Scatterv(&va, rcounts, displs, MPI_FLOAT, vb,
                 scol, MPI_FLOAT, 0, MPI_COMM_WORLD);
    printf("Process %d Scol %d\n", rank, scol);
    for (i = 0; i < scol; i++)
        printf("%8.2f", vb[i]);
    printf("\n");

    max1 = min1 = vb[0];
    indx1 = indi1 = 0;
    lsum = 0;
    for (i = 1; i < scol; i++)
    {
        if (max1 < vb[i])
        {
            max1 = vb[i];
            indx1 = i;
        }
        else if (min1 > vb[i])
        {
            min1 = vb[i];
            indi1 = i;
        }
        maxim.val = max1;
        maxim.pos = indx1 + nach;
        minim.val = min1;
        minim.pos = indi1 + nach;
        lsum = lsum + vb[i];
    }

    printf("MAXIMUM %8.2f %5d %5d\n", maxim.val, maxim.pos, maxim.pos - nach);
    printf("MINIMUM %8.2f %5d %5d\n", minim.val, minim.pos, minim.pos - nach);

    MPI_Reduce(&maxim, &omax, 1, MPI_FLOAT_INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Reduce(&minim, &omin, 1, MPI_FLOAT_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
    MPI_Reduce(&lsum, &gsum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)           /* Only master-process  */
    {                        //1
        time2 = MPI_Wtime(); //Time ending programm
        printf("\nTime parallel calculation = %f s.\n", time2 - time1);

        printf("Max element %8.2f, index %5d.\n", omax.val, omax.pos);
        printf("Min element %8.2f, index %5d.\n", omin.val, omin.pos);
        avg = gsum / m;
        printf("Average     %8.2f, Summa %8.2f\n", avg, gsum);
        //      prnvecl(m, va,"Vector va from parts");

        free(displs);
        free(rcounts);
    } //1 End work of master-process

    /* Delete storage array VB of process */
    free(vb);

    MPI_Finalize();
    return 0;
}
