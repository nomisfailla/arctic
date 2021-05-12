#pragma once

#include <vector>
#include <memory>

#include "../parse/ast.h"
#include "../error/exceptions.h"
#include "../util/source_file.h"

namespace arc
{
    class control_analyzer
    {
    private:
        std::vector<std::shared_ptr<decl>> _ast;

        std::vector<line_exception> _errors;
        const source_file& _source;
    public:
        control_analyzer(const std::vector<std::shared_ptr<decl>>& ast, const source_file& source);

        bool is_terminating(const std::shared_ptr<stmt>& s);
        bool is_terminating(const std::vector<std::shared_ptr<stmt>>& block);

        void analyze_func(const std::shared_ptr<decl_func>& decl);

        std::vector<line_exception> analyze();
    };
}
