#pragma once

#include <functional>

#include "token.h"
#include "../error/exceptions.h"
#include "../util/source_file.h"

namespace arc
{
    class character_stream
    {
    private:
        const char* _buffer;
        const size_t _size;

        size_t _ptr;

        source_pos _cur_pos;
    public:
        character_stream(const char* buffer, size_t size)
            : _buffer(buffer), _size(size), _ptr(0)
        {
        }

        bool has_next() const
        {
            return _ptr < _size;
        }

        char next()
        {
            _cur_pos.column++;
            char c = _buffer[_ptr++];
            if(c == '\n')
            {
                _cur_pos.column = 1;
                _cur_pos.line++;
            }
            return c;
        }

        char peek(int o = 0) const
        {
            return _buffer[_ptr + o];
        }

        source_pos position() const
        {
            return _cur_pos;
        }
    };

    struct lexed_number
    {
        bool is_float;
        uint64_t val_int;
        double val_float;
    };

    class lexer
    {
    private:
        const source_file& _source;
        character_stream _stream;
    public:
        lexer(const source_file& source);

        std::vector<token> lex();
    private:
        std::string parse_identifier();
        uint64_t parse_number(uint32_t base, std::function<bool(char)> predicate);
        lexed_number parse_number();
    };
}
