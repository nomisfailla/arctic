#pragma once

#include "../util/source_file.h"

namespace arc
{
    struct source_pos
    {
        size_t line = 1;
        size_t column = 1;
    };

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

        char peek() const
        {
            return _buffer[_ptr];
        }

        source_pos position() const
        {
            return _cur_pos;
        }
    };

    class lexer
    {
    private:
        const source_file& _source;
        character_stream _stream;
    public:
        lexer(const source_file& source);

        void lex();
    };
}
