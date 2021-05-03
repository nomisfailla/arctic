#include "catch.hpp"

#include "../lex/lexer.h"

TEST_CASE("lexer completes or fails properly", "[lexer]")
{
    SECTION("basic pass case") {
        arc::source_file input("func main(a: u32, b: u32) : none { return (a / 2) + b << 2; }", true);
        auto result = arc::lexer(input).lex();

        REQUIRE(result.succeeded());
        REQUIRE(result.tokens.size() == 27);
    }

    SECTION("number literals pass case") {
        arc::source_file input("12345 0b101 0o0713 0xFFAEF 3.1415", true);
        auto result = arc::lexer(input).lex();

        REQUIRE(result.succeeded());
        REQUIRE(result.tokens.size() == 6);
    }

    SECTION("unknown character fail") {
        arc::source_file input("func # 123", true);
        auto result = arc::lexer(input).lex();

        REQUIRE(!result.succeeded());
    }

    SECTION("malformed binary literal fail") {
        arc::source_file input("0b2", true);
        auto result = arc::lexer(input).lex();

        REQUIRE(!result.succeeded());
    }

    SECTION("malformed octal literal fail") {
        arc::source_file input("0oF", true);
        auto result = arc::lexer(input).lex();

        REQUIRE(!result.succeeded());
    }

    SECTION("malformed hex literal fail") {
        arc::source_file input("0xP", true);
        auto result = arc::lexer(input).lex();

        REQUIRE(!result.succeeded());
    }
}

TEST_CASE("lexer lexes correct token types", "[lexer]")
{
    SECTION("literal types") {
        arc::source_file input("true false 12345 0b101 0o0713 0xFFAEF 3.1415 hello", true);
        auto tokens = arc::lexer(input).lex().tokens;

        auto expected_types = {
            arc::token_type::boolean,
            arc::token_type::boolean,
            arc::token_type::integer,
            arc::token_type::integer,
            arc::token_type::integer,
            arc::token_type::integer,
            arc::token_type::float_,
            arc::token_type::identifier,
            arc::token_type::eof
        };

        REQUIRE(tokens.size() == expected_types.size());

        int i = 0;
        for(auto expected : expected_types)
        {
            REQUIRE(tokens[i].type == expected);
            i++;
        }
    }

    SECTION("keywords") {
        arc::source_file input("func return if as", true);
        auto tokens = arc::lexer(input).lex().tokens;

        auto expected_types = {
            arc::token_type::func,
            arc::token_type::return_,
            arc::token_type::if_,
            arc::token_type::as,
            arc::token_type::eof
        };

        REQUIRE(tokens.size() == expected_types.size());

        int i = 0;
        for(auto expected : expected_types)
        {
            REQUIRE(tokens[i].type == expected);
            i++;
        }
    }

    SECTION("seperators") {
        arc::source_file input("() [] {} , : :: ; .", true);
        auto tokens = arc::lexer(input).lex().tokens;

        auto expected_types = {
            arc::token_type::l_paren,
            arc::token_type::r_paren,
            arc::token_type::l_square,
            arc::token_type::r_square,
            arc::token_type::l_curly,
            arc::token_type::r_curly,
            arc::token_type::comma,
            arc::token_type::colon,
            arc::token_type::dbl_colon,
            arc::token_type::semi_colon,
            arc::token_type::dot,
            arc::token_type::eof
        };

        REQUIRE(tokens.size() == expected_types.size());

        int i = 0;
        for(auto expected : expected_types)
        {
            REQUIRE(tokens[i].type == expected);
            i++;
        }
    }

    SECTION("operators") {
        arc::source_file input("+ += ++ - -= -- * *= / /= < <= << <<= > >= >> >>= ^ ^= | |= || & &= && ~ = == ! != % %=", true);
        auto tokens = arc::lexer(input).lex().tokens;

        auto expected_types = {
            arc::token_type::plus,
            arc::token_type::plus_eq,
            arc::token_type::dbl_plus,
            arc::token_type::minus,
            arc::token_type::minus_eq,
            arc::token_type::dbl_minus,
            arc::token_type::asterix,
            arc::token_type::asterix_eq,
            arc::token_type::slash,
            arc::token_type::slash_eq,
            arc::token_type::less,
            arc::token_type::less_eq,
            arc::token_type::dbl_less,
            arc::token_type::dbl_less_eq,
            arc::token_type::grtr,
            arc::token_type::grtr_eq,
            arc::token_type::dbl_grtr,
            arc::token_type::dbl_grtr_eq,
            arc::token_type::caret,
            arc::token_type::caret_eq,
            arc::token_type::pipe,
            arc::token_type::pipe_eq,
            arc::token_type::dbl_pipe,
            arc::token_type::amp,
            arc::token_type::amp_eq,
            arc::token_type::dbl_amp,
            arc::token_type::tilde,
            arc::token_type::eq,
            arc::token_type::dbl_eq,
            arc::token_type::bang,
            arc::token_type::bang_eq,
            arc::token_type::percent,
            arc::token_type::percent_eq,
            arc::token_type::eof
        };

        REQUIRE(tokens.size() == expected_types.size());

        int i = 0;
        for(auto expected : expected_types)
        {
            REQUIRE(tokens[i].type == expected);
            i++;
        }
    }

    SECTION("strange operators") {
        arc::source_file input(">>>=== ||= <<<", true);
        auto tokens = arc::lexer(input).lex().tokens;

        auto expected_types = {
            arc::token_type::dbl_grtr,
            arc::token_type::grtr_eq,
            arc::token_type::dbl_eq,
            arc::token_type::dbl_pipe,
            arc::token_type::eq,
            arc::token_type::dbl_less,
            arc::token_type::less,
            arc::token_type::eof
        };

        REQUIRE(tokens.size() == expected_types.size());

        int i = 0;
        for(auto expected : expected_types)
        {
            REQUIRE(tokens[i].type == expected);
            i++;
        }
    }
}

TEST_CASE("lexer parses correct token values", "[lexer]")
{
    SECTION("booleans") {
        arc::source_file input("true false", true);
        auto tokens = arc::lexer(input).lex().tokens;

        REQUIRE(tokens.size() == 3);

        auto& t1 = tokens[0];
        REQUIRE(t1.has_type<bool>());
        REQUIRE(t1.val_boolean() == true);

        auto& t2 = tokens[1];
        REQUIRE(t2.has_type<bool>());
        REQUIRE(t2.val_boolean() == false);
    }

    SECTION("integers") {
        arc::source_file input("12345 0b1011 0o4526 0xABC2", true);
        auto tokens = arc::lexer(input).lex().tokens;

        REQUIRE(tokens.size() == 5);

        auto& t1 = tokens[0];
        REQUIRE(t1.has_type<uint64_t>());
        REQUIRE(t1.val_integer() == 12345);

        auto& t2 = tokens[1];
        REQUIRE(t2.has_type<uint64_t>());
        REQUIRE(t2.val_integer() == 11);

        auto& t3 = tokens[2];
        REQUIRE(t3.has_type<uint64_t>());
        REQUIRE(t3.val_integer() == 2390);

        auto& t4 = tokens[3];
        REQUIRE(t4.has_type<uint64_t>());
        REQUIRE(t4.val_integer() == 0xABC2);
    }

    SECTION("floats") {
        arc::source_file input("3.1415926535 2.0 2.5", true);
        auto tokens = arc::lexer(input).lex().tokens;

        REQUIRE(tokens.size() == 4);

        auto& t1 = tokens[0];
        REQUIRE(t1.has_type<double>());
        REQUIRE(t1.val_double() == Approx(3.1415926535));

        auto& t2 = tokens[1];
        REQUIRE(t2.has_type<double>());
        REQUIRE(t2.val_double() == Approx(2.0));

        auto& t3 = tokens[2];
        REQUIRE(t3.has_type<double>());
        REQUIRE(t3.val_double() == Approx(2.5));
    }

    SECTION("identifiers") {
        arc::source_file input("test.b _a12 7test", true);
        auto tokens = arc::lexer(input).lex().tokens;

        REQUIRE(tokens.size() == 7);

        auto& t1 = tokens[0];
        REQUIRE(t1.has_type<std::string>());
        REQUIRE(t1.val_string() == "test");

        auto& t2 = tokens[2];
        REQUIRE(t2.has_type<std::string>());
        REQUIRE(t2.val_string() == "b");

        auto& t3 = tokens[3];
        REQUIRE(t3.has_type<std::string>());
        REQUIRE(t3.val_string() == "_a12");

        auto& t4 = tokens[5];
        REQUIRE(t4.has_type<std::string>());
        REQUIRE(t4.val_string() == "test");
    }
}
