#pragma once

#include <string>
#include <vector>

namespace arc
{
    class source_file
    {
    private:
        bool _exists;
        std::vector<char> _buffer;
    public:
        source_file(const std::string& path);

        std::string get_line(size_t line) const;

        const char* buffer() const;
        size_t size() const;
        bool exists() const;
    };
}
