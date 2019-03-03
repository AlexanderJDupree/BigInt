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
    size_t digits;
    size_t nbuckets;
    int8_t sign;
};

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

static bucket_t string_to_num(const char* str_num, int base)
{
    const char** endptr = &str_num;
    bucket_t num = labs(strtol(str_num, (char**) endptr, base));
    return (**endptr == '\0') ? num : 0;
}

// Points start to the first character of the string, and end to the last
static int format_string(const char* str, const char** start, const char** end, int base)
{
    if(str && start && end)
    {
        int i = -1;
        int sign = 1;
        while(isspace(str[++i]));

        if (str[i] == '-')
        {
            sign = -1;
            ++i;
        }

        if (str[i] == '\0') // String was all spaces
        {
            return 0;
        }

        // Allows hex strings to be prefixed with 0x. TODO allow other prefixes
        if(str[i + 1] == 'x' && base == 16)
        {
            i += 2;
        }

        while(str[i] == '0')
        {
            ++i;
        }

        if(str[i] == '\0')
        {
            return 0;
        }

        *start = str + i;

        // TODO this will allow strings like "123,./" through. Examine if this 
        // behavior is wanted. *start = 1, *end = ,.
        while(isalnum(str[i]))
        {
            ++i;
        }

        *end = str + i;
        return sign;
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

BigInt* val_BigInt(bucket_t num)
{
    BigInt* new_int = reserve_BigInt(1);
    if(new_int)
    {
        new_int->value[0] = num;
    }
    return new_int;
}

BigInt* str_BigInt(const char* str_num, int base)
{
    // TODO refactor algorithm to support conversion of different bases
    if (base != 16) return NULL;

    const char* start = NULL;
    const char* end = NULL;
    int8_t sign = format_string(str_num, & start, &end, base);

    // sign is zero if format_string failed
    if(base < 2 || base > 36 || sign == 0)
    {
        return NULL;
    }

    size_t digits = end - start;
    size_t digits_per_bucket = 2 * sizeof(bucket_t); 
    size_t nbuckets = (digits + digits_per_bucket - 1) / digits_per_bucket;
    if(nbuckets == 1)
    {
        BigInt* new_int = val_BigInt(string_to_num(str_num, base));
        new_int->sign = sign;
        return new_int;
    }

    BigInt* new_int = reserve_BigInt(nbuckets);
    
    if(new_int)
    {
        bucket_t total= 0;
        for(size_t i = 0; i < digits % digits_per_bucket; ++i)
        {
            int_val digit = char_to_num(*(start++), base);
            if(digit == -1)
            {
                return clear_BigInt(new_int);
            }
            total |= digit << (BUCKET_WIDTH - (4 * (i + 2)));
        }

        if(total != 0)
        {
            new_int->value[--nbuckets] = total;
        }

        for (; start < end; start += digits_per_bucket)
        {
            total = 0;
            for(size_t i = 0; i < digits_per_bucket; ++i)
            {
                int_val digit = char_to_num(*(start + i), base);
                if( digit == -1 )
                {
                    return clear_BigInt(new_int);
                }
                total |= digit << (BUCKET_WIDTH - (4 * (i + 1)));
            }
            new_int->value[--nbuckets] = total;
        }
    }
    new_int->sign = sign;
    return new_int;
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

int compare_uint(BigInt* lhs, bucket_t rhs)
{
    if(lhs)
    {
        return lhs->value[0] - rhs;
    }
    return rhs;
}

int compare_int(BigInt* lhs, int_val rhs)
{
    rhs = labs(rhs);
    return compare_uint(lhs, rhs);
}

#ifdef MOCKING_ENABLED

static bucket_t* get_buckets(BigInt* num)
{
    return num->value;
} 

const mock_bigint m_bigint = {
    .char_to_num = char_to_num,
    .format_string = format_string,
    .get_buckets = get_buckets
};

#endif // MOCKING_ENABLED

