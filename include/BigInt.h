/* 
 * File: BigInt.h
 *
 * Brief: Big Integer utility for arbitrary precision arithmetic. Currently
 *        BigInt only supports strings in hexadecimal format
 *
 * Author: Alexander DuPre
 *
 */

#ifndef BIGINT_H
#define BIGINT_H

#define BIGINT_RADIX 16

#include <stdint.h>

#if defined(BIGINT__8bit)
    typedef uint8_t bucket_t;
    typedef int8_t  int_val;
    #define BUCKET_WIDTH 8
    #define BUCKET_MAX_SIZE UINT8_MAX
#elif defined(BIGINT__x64)
    typedef uint64_t bucket_t;
    typedef int64_t  int_val;
    #define BUCKET_WIDTH 64
    #define BUCKET_MAX_SIZE UINT64_MAX
#else // BIGINT__x86
    typedef uint32_t bucket_t;
    typedef int32_t  int_val;
    #define BUCKET_WIDTH 32
    #define BUCKET_MAX_SIZE UINT32_MAX
#endif // BIGINT__SIZE

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
BigInt* val_BigInt(bucket_t num);

// Returns BigInt with value represented by num. Returns empty_BigInt() if num
// cannot be converted
 BigInt* str_BigInt(const char* num);

// Resets all buckets to 0, returns num
BigInt* clear_BigInt(BigInt* num);

// Returns a BigInt with the computed factorial value
BigInt* factorial(long n);

void free_BigInt(BigInt* num);

// Returns number of allocated buckets. 0 if num is NULL
int buckets(BigInt* num);

// sign < 0 if negative sign > 0 if postive. 0 is returned if num is NULL
int sign(BigInt* num);

// Returns -1 if num is NULL or base is invalid
int hex_digits(BigInt* num);

// Returns 0 if equal. < 0 if (lhs < rhs) and > 0 if (lhs > rhs)
int compare_int(BigInt* lhs, int_val rhs);
int compare_uint(BigInt* lhs, bucket_t rhs);
int compare_bigint(BigInt* lhs, BigInt* rhs);

#ifdef MOCKING_ENABLED
// mock_bigint allows static functions and private members to be visible during
// unit tests
typedef struct mock_bigint {
    int (*char_to_num)(char, int);
    int (*format_string)(const char*, const char**, const char**);
    bucket_t* (*get_buckets)(BigInt* num);
} mock_bigint;

extern const mock_bigint m_bigint;

#endif // MOCKING_ENABLED

#endif // BIGINT_H
