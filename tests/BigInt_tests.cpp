/*
 * File: BigInt_tests.cpp
 *
 * Brief: Unit Tests for BigInt utility
 *
 * Author: Alexander DuPree
 *
 */

#include <iostream>
#include "catch.hpp"

extern "C" {
    #include "BigInt.h"
}

TEST_CASE("Constructing BigInt's", "[constructors]")
{
    SECTION("default constructor")
    {
        BigInt* num = new_BigInt();
        REQUIRE(num != NULL);

        free_BigInt(num);
    }
    SECTION("Value constructor")
    {
        int test_val = 666;
        BigInt* num = construct_BigInt(test_val);

        REQUIRE(compare_int(num, test_val) == 0);

        free_BigInt(num);
    }
}


