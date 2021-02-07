#include "lexer.h"

#include <iostream>

namespace
{
    static bool is_whitespace(char c)
    {
        return c == 0x20 || c == 0x0A || c == 0x0D || c == 0x09;
    }
}

namespace arc
{
    lexer::lexer(const source_file& source)
        : _source(source), _stream(source.buffer(), source.size())
    {
    }
    
    void lexer::lex()
    {
        while(_stream.has_next())
        {
            if(is_whitespace(_stream.peek()))
            {
                _stream.next();
                continue;
            }

            auto _cur_pos = _stream.position();
            if(_stream.next() == 'u')
            {
                std::cout << "found u at " << _cur_pos.line << ":" << _cur_pos.column << std::endl;
                std::cout << _source.get_line(_cur_pos.line) << std::endl;
            }
        }
    }
}
