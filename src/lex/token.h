#pragma once

#include <variant>

#include "../util/source_file.h"

namespace arc
{
    enum class token_type
    {
        // keywords
        func,        // func
        return_,     // return
        if_,         // if
        elif,        // elif
        else_,       // else
        as,          // as
        let,         // let
        const_,      // const
        import,      // import
        namespace_,  // namespace
        alias,       // alias
        struct_,     // struct

        // literals
        boolean,     // true/false
        integer,     // 123456789
        float_,      // 3.14159

        identifier,  // local_var

        // seperators
        l_paren,     // (
        r_paren,     // )
        l_square,    // [
        r_square,    // ]
        l_curly,     // {
        r_curly,     // }
        comma,       // ,
        colon,       // :
        dbl_colon,   // ::
        semi_colon,  // ;
        dot,         // .

        // operators
        plus,        // +
        plus_eq,     // +=
        dbl_plus,    // ++
        minus,       // -
        minus_eq,    // -=
        dbl_minus,   // --
        asterix,     // *
        asterix_eq,  // *=
        slash,       // /
        slash_eq,    // /=
        less,        // <
        less_eq,     // <=
        dbl_less,    // <<
        dbl_less_eq, // <<=
        grtr,        // >
        grtr_eq,     // >=
        dbl_grtr,    // >>
        dbl_grtr_eq, // >>=
        caret,       // ^
        caret_eq,    // ^=
        pipe,        // |
        pipe_eq,     // |=
        dbl_pipe,    // ||
        amp,         // &
        amp_eq,      // &=
        dbl_amp,     // &&,
        tilde,       // ~
        eq,          // =
        dbl_eq,      // ==
        bang,        // !
        bang_eq,     // !=
        percent,     // %
        percent_eq,  // %=

        eof
    };

    using token_value = std::variant<double, uint64_t, bool, std::string>;

    struct token
    {
        const token_type type;
        const token_value value;
        const source_pos position;

        token(token_type type, token_value value, source_pos position)
            : type(type), value(value), position(position)
        {
        }

        double val_double() const
        {
            return std::get<double>(value);
        }

        uint64_t val_integer() const
        {
            return std::get<uint64_t>(value);
        }

        bool val_boolean() const
        {
            return std::get<bool>(value);
        }

        std::string val_string() const
        {
            return std::get<std::string>(value);
        }

        template<typename T>
        bool has_type() const
        {
            try
            {
                std::get<T>(value);
                return true;
            }
            catch(const std::bad_variant_access&)
            {
                return false;
            }
        }
    };
}
