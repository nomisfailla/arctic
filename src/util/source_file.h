#pragma once

#include <string>
#include <vector>

namespace arc
{
    struct source_pos
    {
        size_t line = 1;
        size_t column = 1;
    };

    class source_file
    {
    private:
        bool _exists;
        std::string _path;
        std::vector<char> _buffer;
    public:
        source_file(const std::string& path, bool is_content = false);

        std::string get_line(size_t line) const;

        std::string path() const;
        const char* buffer() const;
        size_t size() const;
        bool exists() const;
    };
}
