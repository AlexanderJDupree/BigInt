/* 
 * File: BigFibonacci.c
 *
 * Brief:  
 * 
 * Author: Alexander DuPre
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "BigInt.h"

int main(int argc, char **argv) 
{
    if (argc != 2)
    {
        printf("\nEnter a number");
        return 1;
    }

    int n = atoi(argv[1]);

    BigInt* b1 = str_BigInt("1");
    BigInt* b2 = str_BigInt("1");

    for (int i = 2; i < n; i++) {

        BigInt* b = str_BigInt("1");

        free_BigInt(b1);
        b1 = b2;
        b2 = b;
    }
    free_BigInt(b1);
    free_BigInt(b2);
    return 0;
}
