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
#include <getopt.h>

#include "BigInt.h"

void usage();
void run_BigFibonacci(unsigned n);

int main(int argc, char **argv) 
{
    int opt = 0;
    int exit_status = 0;

    while((opt = getopt(argc, argv, "h?")) != -1)
    {
        switch(opt)
        {
            case 'h' : exit_status = 1; 
                       break;
            case '?' : exit_status = 1;
                       break;
            default  : break;
        }
    }
    if(exit_status || optind >= argc)
    {
        usage();
    }
    else
    {
        run_BigFibonacci(strtoul(argv[optind], NULL, 0));
    }
    return exit_status;
}

void usage()
{
    fprintf(stderr, "usage:\n  run_BigFibonacci <integer>\nOptions:\n  "
            "-h or -?\t\tDisplays usage info\n  "
            "\n  BigFibonacci utilizes the BigInt library to display the Nth\n  "
            "Fibonacci number. https://github.com/AlexanderJDupree/BigInt\n");
    return;
}

void run_BigFibonacci(unsigned n)
{
    BigInt* a = val_BigInt(0);
    BigInt* b = val_BigInt(1);

    printf("Fibonacci Number %d is:\n", n);
    for (unsigned i = 1; i < n; ++i)
    {
        BigInt* c = add(a, b);
        free_BigInt(a);
        a = b;
        b = c;
    }
    display(b);
    free_BigInt(a);
    free_BigInt(b);
    return;
}

