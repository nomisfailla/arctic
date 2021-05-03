#include "lexer.h"

#include <iostream>

namespace
{
    static bool is_whitespace(char c)
    {
        return c == 0x20 || c == 0x0A || c == 0x0D || c == 0x09;
    }

    static bool is_number(char c)
    {
        return c >= '0' && c <= '9';
    }

    static bool is_letter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    static bool is_ident_start_char(char c)
    {
        return is_letter(c) || c == '_';
    }

    static bool is_ident_char(char c)
    {
        return is_ident_start_char(c) || is_number(c);
    }

    static bool is_decimal_number(char c)
    {
        return is_number(c);
    }

    static bool is_hex_number(char c)
    {
        return is_number(c) || (c >= 'a' && c <= 'F') || (c >= 'A' && c <= 'F');
    }

    static bool is_octal_number(char c)
    {
        return c >= '0' && c <= '7';
    }

    static bool is_binary_number(char c)
    {
        return c == '0' || c == '1';
    }

    static uint64_t convert_char(char c)
    {
        if(is_number(c)) { return uint64_t(c - '0'); }
        if(c >= 'a' && c <= 'f') { return uint64_t(c - 'a') + 10; }
        if(c >= 'A' && c <= 'F') { return uint64_t(c - 'A') + 10; }
        return 0;
    }
}

namespace arc
{
    lexer::lexer(const source_file& source)
        : _source(source), _stream(source.buffer(), source.size())
    {
    }

    std::string lexer::parse_identifier()
    {
        std::string ident = "";
        while(_stream.has_next() && is_ident_char(_stream.peek()))
        {
            ident += _stream.next();
        }

        return ident;
    }

    uint64_t lexer::parse_number(uint32_t base, std::function<bool(char)> predicate)
    {
        if(!_stream.has_next() || !predicate(_stream.peek()))
        {
            _errors.push_back(line_exception("malformed integer literal", _source, _stream.position()));
            return 0;
        }

        uint64_t val = 0;
        while(_stream.has_next() && predicate(_stream.peek()))
        {
            val *= base;
            val += convert_char(_stream.next());
        }

        return val;
    }

    lexed_number lexer::parse_number()
    {
        if(_stream.peek() == '0')
        {
            if(_stream.peek(1) == 'b')
            {
                _stream.next(); // eat the leading 0
                _stream.next(); // eat the leading b
                auto v = parse_number(2, is_binary_number);

                return {
                    .is_float = false,
                    .val_int = v
                };
            }
            else if(_stream.peek(1) == 'o')
            {
                _stream.next(); // eat the leading 0
                _stream.next(); // eat the leading o
                auto v = parse_number(8, is_octal_number);
                
                return {
                    .is_float = false,
                    .val_int = v
                };
            }
            else if(_stream.peek(1) == 'x')
            {
                _stream.next(); // eat the leading 0
                _stream.next(); // eat the leading x
                auto v = parse_number(16, is_hex_number);
                
                return {
                    .is_float = false,
                    .val_int = v
                };
            }
        }

        auto v = parse_number(10, is_decimal_number);
        if(_stream.peek() == '.')
        {
            _stream.next();

            double f = 0.0;
            double m = 0.1;

            while(_stream.has_next() && is_decimal_number(_stream.peek()))
            {
                f += double(convert_char(_stream.next())) * m;
                m *= 0.1;
            }

            return {
                .is_float = true,
                .val_float = double(v) + f
            };
        }
        
        return {
            .is_float = false,
            .val_int = v
        };
    }

    lexer_result lexer::lex()
    {
        std::vector<token> tokens;

        while(_stream.has_next())
        {
            auto cur_pos = _stream.position();

            auto emit = [&](token_type type, token_value value = uint64_t(0)) {
                tokens.emplace_back(type, value, cur_pos);
            };

            if(is_whitespace(_stream.peek()))
            {
                _stream.next();
                continue;
            }

            if(is_ident_start_char(_stream.peek()))
            {
                auto ident = parse_identifier();
                if(ident == "func"     ) { emit(token_type::func,       "func"     ); continue; }
                if(ident == "return"   ) { emit(token_type::return_,    "return"   ); continue; }
                if(ident == "if"       ) { emit(token_type::if_,        "if"       ); continue; }
                if(ident == "elif"     ) { emit(token_type::elif,       "elif"     ); continue; }
                if(ident == "else"     ) { emit(token_type::else_,      "else"     ); continue; }
                if(ident == "as"       ) { emit(token_type::as,         "as"       ); continue; }
                if(ident == "let"      ) { emit(token_type::let,        "let"      ); continue; }
                if(ident == "const"    ) { emit(token_type::const_,     "const"    ); continue; }
                if(ident == "import"   ) { emit(token_type::import,     "import"   ); continue; }
                if(ident == "namespace") { emit(token_type::namespace_, "namespace"); continue; }
                if(ident == "alias"    ) { emit(token_type::alias,      "alias"    ); continue; }
                if(ident == "struct"   ) { emit(token_type::struct_,    "struct"   ); continue; }
                if(ident == "true"     ) { emit(token_type::boolean,    true       ); continue; }
                if(ident == "false"    ) { emit(token_type::boolean,    false      ); continue; }

                emit(token_type::identifier, ident);
                continue;
            }

            if(is_number(_stream.peek()))
            {
                auto number = parse_number();
                if(number.is_float)
                {
                    emit(token_type::float_, number.val_float);
                }
                else
                {
                    emit(token_type::integer, number.val_int);
                }
                continue;
            }

            auto single = [&](char c, token_type type) {
                if(_stream.peek() == c)
                {
                    _stream.next();
                    emit(type);
                    return true;
                }
                return false;
            };

            auto dbl = [&](const char* pattern, token_type t1, token_type t2) {
                if(_stream.peek() == pattern[0])
                {
                    _stream.next();
                    if(_stream.peek() == pattern[1])
                    {
                        _stream.next();
                        emit(t2);
                    }
                    else
                    {
                        emit(t1);
                    }
                    return true;
                }
                return false;
            };

            auto triple = [&](const char* pattern, token_type t1, token_type t2, token_type t3) {
                if(_stream.peek() == pattern[0])
                {
                    _stream.next();
                    if(_stream.peek() == pattern[1])
                    {
                        _stream.next();
                        emit(t2);
                    }
                    else if(_stream.peek() == pattern[2])
                    {
                        _stream.next();
                        emit(t3);
                    }
                    else
                    {
                        emit(t1);
                    }
                    return true;
                }
                return false;
            };

            auto quad = [&](const char* pattern, token_type t1, token_type t2, token_type t3, token_type t4) {
                if(_stream.peek() == pattern[0])
                {
                    _stream.next();
                    if(_stream.peek() == pattern[1])
                    {
                        _stream.next();
                        emit(t2);
                    }
                    else if(_stream.peek() == pattern[2])
                    {
                        _stream.next();

                        if(_stream.peek() == pattern[3])
                        {
                            _stream.next();
                            emit(t4);
                        }
                        else
                        {
                            emit(t3);
                        }
                    }
                    else
                    {
                        emit(t1);
                    }
                    return true;
                }
                return false;
            };

            if(single('(', token_type::l_paren   )) { continue; }
            if(single(')', token_type::r_paren   )) { continue; }
            if(single('[', token_type::l_square  )) { continue; }
            if(single(']', token_type::r_square  )) { continue; }
            if(single('{', token_type::l_curly   )) { continue; }
            if(single('}', token_type::r_curly   )) { continue; }
            if(single(',', token_type::comma     )) { continue; }
            if(single('~', token_type::tilde     )) { continue; }
            if(single('.', token_type::dot       )) { continue; }
            if(single(';', token_type::semi_colon)) { continue; }

            if(dbl("::", token_type::colon,   token_type::dbl_colon )) { continue; }
            if(dbl("*=", token_type::asterix, token_type::asterix_eq)) { continue; }
            if(dbl("/=", token_type::slash,   token_type::slash_eq  )) { continue; }
            if(dbl("^=", token_type::caret,   token_type::caret_eq  )) { continue; }
            if(dbl("==", token_type::eq,      token_type::dbl_eq    )) { continue; }
            if(dbl("!=", token_type::bang,    token_type::bang_eq   )) { continue; }
            if(dbl("%=", token_type::percent, token_type::percent_eq)) { continue; }

            if(triple("+=+", token_type::plus,  token_type::plus_eq,  token_type::dbl_plus )) { continue; }
            if(triple("-=-", token_type::minus, token_type::minus_eq, token_type::dbl_minus)) { continue; }
            if(triple("|=|", token_type::pipe,  token_type::pipe_eq,  token_type::dbl_pipe )) { continue; }
            if(triple("&=&", token_type::amp,   token_type::amp_eq,   token_type::dbl_amp  )) { continue; }

            if(quad(">=>=", token_type::grtr, token_type::grtr_eq, token_type::dbl_grtr, token_type::dbl_grtr_eq)) { continue; }
            if(quad("<=<=", token_type::less, token_type::less_eq, token_type::dbl_less, token_type::dbl_less_eq)) { continue; }
            
            _errors.push_back(line_exception("unexpected character", _source, cur_pos));
            _stream.next();
        }

        tokens.emplace_back(token_type::eof, "eof", _stream.position());

        return lexer_result(tokens, _errors);
    }
}
