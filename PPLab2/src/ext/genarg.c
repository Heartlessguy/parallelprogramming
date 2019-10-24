/* Program generation of random  numbers
*  Name:        genarg
*  Arguments:   number of rand, range, startnum
*  Executions:  genarg number range startnum
*  Example:     genarg 500 100.0 127
*  Numbers:     500, range: (-100.0 - +100.0), startnum 127
*  Output:      output of range random numbers in standard output
*  Autor:       Fefelov N.P
*  Last modification:   21.06.11                */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int n, i, st;
    double x, dp, rmax, max, min;

    if (argc < 4)
    {
        fprintf(stderr, "\nERROR!\n");
        fprintf(stderr, "Execution: genarg rand_numbers range start_num\n");
        fprintf(stderr, "Example:   genarg 500 100.0 10\n");
        fprintf(stderr, "Numbers: 500, range: -100.0 - +100.0, start_num: 10\n");
        fflush(stderr);
        return 3;
    }

    n = atoi(argv[1]);
    dp = atof(argv[2]);
    st = atoi(argv[3]);
    srand(st);
    for (i = 0; i < n; i++)
    {
        x = (float)rand() / RAND_MAX * dp * 2.0f - dp;
        printf("%20.10f", x);
        return 0;
    }
