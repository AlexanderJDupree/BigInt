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

int_val any_int()
{
    std::mt19937 generator(get_seed());
    std::uniform_int_distribution<int_val> u_dist(INT_VAL_MIN, INT_VAL_MAX);
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
}

