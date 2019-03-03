/* 
 * File: BigInt.h
 *
 * Brief: Big Integer utility for arbitrary precision arithmetic
 *
 * Author: Alexander DuPre
 *
 */

#ifndef BIGINT_H
#define BIGINT_H

#include <stdint.h>

#ifdef DEBUG
typedef uint8_t bucket_t;
typedef int8_t  int_val;
#define BUCKET_WIDTH 8
#define BUCKET_MAX_SIZE UINT8_MAX
#define INT_VAL_MAX INT8_MAX
#define INT_VAL_MIN INT8_MIN
#else
    #ifdef BIGINT__x64
        typedef uint64_t bucket_t;
        typedef int64_t  int_val;
        #define BUCKET_WIDTH 64
        #define BUCKET_MAX_SIZE UINT64_MAX
        #define INT_VAL_MAX INT64_MAX
        #define INT_VAL_MIN INT64_MIN
    #else
        typedef uint32_t bucket_t;
        typedef int32_t  int_val;
        #define BUCKET_WIDTH 32
        #define BUCKET_MAX_SIZE UINT32_MAX
        #define INT_VAL_MAX INT32_MAX
        #define INT_VAL_MIN INT32_MIN
    #endif // BIGINT__x64
#endif // DEBUG

/*
 * bucket_t is an unsigned integral type that represents portions of the BigInt
 *
 * int_val is the signed integral of bucket_t and is meant for the user to quickly
 * assign values to the BigInt when the values are less than BUCKET_MAX_SIZE
 * int_val is also used for comparison of the BigInt with fixed precision integers
 *
 * If you are constructing a BigInt with a value greater than the BUCKET_MAX_SIZE
 * use the str_BigInt constructor which constructs the big int from the textual
 * representation of the number
 */

typedef struct BigInt BigInt;

// Returns BigInt with N buckets allocated, all with a value of 0
BigInt* reserve_BigInt(bucket_t buckets);

// Returns allocated empty BigInt, if malloc fails returns NULL
BigInt* empty_BigInt();

// Returns allocated BigInt with stored value, if malloc fails returns NULL
BigInt* val_BigInt(int_val num);

// Returns allocated BigInt with num converted to a value. Returns NULL if malloc
// fails, num isn't a number, or the base isn't hex, decimal
BigInt* str_BigInt(const char* num, int base);

// Returns a BigInt with the computed factorial value
BigInt* factorial(long n);

void free_BigInt(BigInt* num);

// Returns number of allocated buckets. 0 if num is NULL
int buckets(BigInt* num);

// sign < 0 if negative sign >= 0 if postive. 0 is included in postive range
int sign(BigInt* num);

// Returns -1 if num is NULL or base isn't hex or decimal
int digits(BigInt* num, int base);
 
// Returns -1 if base isn't hex or decimal
int count_digits(long num, int base);

// Returns 0 if equal. < 0 if (lhs < rhs) and > 0 if (lhs > rhs)
int compare_int(BigInt* lhs, int_val rhs);
int compare_bigint(BigInt* lhs, BigInt* rhs);

// Returns -1 if there is no valid conversion
int char_to_num(char c, int base);

#endif // BIGINT_H
