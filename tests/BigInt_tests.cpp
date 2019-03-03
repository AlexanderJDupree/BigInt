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

TEST_CASE("Constructing BigInt's", "[constructors]")
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
    SECTION("Value constructor with negative value")
    {
        int test_val = -123;
        BigInt* num = val_BigInt(test_val);

        REQUIRE(sign(num) < 0);
        REQUIRE(compare_int(num, test_val) == 0);

        free_BigInt(num);
    }
    SECTION("Value contructor with INT_VAL_MAX")
    {
        BigInt* num = val_BigInt(INT_VAL_MAX);

        REQUIRE(compare_int(num, INT_VAL_MAX) == 0);
        free_BigInt(num);
    }
    SECTION("Value constructor with INT_VAL_MIN")
    {
        BigInt* num = val_BigInt(INT_VAL_MIN);

        REQUIRE(sign(num) < 0);
        REQUIRE(compare_int(num, INT_VAL_MIN) == 0);
        free_BigInt(num);
    }
    SECTION("Constructing BigInts with string literals within bounds of int_val")
    {
        int test_val = 123;
        const char* test_str = "123";
        BigInt* num = str_BigInt(test_str, 10);

        REQUIRE(compare_int(num, test_val) == 0);
        free_BigInt(num);
    }
    SECTION("Negative string literal within bounds of int_val")
    {
        int test_val = -123;
        const char* test_str = "-123";

        BigInt* num = str_BigInt(test_str, 10);

        REQUIRE(sign(num) < 0);
        REQUIRE(compare_int(num, test_val) == 0);
        free_BigInt(num);
    }
    SECTION("Construction with hexadecimal string literal")
    {
        int test_val = 0xff;
        const char* test_str = "0xff";

        BigInt* num = str_BigInt(test_str, 16);
        REQUIRE(compare_int(num, test_val) == 0);
        free_BigInt(num);
    }
    /*
    SECTION("Construction with invalid base does not allocate bigint")
    {
        REQUIRE(str_BigInt("0xff", 42) == NULL);
    }
    SECTION("Invalid characters in string construct big int with 0 value")
    {
        const char* test_str = "1234ancvb";

        BigInt* num = str_BigInt(test_str, 10);

        REQUIRE(compare_int(num, 0) == 0);

        free_BigInt(num);
    }
    */
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

