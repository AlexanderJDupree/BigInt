/*
 * File: BigInt.c
 *
 * Brief: BigInt Implementation file
 *
 * Author: Alexander DuPree
 *
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "BigInt.h"

#ifdef DEBUG
typedef int8_t Value;
#else
    #ifdef BIGINT__x64
        typedef int64_t Value;
    #else
        typedef int32_t Value;
    #endif // BIGINT__x64
#endif // DEBUG

struct BigInt
{
    Value value;
    int8_t sign;
};

BigInt* new_BigInt() 
{
    return (BigInt*) malloc(sizeof(BigInt));
}

BigInt* construct_BigInt(long num)
{
    BigInt* new_int = new_BigInt();
    new_int->value = num;
    return new_int;
}

void free_BigInt(BigInt* num)
{
    free(num);
    return;
}

int sign(BigInt* num)
{
    return num->sign;
}

int compare_bigint(BigInt* lhs, BigInt* rhs)
{
    return lhs->value - rhs->value;
}

int compare_int(BigInt* lhs, long rhs)
{
    return lhs->value - (Value) rhs;
}

