/*
 * File: BigInt_tests.cpp
 *
 * Brief: Unit Tests for BigInt utility
 *
 * Author: Alexander DuPree
 *
 */


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

        REQUIRE(compare_int(num, 0) == 0);

        free_BigInt(num);
    }
    SECTION("Reserving space for a big int")
    {
        BigInt* num = reserve_BigInt(42);
        REQUIRE(buckets(num) == 42);
        free_BigInt(num);
    }
    SECTION("Value constructor instantiates BigInt with specified value")
    {
        int test_val = 123;
        BigInt* num = val_BigInt(test_val);

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

        BigInt* num = str_BigInt(test_str);
        REQUIRE(compare_uint(num, test_val) == 0);
        free_BigInt(num);
    }
    SECTION("Consturction without '0x' prefix on hexadecimal string")
    {
        int test_val = 0xff;
        const char* test_str = "ff";

        BigInt* num = str_BigInt(test_str);
        REQUIRE(compare_uint(num, test_val) == 0);
        free_BigInt(num);
    }
    SECTION("Invalid characters in string returns a BigInt with 0 value")
    {
        BigInt* num = str_BigInt("0xffghl");
        REQUIRE(compare_uint(num, 0) == 0);
        free_BigInt(num);
    }
}

TEST_CASE("Determining sign of a BigInt", "[sign]")
{
    SECTION("Negative BigInt returns sign < 0")
    {
        BigInt* num = str_BigInt("-0xffff");
        REQUIRE(sign(num) < 0);
        free_BigInt(num);
    }
    SECTION("Positive BigInt returns sign > 0")
    {
        BigInt* num = str_BigInt("0xffff");
        REQUIRE(sign(num) > 0);
        free_BigInt(num);
    }
    SECTION("Empty BigInt returns sign > 0")
    {
        BigInt* num = empty_BigInt();
        REQUIRE(sign(num) > 0);
        free_BigInt(num);
    }
    SECTION("Passing NULL to sign() returns 0")
    {
        BigInt* num = NULL;
        REQUIRE(sign(num) == 0);
    }
}

TEST_CASE("Determining the number of hex_digits in a BigInt", "[hex_digits]")
{
    SECTION("Number of hexadecimal digits")
    {
        // 81985529216486895 DEC
        BigInt* num = str_BigInt("0x123456789abcdef");
        REQUIRE(hex_digits(num) == 15);
        free_BigInt(num);
    }
    SECTION("Passing null to hex_digits return -1")
    {
        REQUIRE(hex_digits(NULL) == -1);
    }
    SECTION("Number of hex_digits on a zero valued bigint = 1")
    {
        BigInt* num = str_BigInt("0x000000000000000000");
        REQUIRE(num != NULL);
        REQUIRE(hex_digits(num) == 1);
        free_BigInt(num);
    }
}

#ifdef MOCKING_ENABLED

TEST_CASE("Constructing BigInts with values > BUCKET_MAX_SIZE", "[constructors]")
{
    // To make these tests easier to write/debug most of them are written only 
    // for the 8bit configuration, the final composite test tests all 
    // configurations.
    #ifdef BIGINT__8bit 
    SECTION("Even number of digits")
    {
        const char* test_str = "0x1000";
        BigInt* num = str_BigInt(test_str);

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

        BigInt* num = str_BigInt(test_str);
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

        BigInt* num = str_BigInt(test_str);
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

        BigInt* num = str_BigInt(test_str);
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
            0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 
            0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x1
        };
        int nbuckets = 16;
        #else
        #ifdef BIGINT__x64
        uint64_t expected_values[] = {
            0x8899aabbccddeeff, 0x111223344556677
        };
        int nbuckets = 2;
        #else // BIGINT__x86
        uint32_t expected_values[] = {
            0xccddeeff, 0x8899aabb, 0x44556677, 0x1112233
        };
        int nbuckets = 4;
        #endif
        #endif

        BigInt* num = str_BigInt(test_str);
        bucket_t* values = m_bigint.get_buckets(num);

        REQUIRE(buckets(num) == nbuckets);
        for(int i = 0; i < nbuckets; ++i)
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
            REQUIRE(m_bigint.char_to_num(i + '0', 10) == i);
        }
    }
    SECTION("Base 16 characters")
    {
        for (int i = 0; i < 10; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + '0', 16) == i);
        }
        for (int i = 10; i < 16; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + 'a' - 10, 16) == i);
        }
    }
    SECTION("Base 36 characters, is the upper limit for conversion")
    {
        for (int i = 0; i < 10; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + '0', 36) == i);
        }
        for (int i = 10; i < 36; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + 'a' - 10, 36) == i );
        }
    }
    SECTION("Base 2 Characters, is the lower limit for conversion")
    {
        for (int i = 0; i < 2; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + '0', 2) == i);
        }
    }
    SECTION("Arbitrary base in range 2 - 36 inclusive")
    {
        int i = 0;
        int base = any_int(2, 36);

        for (; i < base && i < 10; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + '0', base) == i);
        }

        for (; i < base; ++i)
        {
            REQUIRE(m_bigint.char_to_num(i + 'a' - 10, base) == i);
        }
    }
    SECTION("Invalid characters return -1")
    {
        REQUIRE(m_bigint.char_to_num('a', 10) == -1);
        REQUIRE(m_bigint.char_to_num('g', 16) == -1);
        REQUIRE(m_bigint.char_to_num('/', 36) == -1);
        REQUIRE(m_bigint.char_to_num('3', 2) == -1);
    }
}

TEST_CASE("Formatting string input" , "[format_string]")
{
    const char* start = NULL;
    const char* end = NULL;

    SECTION("Valid string")
    {
        const char* str = "123456789";
        REQUIRE(m_bigint.format_string(str, &start, &end) == 1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("Strings prefixed with '-' return -1 and point start to first valid char")
    {
        const char* str = "-123456789";
        REQUIRE(m_bigint.format_string(str, &start, &end) == -1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("Strings prefixed with '0x' point start to first valid character")
    {
        const char* str = "0x123456abcdef";
        REQUIRE(m_bigint.format_string(str, &start, &end) == 1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("leading/trailing whitespace")
    {
        const char* str = "    \t\n0xff   \n\t ";
        REQUIRE(m_bigint.format_string(str, &start, &end) == 1);
        REQUIRE(*start == 'f');
        REQUIRE(*(end - 1) == 'f');
    }
    SECTION("string of only whitespace returns 0")
    {
        const char* str = "     ";
        REQUIRE(m_bigint.format_string(str, &start, &end) == 0);
    }
    SECTION("String with whitespace, in prefixed with -0x")
    {
        const char* str = "   \n\t-0x1234fff  ";
        REQUIRE(m_bigint.format_string(str, &start, &end) == -1);
        REQUIRE(*start == '1');
        REQUIRE(*(end - 1) == 'f');
    }
    SECTION("Leading zeroes are ignored")
    {
        const char* str = "0x000000100ff";
        REQUIRE(m_bigint.format_string(str, &start, &end) == 1);
        REQUIRE(*start == '1');
        REQUIRE(*end == '\0');
    }
    SECTION("String with only valid prefixes returns 0")
    {
        const char* str = "    -0x";
        REQUIRE(m_bigint.format_string(str, &start, &end) == 0);
    }
}
#endif

