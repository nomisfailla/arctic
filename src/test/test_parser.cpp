#include "catch.hpp"

#include "../lex/lexer.h"
#include "../parse/parser.h"

namespace
{
    bool expr_equals(const std::shared_ptr<arc::expr>& lhs, const std::shared_ptr<arc::expr>& rhs)
    {
        return *lhs == *rhs;
    }
    
    bool typespec_equals(const std::shared_ptr<arc::typespec>& lhs, const std::shared_ptr<arc::typespec>& rhs)
    {
        return *lhs == *rhs;
    }
    
    bool stmt_equals(const std::shared_ptr<arc::stmt>& lhs, const std::shared_ptr<arc::stmt>& rhs)
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
        auto tokens = arc::lexer(input).lex();
        auto expr = arc::parser(tokens, input).parse_expr();

        auto expected = arc::make_binary_expr(
            arc::binary_op::add,
            arc::make_integer_expr(1),
            arc::make_integer_expr(2)
        );

        REQUIRE(expr_equals(expr, expected));
    }

    SECTION("complex expression case") {
        arc::source_file input("-1 + 2 / 3 == hello.world - my.array[7]", true);
        auto tokens = arc::lexer(input).lex();
        auto expr = arc::parser(tokens, input).parse_expr();
    
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
        auto tokens = arc::lexer(input).lex();
        auto expr = arc::parser(tokens, input).parse_expr();
    
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

    SECTION("casting") {
        arc::source_file input("123 == ~my_data.field++ as u32 as u8", true);
        auto tokens = arc::lexer(input).lex();
        auto expr = arc::parser(tokens, input).parse_expr();

        auto expected = arc::make_binary_expr(
            arc::binary_op::equality,
            arc::make_integer_expr(123),
            arc::make_cast_expr(
                arc::make_cast_expr(
                    arc::make_unary_expr(
                        arc::unary_op::bitwise_not,
                        arc::make_unary_expr(
                            arc::unary_op::postfix_add,
                            arc::make_access_expr(
                                arc::make_name_expr("my_data"),
                                "field"
                            )
                        )
                    ),
                    arc::make_name_typespec("u32")
                ),
                arc::make_name_typespec("u8")
            )
        );

        REQUIRE(expr_equals(expr, expected));
    }

    SECTION("boolean literals") {
        arc::source_file input("true == false", true);
        auto tokens = arc::lexer(input).lex();
        auto expr = arc::parser(tokens, input).parse_expr();

        auto expected = arc::make_binary_expr(
            arc::binary_op::equality,
            arc::make_boolean_expr(true),
            arc::make_boolean_expr(false)
        );

        REQUIRE(expr_equals(expr, expected));
    }
}

TEST_CASE("type parsing completes or fails properly", "[parser]")
{
    SECTION("basic pass case") {
        arc::source_file input("u32 bool u32* u32** (u32, u8):bool ():u8* ():(u8):bool", true);
        auto tokens = arc::lexer(input).lex();
        REQUIRE_NOTHROW(arc::parser(tokens, input).parse_typespec());
    }

    SECTION("func type fail") {
        arc::source_file input("(u8", true);
        auto tokens = arc::lexer(input).lex();
        REQUIRE_THROWS(arc::parser(tokens, input).parse_typespec());
    }
}

TEST_CASE("type parsing produces correct ast", "[parser]")
{
    SECTION("basic types") {
        arc::source_file input("u32 ():none (u32):none (u32, bool):none", true);
        auto tokens = arc::lexer(input).lex();
        auto parser = arc::parser(tokens, input);

        std::vector<std::shared_ptr<arc::typespec>> expected_types = {
            arc::make_name_typespec("u32"),

            arc::make_func_typespec({
            }, arc::make_name_typespec("none")),

            arc::make_func_typespec({
                arc::make_name_typespec("u32")
            }, arc::make_name_typespec("none")),

            arc::make_func_typespec({
                arc::make_name_typespec("u32"),
                arc::make_name_typespec("bool")
            }, arc::make_name_typespec("none"))
        };

        for(const auto& expected : expected_types)
        {
            REQUIRE(typespec_equals(parser.parse_typespec(), expected));
        }
    }

    SECTION("pointer types") {
        arc::source_file input("*u32 **u32 ***u32 *():*u32", true);
        auto tokens = arc::lexer(input).lex();
        auto parser = arc::parser(tokens, input);

        std::vector<std::shared_ptr<arc::typespec>> expected_types = {
            arc::make_pointer_typespec(
                arc::make_name_typespec("u32")
            ),

            arc::make_pointer_typespec(
                arc::make_pointer_typespec(
                    arc::make_name_typespec("u32")
                )
            ),

            arc::make_pointer_typespec(
                arc::make_pointer_typespec(
                    arc::make_pointer_typespec(
                        arc::make_name_typespec("u32")
                    )
                )
            ),

            arc::make_pointer_typespec(
                arc::make_func_typespec({
                }, arc::make_pointer_typespec(
                    arc::make_name_typespec("u32")
                ))
            )
        };

        for(const auto& expected : expected_types)
        {
            REQUIRE(typespec_equals(parser.parse_typespec(), expected));
        }
    }
}

TEST_CASE("statement parsing produces correct ast", "[parser]")
{
    SECTION("complex if statement") {
        arc::source_file input(R"(
            if 1 == 1 {
                1 + 1;
            } elif 2 == 2 {
                let a = 2;
                let b: u32 = 2;
                let c: u32;
            } elif true {
                const a = 2;
                const b: u32 = 2;
                const c: u32;
            } else {
                return true;
            }
        )", true);

        auto tokens = arc::lexer(input).lex();
        auto parser = arc::parser(tokens, input);

        auto stmt = parser.parse_stmt();

        auto expected = arc::make_if_stmt({
            arc::if_branch(
                arc::make_binary_expr(
                    arc::binary_op::equality,
                    arc::make_integer_expr(1),
                    arc::make_integer_expr(1)
                ),
                {
                    arc::make_expr_stmt(
                        arc::make_binary_expr(
                            arc::binary_op::add,
                            arc::make_integer_expr(1),
                            arc::make_integer_expr(1)
                        )
                    )
                }
            ),
            arc::if_branch(
                arc::make_binary_expr(
                    arc::binary_op::equality,
                    arc::make_integer_expr(2),
                    arc::make_integer_expr(2)
                ),
                {
                    arc::make_let_stmt("a", nullptr, arc::make_integer_expr(2)),
                    arc::make_let_stmt("b", arc::make_name_typespec("u32"), arc::make_integer_expr(2)),
                    arc::make_let_stmt("c", arc::make_name_typespec("u32"), nullptr)
                }
            ),
            arc::if_branch(
                arc::make_boolean_expr(true),
                {
                    arc::make_const_stmt("a", nullptr, arc::make_integer_expr(2)),
                    arc::make_const_stmt("b", arc::make_name_typespec("u32"), arc::make_integer_expr(2)),
                    arc::make_const_stmt("c", arc::make_name_typespec("u32"), nullptr)
                }
            )
        }, {
            arc::make_return_stmt(
                arc::make_boolean_expr(true)
            )
        });

        REQUIRE(stmt_equals(stmt, expected));
    }
}
