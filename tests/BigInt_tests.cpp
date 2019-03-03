/*
 * File: BigInt_tests.cpp
 *
 * Brief: Unit Tests for BigInt utility
 *
 * Author: Alexander DuPree
 *
 */

#ifdef UNIT_TESTS

#include <random>
#include <chrono>
#include <climits>
#include <iostream>
#include "catch.hpp"

extern "C" {
    #include "BigInt.h"
}

long long get_seed()
{
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

int_val any_int(int_val min, int_val max)
{
    std::mt19937 generator(get_seed());
    std::uniform_int_distribution<int_val> u_dist(min, max);
    return u_dist(generator);
}

TEST_CASE("Constructing BigInt's with values <= BUCKET_MAX_SIZE", "[constructors]")
{
    SECTION("default constructor returns an empty BigInt")
    {
        BigInt* num = empty_BigInt();

        // 0 is still a digit count of 1
        REQUIRE(digits(num, 10) == 1);
        REQUIRE(buckets(num) == 1);

        free_BigInt(num);
    }
    SECTION("Reserving space for a big int")
    {
        BigInt* num = reserve_BigInt(42);
        REQUIRE(buckets(num) == 42);
    }
    SECTION("Value constructor instantiates BigInt with specified value")
    {
        int test_val = 123;
        BigInt* num = val_BigInt(test_val);

        REQUIRE(digits(num, 10) == 3);
        REQUIRE(compare_int(num, test_val) == 0);

        free_BigInt(num);
    }
    SECTION("Value contructor with BUCKET_MAX_SIZE")
    {
        BigInt* num = val_BigInt(BUCKET_MAX_SIZE);

        REQUIRE(compare_uint(num, BUCKET_MAX_SIZE) == 0);
        free_BigInt(num);
    }
    SECTION("Construction with hexadecimal string literal")
    {
        int test_val = 0xff;
        const char* test_str = "0xff";

        BigInt* num = str_BigInt(test_str, 16);
        REQUIRE(compare_uint(num, test_val) == 0);
        free_BigInt(num);
    }
    SECTION("Consturction without '0x' prefix on hexadeciamal string")
    {
        int test_val = 0xff;
        const char* test_str = "ff";

        BigInt* num = str_BigInt(test_str, 16);
        REQUIRE(compare_uint(num, test_val) == 0);
        free_BigInt(num);
    }
    SECTION("Construction with invalid base does not allocate BigInt")
    {
        REQUIRE(str_BigInt("0xff", 42) == NULL);
    }
    SECTION("Invalid characters in string returns a BigInt with 0 value")
    {
        BigInt* num = str_BigInt("0xffghl", 16);
        REQUIRE(compare_uint(num, 0) == 0);
        free_BigInt(num);
    }
}

TEST_CASE("Constructing BigInts with values > BUCKET_MAX_SIZE", "[constructors]")
{
#ifdef BIGINT__8bit // Debug bucket is 8 bits wide. 
    SECTION("Debug bucket size + 1 allocates 2 buckets")
    {
        const char* test_str = "0x1000";
        BigInt* num = str_BigInt(test_str, 16);

        bucket_t* values = m_bigint.get_buckets(num);

        REQUIRE(buckets(num) == 2);
        REQUIRE(values[0] == 0);
        REQUIRE(values[1] == 16);
        free_BigInt(num);
    }
    SECTION("string with odd number of digits")
    {
        const char* test_str = "0x1234567";
        bucket_t expected_values[] = { 103, 69, 35, 1 };

        BigInt* num = str_BigInt(test_str, 16);
        bucket_t* values = m_bigint.get_buckets(num);

        REQUIRE(buckets(num) == 4);
        for(int i = 0; i < 4; ++i)
        {
            REQUIRE(expected_values[i] == values[i]);
        }

        free_BigInt(num);
    }
    SECTION("Leading zeroes are ignored")
    {
        const char* test_str = "0x000000100ff";
        bucket_t expected_values[] = { 255, 0, 1 };

        BigInt* num = str_BigInt(test_str, 16);
        bucket_t* values = m_bigint.get_buckets(num);

        REQUIRE(buckets(num) == 3);
        for(int i = 0; i < 3; ++i)
        {
            REQUIRE(expected_values[i] == values[i]);
        }

        free_BigInt(num);
    }
    SECTION("Large negative value")
    {
        const char* test_str = "-0x123456789abcdef";
        bucket_t expected_values[] = { 239, 205, 171, 137, 103, 69, 35, 1 };

        BigInt* num = str_BigInt(test_str, 16);
        bucket_t* values = m_bigint.get_buckets(num);

        REQUIRE(buckets(num) == 8);
        for(int i = 0; i < 8; ++i)
        {
            REQUIRE(expected_values[i] == values[i]);
        }

        free_BigInt(num);
    }
#endif

    SECTION("Composite Test")
    {
        const char* test_str = "\t\t  \n-001112233445566778899aabbccddeeff  \t";
#ifdef BIGINT__8bit
        uint8_t expected_values[] = {
            255, 238, 221, 204, 187, 170, 153, 
            136, 119, 102, 85, 68, 51, 34, 17, 1
        };
        int nbuckets = 16;
#else
#ifdef BIGINT__x64
        uint64_t expected_values[] = {
            9843086184167632639U, 76880272227133047U
        };
        int nbuckets = 2;
#else
        uint32_t expected_values[] = {
            3437096703U, 2291772091U, 1146447479U, 17900083U
        };
        int nbuckets = 4;
#endif
#endif
        BigInt* num = str_BigInt(test_str, 16);
        bucket_t* values = m_bigint.get_buckets(num);

        REQUIRE(buckets(num) == nbuckets);
        for(int i = 0; i < 16; ++i)
        {
            REQUIRE(expected_values[i] == values[i]);
        }
        free_BigInt(num);
    }
}


TEST_CASE("Converting characters to integer values", "[char_to_num]")
{
    SECTION("Base 10 characters")
    {
        for (int i = 0; i < 10; ++i)
        {
            REQUIRE(char_to_num(i + '0', 10) == i);
        }
    }
    SECTION("Base 16 characters")
    {
        for (int i = 0; i < 10; ++i)
        {
            REQUIRE(char_to_num(i + '0', 16) == i);
        }
        for (int i = 10; i < 16; ++i)
        {
            REQUIRE(char_to_num(i + 'a' - 10, 16) == i);
        }
    }
    SECTION("Base 36 characters, is the upper limit for conversion")
    {
        for (int i = 0; i < 10; ++i)
        {
            REQUIRE(char_to_num(i + '0', 36) == i);
        }
        for (int i = 10; i < 36; ++i)
        {
            REQUIRE(char_to_num(i + 'a' - 10, 36) == i );
        }
    }
    SECTION("Base 2 Characters, is the lower limit for conversion")
    {
        for (int i = 0; i < 2; ++i)
        {
            REQUIRE(char_to_num(i + '0', 2) == i);
        }
    }
    SECTION("Arbitrary base in range 2 - 36 inclusive")
    {
        int i = 0;
        int base = any_int(2, 36);

        for (; i < base && i < 10; ++i)
        {
            REQUIRE(char_to_num(i + '0', base) == i);
        }

        for (; i < base; ++i)
        {
            REQUIRE(char_to_num(i + 'a' - 10, base) == i);
        }
    }
    SECTION("Invalid characters return -1")
    {
        REQUIRE(char_to_num('a', 10) == -1);
        REQUIRE(char_to_num('g', 16) == -1);
        REQUIRE(char_to_num('/', 36) == -1);
        REQUIRE(char_to_num('3', 2) == -1);
    }
}

TEST_CASE("Formatting string input" , "[format_string]")
{
    const char* start = NULL;
    const char* end = NULL;

    SECTION("Valid string")
    {
        const char* str = "123456789";
        REQUIRE(m_bigint.format_string(str, &start, &end, 10) == 1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("Strings prefixed with '-' return -1 and point start to first valid char")
    {
        const char* str = "-123456789";
        REQUIRE(m_bigint.format_string(str, &start, &end, 10) == -1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("Strings prefixed with '0x' point start to first valid character")
    {
        const char* str = "0x123456abcdef";
        REQUIRE(m_bigint.format_string(str, &start, &end, 16) == 1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("leading/trailing whitespace")
    {
        const char* str = "    \t\n0xff   \n\t ";
        REQUIRE(m_bigint.format_string(str, &start, &end, 16) == 1);
        REQUIRE(*start == 'f');
        REQUIRE(*(end - 1) == 'f');
    }
    SECTION("string of only whitespace returns 0")
    {
        const char* str = "     ";
        REQUIRE(m_bigint.format_string(str, &start, &end, 10) == 0);
    }
    SECTION("String with whitespace, in prefixed with -0x")
    {
        const char* str = "   \n\t-0x1234fff  ";
        REQUIRE(m_bigint.format_string(str, &start, &end, 16) == -1);
        REQUIRE(*start == '1');
        REQUIRE(*(end - 1) == 'f');
    }
    SECTION("Leading zeroes are ignored")
    {
        const char* str = "0x000000100ff";
        REQUIRE(m_bigint.format_string(str, &start, &end, 16) == 1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("String with only valid prefixes returns 0")
    {
        const char* str = "    -0x";
        REQUIRE(m_bigint.format_string(str, &start, &end, 16) == 0);
    }
}

#endif

