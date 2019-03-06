/*
 * File: BigInt.c
 *
 * Brief: BigInt Implementation file
 *
 * Author: Alexander DuPree
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BigInt.h"

// TODO inspect padding
struct BigInt
{
    bucket_t* value;
    size_t nbuckets;
    int8_t sign;
};

/*******************************************************************************
* STATIC MEMBER FUNCTIONS
*******************************************************************************/

#if defined( BIGINT__x64 )

static bucket_t add_with_carry(bucket_t *carry, bucket_t b1, bucket_t b2) 
{
    bucket_t sum;
    bucket_t carry_in = *carry;
    uint8_t carry_out;

    asm(
        "xorq	%0, %0\n\t"
        "cmpq   %4, %0\n\t"
        "movq	%2, %0\n\t"
        "adcq	%3, %0\n\t"
        "setc	%1\n\t"
        : "=&rm" (sum), "=&rm"(carry_out)
        : "rm" (b1), "rm" (b2), "r" (carry_in)
        : "cc"
    );

    *carry = carry_out;
    return sum;
}

#elif defined( BIGINT__x86 )

static bucket_t add_with_carry(bucket_t *carry, bucket_t b1, bucket_t b2) 
{
    bucket_t sum;
    bucket_t carry_in = *carry;
    uint8_t carry_out;

    asm(
        "xorl	%0, %0\n\t"
        "cmpl   %4, %0\n\t"
        "movl	%2, %0\n\t"
        "adcl	%3, %0\n\t"
        "setc	%1\n\t"
        : "=&rm" (sum), "=&rm"(carry_out)
        : "rm" (b1), "rm" (b2), "r" (carry_in)
        : "cc"
    );

    *carry = carry_out;
    return sum;
}

#else

static bucket_t add_with_carry(bucket_t* carry, bucket_t b1, bucket_t b2) 
{
    bucket_t sum = b1;

    uint8_t carry_in = (sum += *carry) < b1 ? 1 : 0;

    uint8_t carry_out = (sum += b2) < b2 ? 1 : carry_in;

    *carry = carry_out;

    return sum;
}

#endif

static int count_hex_digits(bucket_t num)
{
    if(num == 0)
    {
        return 1;
    }

    //stackoverflow.com/questions/9721042/
    //count-number-of-digits-which-method-is-most-efficient/
    // There was a number of different methodoligies suggested, this one was
    // the easiest to implement
#ifdef BIGINT__x64
    return snprintf(NULL, 0, "%lx", num);
#else
    return snprintf(NULL, 0, "%x", num);
#endif
}

static int hex_value(char c)
{
    return (c >= '0' && c <= '9') ? c - '0' :
           ((c = tolower(c)) >= 'a' && c <= 'f') ? c - 'a' + 10 : -1;
}

static int iszero(int c)
{
    return c == '0';
}

static const char* strip_character(const char* str, int (*istarget)(int c))
{
    if(str)
    {
        while(istarget(*str))
        {
            ++str;
        }
    }
    return str;
}

static const char* strip_negative_and_store(const char* str, int* sign)
{
    if(str && *str == '-')
    {
        *sign = -1;
        ++str;
    }
    return str;
}

static const char* strip_hex_prefix(const char* str)
{
    if(str && *str == '0' && *(str + 1) == 'x')
    {
        str += 2;
    }
    return str;
}

// Points start to the first character of the string, and end to the last
static int format_string(const char* str, const char** start, const char** end)
{
    if(str && start && end)
    {
        int sign = 1;

        // Strips white space, strips negative and stores into sign, strips
        // hex prefix, then finally strips leading zeroes
        str = strip_negative_and_store(strip_character(str, isspace), &sign);
        str = strip_character(strip_hex_prefix(str), iszero);

        if (*str == '\0') // String is invalid
        {
            return 0;
        }

        *start = str;

        // sets end pointer to one past last alphanumeric character
        *end = strip_character(str, isxdigit);
        return (**end == '\0' || isspace(**end)) ? sign : 0;
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

static const char* fill_buckets(const char* start, const char* end, 
                                bucket_t* buckets, size_t* nbuckets)
{
        bucket_t bucket = 0;
        int digits_per_bucket = 2 * sizeof(bucket_t);
        int digits_to_process = (start != end) ? digits_per_bucket : 0;

        for (; start < end; ++start)
        {
            int digit = hex_value(*start);
            // digit will always be valid, string was checked for non hex digits 

            bucket = (bucket << 4) | digit;

            if(--digits_to_process == 0)
            {
                // Dump bucket
                buckets[--(*nbuckets)] = bucket;
                bucket = 0;

                // Don't reset counter if this is the last iteration
                digits_to_process = (start + 1 != end) ? digits_per_bucket : 0;
            }
        }
        if(digits_to_process > 0)
        {
            // Dump remaining values in bucket
            buckets[--(*nbuckets)] = bucket;
        }
        return start;
}

static void swap (BigInt** b1, BigInt** b2)
{
    BigInt* temp = *b1;
    *b1 = *b2;
    *b2 = temp;
    return;
}

static size_t leading_bucket(BigInt* num)
{
    if(num)
    {
        size_t lead_bucket = num->nbuckets;
        while(num->value[--lead_bucket] == 0 && lead_bucket > 0);
        return ++lead_bucket;
    }
    return 0;
}

/*******************************************************************************
* CONSTRUCTORS
*******************************************************************************/

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

BigInt* val_BigInt(bucket_t num)
{
    BigInt* new_int = reserve_BigInt(1);
    if(new_int)
    {
        new_int->value[0] = num;
    }
    return new_int;
}

BigInt* str_BigInt(const char* str_num)
{
    const char* start = NULL;
    const char* end = NULL;
    int8_t sign = format_string(str_num, &start, &end);
    
    if(sign == 0) // format_string failed, do not parse string
    {
        return empty_BigInt();
    }

    size_t digits = end - start;
    size_t digits_per_bucket = 2 * sizeof(bucket_t); 
    size_t nbuckets = (digits + digits_per_bucket - 1) / digits_per_bucket;

    BigInt* new_int = reserve_BigInt(nbuckets);
    if(new_int)
    {
        new_int->sign = sign;
        const char* overhang = start + (digits % digits_per_bucket);

        // Fills buckets from start - overhang then from overhang to end
        fill_buckets(fill_buckets(start, overhang, new_int->value, &nbuckets), 
                     end, new_int->value, &nbuckets);
    }
    return new_int;
}

/*******************************************************************************
* ARITHMETIC
*******************************************************************************/

BigInt* add(BigInt* b1, BigInt* b2)
{
    if(b1 == NULL || b2 == NULL)
    {
        return NULL;
    }

    size_t b1_buckets = leading_bucket(b1);
    size_t b2_buckets = leading_bucket(b2);
    if(b1_buckets < b2_buckets)
    {
        swap(&b1, &b2);
        size_t temp = b1_buckets;
        b1_buckets = b2_buckets;
        b2_buckets = temp;
    }

    bucket_t carry_out = 0;

    BigInt* result = reserve_BigInt(b1_buckets);

    size_t i = 0;
    for(; i < b2_buckets; ++i)
    {
        result->value[i] = add_with_carry(&carry_out, b1->value[i], b2->value[i]);
    }
    for(; i < b1_buckets; ++i)
    {
        result->value[i] = add_with_carry(&carry_out, b1->value[i], 0);
    }
    result->value[i] = carry_out;

    return result;
}

/*******************************************************************************
* UTILITIES/COMPARISON
*******************************************************************************/

void free_BigInt(BigInt* num)
{
    free(num->value);
    free(num);
    return;
}

BigInt* clear_BigInt(BigInt* num)
{
    for(size_t i = 0; i < num->nbuckets; ++i)
    {
        num->value[i] = 0;
    }
    num->sign = 1;
    return num;
}

int compare_bigint(BigInt* lhs, BigInt* rhs)
{
    // TODO this compares pointers, need to perform arithmetic
    return lhs->value - rhs->value;
}

int compare_uint(BigInt* lhs, bucket_t rhs)
{
    if(lhs)
    {
        return lhs->value[0] - rhs;
    }
    return rhs;
}

int compare_int(BigInt* lhs, sbucket_t rhs)
{
    if(lhs)
    {
        return (lhs->sign < 0 && rhs > 0 ) ? 
                compare_uint(lhs, labs(rhs)) * -1 : compare_uint(lhs, labs(rhs));
    }
    return rhs;
}

/*******************************************************************************
* INSPECTORS
*******************************************************************************/

int buckets(BigInt* num)
{
    return (num != NULL) ? num->nbuckets : 0;
}

int sign(BigInt* num)
{
    return (num != NULL) ? num->sign : 0;
}

int hex_digits(BigInt* num)
{
    if(num != NULL)
    {
        // Finds the leading bucket value
        size_t i = num->nbuckets - 1;
        for(; i > 0  && num->value[i] == 0; --i);

        // Return leading digits plus the number of digits for each full bucket
        return count_hex_digits(num->value[i]) 
               + (num->nbuckets - (num->nbuckets - i)) * 2 * sizeof(bucket_t);
    }
    return -1;
}

/*******************************************************************************
* UNIT TEST MOCK OBJECT
*******************************************************************************/

#ifdef MOCKING_ENABLED

// Deprecated now that BigInt only supports hexadecimal input
static int char_to_num(char c, int base)
{
    c = tolower(c);

    // Default base is 10
    base = (base < 2 || base > 36) ? 10 : base;

    char upper_alphabetic_limit = 'a' + base - 11;
    char upper_numeric_limit = (base < 10) ? '0' + base - 1 : '9';

    return (c >= '0' && c <= upper_numeric_limit)   ? c - '0' :
           (c >= 'a'&& c <= upper_alphabetic_limit) ? c - 'a' + 10 : -1;
}

static bucket_t* get_buckets(BigInt* num)
{
    return num->value;
} 

const mock_bigint m_bigint = {
    .char_to_num = char_to_num,
    .format_string = format_string,
    .add_carry = add_with_carry,
    .get_buckets = get_buckets
};

#endif // MOCKING_ENABLED

