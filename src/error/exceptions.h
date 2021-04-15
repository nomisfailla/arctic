#pragma once

#include <string>
#include <exception>

#include "../util/source_file.h"

namespace arc
{
    struct line_exception : public std::exception
    {
        const std::string error;
        const source_file& file;
        const source_pos position;

        line_exception(const std::string& error, const source_file& file, source_pos position)
            : error(error), file(file), position(position)
        {
        }
    };

    struct internal_exception : public std::exception
    {
        const std::string error;

        internal_exception(const std::string& error)
            : error(error)
        {
        }
    };
}
