#include "control_analyzer.h"

#include "../util/casting.h"

namespace arc
{
    control_analyzer::control_analyzer(const std::vector<std::shared_ptr<decl>>& ast, const source_file& source)
        : _ast(ast), _source(source)
    {
    }

    bool control_analyzer::is_terminating(const std::shared_ptr<stmt>& s)
    {
        if(auto stmt = arc::is<stmt_return>(s))
        {
            return true;
        }

        if(auto stmt = arc::is<stmt_if>(s))
        {
            bool can_fall_through = false;
            for(const auto& branch : stmt->if_branches)
            {
                if(!is_terminating(branch.body))
                {
                    can_fall_through = true;
                    break;
                }
            }

            can_fall_through |= !is_terminating(stmt->else_branch);
            return !can_fall_through;
        }

        if(auto stmt = arc::is<stmt_block>(s))
        {
            return is_terminating(stmt->block);
        }

        return false;
    }

    bool control_analyzer::is_terminating(const std::vector<std::shared_ptr<stmt>>& block)
    {
        bool found_return = false;
        for(const auto& s : block)
        {
            if(found_return)
            {
                _errors.push_back(line_exception("unreachable code", _source, s->position));
                break;
            }

            if(is_terminating(s))
            {
                found_return = true;
            }
        }

        return found_return;
    }

    void control_analyzer::analyze_func(const std::shared_ptr<decl_func>& decl)
    {
        // TODO: Only if function has a return type
        if(!is_terminating(decl->body))
        {
            _errors.push_back(line_exception("not all control paths return a value", _source, decl->position));
        }
    }

    std::vector<line_exception> control_analyzer::analyze()
    {
        for(const auto& d : _ast)
        {
            if(auto decl = arc::is<decl_func>(d))
            {
                analyze_func(decl);
            }
        }
        return _errors;
    }
}
