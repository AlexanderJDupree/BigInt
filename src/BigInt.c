/*
 * File: BigInt.c
 *
 * Brief: BigInt Implementation file
 *
 * Author: Alexander DuPree
 *
 */

#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "BigInt.h"

// TODO inspect padding
struct BigInt
{
    bucket_t* value;
    bucket_t digits;
    size_t nbuckets;
    int8_t sign;
};

static bucket_t convert_to_num(const char* str_num, int base)
{
    const char** endptr = &str_num;
    long num = labs(strtol(str_num, (char**) endptr, base));
    return (**endptr == '\0') ? labs(num) : 0;
}

static int8_t is_negative(const char* str_num)
{
    // strips white space and searches for first character
    // Assumes str_num is null_terminated
    if(str_num)
    {
        int i = -1;
        while(isspace(str_num[++i]));
        return (str_num[i] == '-') ? -1 : 1;
    }
    return 0;
}

static bucket_t* allocate_buckets(size_t buckets)
{
    bucket_t* bucket = (bucket_t*) malloc(sizeof(bucket_t) * buckets);
    for (bucket_t i = 0; i < buckets; ++i)
    {
        bucket[i] = 0;
    }
    return bucket;
}

BigInt* reserve_BigInt(bucket_t buckets)
{
    BigInt* new_int = (BigInt*) malloc(sizeof(BigInt));
    if(new_int)
    {
        new_int->value = allocate_buckets(buckets);
        new_int->nbuckets = buckets;
        new_int->sign = 1;
    }
    return new_int;
}

BigInt* empty_BigInt() 
{
    return reserve_BigInt(1);
}

BigInt* val_BigInt(int_val num)
{
    BigInt* new_int = reserve_BigInt(1);
    if(new_int)
    {
        new_int->sign = num;
        new_int->value[0] = labs(num);
    }
    return new_int;
}

BigInt* str_BigInt(const char* str_num, int base)
{
    if(base < 2 || base > 36)
    {
        return NULL;
    }
    // Count number of digits in string
    
    bucket_t val = convert_to_num(str_num, base);
    BigInt* new_int = val_BigInt(val);
    new_int->sign = is_negative(str_num);
    return new_int;
}

void free_BigInt(BigInt* num)
{
    free(num->value);
    free(num);
    return;
}

int buckets(BigInt* num)
{
    return (num != NULL) ? num->nbuckets : 0;
}

int sign(BigInt* num)
{
    return num->sign;
}

int digits(BigInt* num, int base)
{
    return (num != NULL) ? count_digits(num->value[num->nbuckets - 1], base) : -1;
}

int count_digits(long num, int base)
{
    num = labs(num); // labs : long absolute value
    if(base == 10 || base == 16)
    {
        int digits = 1;
        int power = 1;
        while(num / power >= base)
        {
            ++digits;
            power *= base;
        }
        return digits;
    }
    return -1;
}

int compare_bigint(BigInt* lhs, BigInt* rhs)
{
    // TODO this compares pointers, need to perform arithmetic
    return lhs->value - rhs->value;
}

int compare_int(BigInt* lhs, int_val rhs)
{
    if(lhs)
    {
        return (lhs->sign > 0) ? lhs->value[0] - rhs : lhs->value[0] - labs(rhs);
    }
    return  rhs;
}

int char_to_num(char c, int base)
{
    c = tolower(c);
    base = (base < 2 || base > 36) ? 10 : base;

    char upper_alphabetic_limit = 'a' + base - 11;
    char upper_numeric_limit = (base < 10) ? '0' + base - 1 : '9';

    return (c >= '0' && c <= upper_numeric_limit) ? c - '0' :
           (c >= 'a'&& c <= upper_alphabetic_limit) ? c - 'a' + 10 : -1;
}

