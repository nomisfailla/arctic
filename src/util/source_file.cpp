#include "source_file.h"

#include <fstream>
#include <iterator>
#include <algorithm>

namespace arc
{
    source_file::source_file(const std::string& path)
        : _exists(false)
    {
        std::ifstream input(path, std::ios::binary);
        if(input.is_open())
        {
            _buffer = std::vector<char>(std::istreambuf_iterator<char>(input), {});
            _exists = true;
        }
    }

    std::string source_file::get_line(size_t line) const
    {
        size_t idx = 0;
        size_t cur_line = 1;
        for(auto c : _buffer)
        {
            if(cur_line == line) { break; }
            idx++;
            if(c == '\n') { cur_line++; }
        }

        auto end = std::find(_buffer.begin() + idx, _buffer.end(), '\n');
        return std::string(_buffer.begin() + idx, end);
    }

    const char* source_file::buffer() const
    {
        return _buffer.data();
    }

    size_t source_file::size() const
    {
        return _buffer.size();
    }

    bool source_file::exists() const
    {
        return _exists;
    }
}
