#include "catch.hpp"

#include "../lex/lexer.h"
#include "../parse/parser.h"

namespace
{
    bool expr_equals(const std::shared_ptr<arc::expr>& lhs, const std::shared_ptr<arc::expr>& rhs)
    {
        return *lhs == *rhs;
    }
}

TEST_CASE("expression parsing completes or fails properly", "[parser]")
{
    SECTION("basic pass case") {
        arc::source_file input("(1 + 2) + (3 - 4) + (5 * 6) + (7 / 8)", true);
        REQUIRE_NOTHROW(arc::parser(arc::lexer(input).lex(), input).parse_expr());
    }
    
    SECTION("expected expression fail") {
        arc::source_file input("()", true);
        REQUIRE_THROWS(arc::parser(arc::lexer(input).lex(), input).parse_expr());
    }
    
    SECTION("mismatched parenthesis fail") {
        arc::source_file input("1 + ((2)", true);
        REQUIRE_THROWS(arc::parser(arc::lexer(input).lex(), input).parse_expr());
    }
}

TEST_CASE("expression parsing produces correct ast", "[parser]")
{
    SECTION("basic expression case") {
        arc::source_file input("1 + 2", true);
        auto expr = arc::parser(arc::lexer(input).lex(), input).parse_expr();

        auto expected = arc::make_binary_expr(
            arc::binary_op::add,
            arc::make_integer_expr(1),
            arc::make_integer_expr(2)
        );

        REQUIRE(expr_equals(expr, expected));
    }

    SECTION("complex expression case") {
        arc::source_file input("-1 + 2 / 3 == hello.world - my.array[7]", true);
        auto expr = arc::parser(arc::lexer(input).lex(), input).parse_expr();
    
        auto expected = arc::make_binary_expr(
            arc::binary_op::equality,
            arc::make_binary_expr(
                arc::binary_op::add,
                arc::make_unary_expr(
                    arc::unary_op::negative,
                    arc::make_integer_expr(1)
                ),
                arc::make_binary_expr(
                    arc::binary_op::div,
                    arc::make_integer_expr(2),
                    arc::make_integer_expr(3)
                )
            ),
            arc::make_binary_expr(
                arc::binary_op::sub,
                arc::make_access_expr(
                    arc::make_name_expr("hello"),
                    "world"
                ),
                arc::make_index_expr(
                    arc::make_access_expr(
                        arc::make_name_expr("my"),
                        "array"
                    ),
                    arc::make_integer_expr(7)
                )
            )
        );

        REQUIRE(expr_equals(expr, expected));
    }

    SECTION("function call") {
        arc::source_file input("some.function(1, 2, 3)", true);
        auto expr = arc::parser(arc::lexer(input).lex(), input).parse_expr();
    
        auto expected = arc::make_call_expr(
            arc::make_access_expr(
                arc::make_name_expr("some"),
                "function"
            ),
            {
                arc::make_integer_expr(1),
                arc::make_integer_expr(2),
                arc::make_integer_expr(3)    
            }
        );

        REQUIRE(expr_equals(expr, expected));
    }
}
