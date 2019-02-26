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

typedef struct BigInt BigInt;

BigInt* new_BigInt();
BigInt* construct_BigInt(long num);

void free_BigInt(BigInt* num);

BigInt* factorial(long n);

int sign(BigInt* num);
unsigned long digits(BigInt* num);

int compare_int(BigInt* lhs, long rhs);
int compare_bigint(BigInt* lhs, BigInt* rhs);

#endif // BIGINT_H
