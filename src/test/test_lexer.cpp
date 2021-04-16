#include "catch.hpp"

static int multiply(int a, int b)
{
    return a * b;
}

TEST_CASE("Numbers are multiplied", "[lexer]")
{
    REQUIRE(multiply(2, 4) == 8);
    REQUIRE(multiply(1, 0) == 0);
}
