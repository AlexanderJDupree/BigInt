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

static int count_hex_digits(bucket_t num)
{
    if(num == 0)
    {
        return 1;
    }
    //stackoverflow.com/questions/9721042/
    //count-number-of-digits-which-method-is-most-efficient/
#ifdef BIGINT__x64
    return snprintf(NULL, 0, "%lx", num);
#else
    return snprintf(NULL, 0, "%x", num);
#endif
}

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
static int format_string(const char* str, const char** start, const char** end)
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
        if(str[i + 1] == 'x' && str[i] == '0')
        {
            i += 2;
        }

        // Strip leading zeroes
        while(str[i] == '0')
        {
            ++i;
        }

        // String was all zeroes
        if(str[i] == '\0')
        {
            *start = *end = str + i;
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
    int8_t sign = format_string(str_num, & start, &end);

    if(start == end) // String was all zeroes
    {
        return empty_BigInt();
    }

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
    if(lhs)
    {
        return (lhs->sign < 0 && rhs > 0 ) ? 
                compare_uint(lhs, labs(rhs)) * -1 : compare_uint(lhs, labs(rhs));
    }
    return rhs;
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

