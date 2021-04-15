#pragma once

#include <vector>
#include <initializer_list>
#include <functional>

#include "../lex/token.h"
#include "ast.h"
#include "../error/exceptions.h"

namespace arc
{
    class token_stream
    {
    private:
        const std::vector<token>& _tokens;

        size_t _ptr;
    public:
        token_stream(const std::vector<token>& tokens)
            : _tokens(tokens), _ptr(0)
        {
        }

        source_pos position() const
        {
            return _tokens[_ptr].position;
        }

        token next()
        {
            return _tokens[_ptr++];
        }

        token_type peek_type() const
        {
            return _tokens[_ptr].type;
        }

        token expect_one_of(const std::initializer_list<token_type>& types, std::function<void()> cb)
        {
            for(const auto& expected : types)
            {
                if(peek_type() == expected)
                {
                    return next();
                }
            }

            cb();
            return _tokens[_ptr];
        }

        token expect(token_type type, std::function<void()> cb)
        {
            if(peek_type() == type)
            {
                return next();
            }

            cb();
            return _tokens[_ptr];
        }
        
        bool next_is_one_of(const std::initializer_list<token_type>& types)
        {
            for(const auto& type : types)
            {
                if(peek_type() == type)
                {
                    return true;
                }
            }
            return false;
        }

        bool next_is(token_type type)
        {
            if(peek_type() == type)
            {
                return true;
            }
            return false;
        }
    };

    class parser
    {
    private:
        const source_file& _source;
        token_stream _stream;
    public:
        parser(const std::vector<token>& tokens, const source_file& source);

        std::shared_ptr<expr> parse();
    private:
        line_exception parse_error(const std::string& msg);

        std::shared_ptr<expr> parse_expr0();
        std::shared_ptr<expr> parse_expr1();
        std::shared_ptr<expr> parse_expr2();
        std::shared_ptr<expr> parse_expr3();
        std::shared_ptr<expr> parse_expr4();
        std::shared_ptr<expr> parse_expr5();
        std::shared_ptr<expr> parse_expr6();
        std::shared_ptr<expr> parse_expr7();
        std::shared_ptr<expr> parse_expr8();
        std::shared_ptr<expr> parse_expr9();
        std::shared_ptr<expr> parse_expr10();
        std::shared_ptr<expr> parse_expr11();
        std::shared_ptr<expr> parse_expr12();
        std::shared_ptr<expr> parse_expr13();
        std::shared_ptr<expr> parse_expr();
    };
}
