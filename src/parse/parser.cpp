#include "parser.h"

namespace
{
    arc::unary_op classify_unary_op(arc::token_type type, bool is_postfix = false)
    {
        switch(type)
        {
        case arc::token_type::plus:    return arc::unary_op::positive;
	    case arc::token_type::minus:   return arc::unary_op::negative;
	    case arc::token_type::asterix: return arc::unary_op::deref;
	    case arc::token_type::amp:     return arc::unary_op::address;
	    case arc::token_type::tilde:   return arc::unary_op::bitwise_not;
	    case arc::token_type::bang:    return arc::unary_op::logical_not;
	    case arc::token_type::dbl_plus: {
            return is_postfix ? arc::unary_op::postfix_add : arc::unary_op::prefix_add;
        } break;
        case arc::token_type::dbl_minus: {
            return is_postfix ? arc::unary_op::postfix_sub : arc::unary_op::prefix_sub;
        } break;
        }

        throw arc::internal_exception("invalid token type for unary operator");
    }

    arc::binary_op classify_binary_op(arc::token_type type)
    {
        switch(type)
        {
        case arc::token_type::plus:        return arc::binary_op::add;
		case arc::token_type::minus:       return arc::binary_op::sub;
		case arc::token_type::asterix:     return arc::binary_op::mul;
		case arc::token_type::slash:       return arc::binary_op::div;
		case arc::token_type::percent:     return arc::binary_op::mod;
		case arc::token_type::dbl_less:    return arc::binary_op::lshift;
		case arc::token_type::dbl_grtr:    return arc::binary_op::rshift;
		case arc::token_type::less:        return arc::binary_op::less;
		case arc::token_type::less_eq:     return arc::binary_op::less_eq;
		case arc::token_type::grtr:        return arc::binary_op::greater;
		case arc::token_type::grtr_eq:     return arc::binary_op::greater_eq;
		case arc::token_type::dbl_eq:      return arc::binary_op::equality;
		case arc::token_type::bang_eq:     return arc::binary_op::inequality;
		case arc::token_type::amp:         return arc::binary_op::bitwise_and;
		case arc::token_type::caret:       return arc::binary_op::bitwise_xor;
		case arc::token_type::pipe:        return arc::binary_op::bitwise_or;
		case arc::token_type::dbl_amp:     return arc::binary_op::logical_and;
		case arc::token_type::dbl_pipe:    return arc::binary_op::logical_or;
		case arc::token_type::eq:          return arc::binary_op::assign;
		case arc::token_type::plus_eq:     return arc::binary_op::add_assign;
		case arc::token_type::minus_eq:    return arc::binary_op::sub_assign;
		case arc::token_type::asterix_eq:  return arc::binary_op::mul_assign;
		case arc::token_type::slash_eq:    return arc::binary_op::div_assign;
		case arc::token_type::percent_eq:  return arc::binary_op::mod_assign;
		case arc::token_type::dbl_less_eq: return arc::binary_op::lshift_assign;
		case arc::token_type::dbl_grtr_eq: return arc::binary_op::rshift_assign;
		case arc::token_type::amp_eq:      return arc::binary_op::bitwise_and_assign;
		case arc::token_type::caret_eq:    return arc::binary_op::bitwise_xor_assign;
		case arc::token_type::pipe_eq:     return arc::binary_op::bitwise_or_assign;
        }

        throw arc::internal_exception("invalid token type for binary operator");
    }
}

namespace arc
{
    parser::parser(const std::vector<token>& tokens, const source_file& source)
        : _stream(tokens), _source(source)
    {
    }

    line_exception parser::parse_error(const std::string& msg)
    {
        return line_exception(msg, _source, _stream.position());
    }

    std::shared_ptr<expr> parser::parse_expr0()
    {
        auto token = _stream.expect_one_of({
            token_type::boolean,
            token_type::integer,
            token_type::float_,
            token_type::identifier,
            token_type::l_paren
        }, [&]() { throw parse_error("expected expression"); });

        switch(token.type)
        {
        case token_type::boolean: {
            return make_boolean_expr(token.val_boolean());
        } break;
        case token_type::integer: {
            return make_integer_expr(token.val_integer());
        } break;
        case token_type::float_: {
            return make_integer_expr(token.val_double());
        } break;
        case token_type::identifier: {
            return make_name_expr(token.val_string());
        } break;
        case token_type::l_paren: {
            auto expr = parse_expr();
            _stream.expect(token_type::r_paren, [&](){ throw parse_error("expected ')'"); });
            return expr;
        } break;
        }
    }
    
    // expr0(expr)
    // expr0[expr]
    // expr0.access
    // expr0++
    // expr0--
    std::shared_ptr<expr> parser::parse_expr1()
    {
        auto base_expr = parse_expr0();

        while(_stream.next_is_one_of({
            token_type::l_paren,
            token_type::l_square,
            token_type::dot,
            token_type::dbl_plus,
            token_type::dbl_minus
        })) {
            switch(_stream.peek_type())
            {
            case token_type::l_paren: {
                std::vector<std::shared_ptr<expr>> args;

                _stream.next();
                if(!_stream.next_is(token_type::r_paren))
                {
                    args.push_back(parse_expr());
                    while(_stream.next_is(token_type::comma))
                    {
                        _stream.next();
                        args.push_back(parse_expr());
                    }
                }
                _stream.expect(token_type::r_paren, [&]() { throw parse_error("expected ')'"); });

                base_expr = make_call_expr(base_expr, args);
            } break;
            case token_type::l_square: {
                _stream.next();
                auto index = parse_expr();
                _stream.expect(token_type::r_square, [&]() { throw parse_error("expected ']'"); });

                base_expr = make_index_expr(base_expr, index);
            } break;
            case token_type::dot: {
                _stream.next();
                token field = _stream.expect(token_type::identifier, [&]() { throw parse_error("expected a field name"); });

                base_expr = make_access_expr(base_expr, field.val_string());
            } break;
            case token_type::dbl_plus: {
                base_expr = make_unary_expr(classify_unary_op(_stream.next().type, true), base_expr);
            } break;
            case token_type::dbl_minus: {
                base_expr = make_unary_expr(classify_unary_op(_stream.next().type, true), base_expr);
            } break;
            }
        }

        return base_expr;
    }

    // +expr
    // -expr
    // ++expr
    // --expr
    // *expr
    // &expr
    // ~expr
    // !expr
    std::shared_ptr<expr> parser::parse_expr2()
    {
        if(_stream.next_is_one_of({
            token_type::plus,
            token_type::minus,
            token_type::dbl_plus,
            token_type::dbl_minus,
            token_type::asterix,
            token_type::amp,
            token_type::tilde,
            token_type::bang
        })) {
            auto op = classify_unary_op(_stream.next().type, false);
            return make_unary_expr(
                op,
                parse_expr2()
            );
        }

        return parse_expr1();
    }

    // expr as type
    std::shared_ptr<expr> parser::parse_expr3()
    {
        auto expr = parse_expr2();

        while(_stream.next_is(token_type::as))
        {
            _stream.next();

            expr = make_cast_expr(
                expr,
                parse_typespec()
            );
        }

        return expr;
    }

    // lhs * rhs
    // lhs / rhs
    // lhs % rhs
    std::shared_ptr<expr> parser::parse_expr4()
    {
        auto expr = parse_expr3();
        while(_stream.next_is_one_of({
            token_type::asterix,
            token_type::slash,
            token_type::percent
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr3()
            );
        }
        return expr;
    }

    // lhs + rhs
    // lhs - rhs
    std::shared_ptr<expr> parser::parse_expr5()
    {
        auto expr = parse_expr4();
        while(_stream.next_is_one_of({
            token_type::plus,
            token_type::minus
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr4()
            );
        }
        return expr;
    }

    // lhs << rhs
    // lhs >> rhs
    std::shared_ptr<expr> parser::parse_expr6()
    {
        auto expr = parse_expr5();
        while(_stream.next_is_one_of({
            token_type::dbl_less,
            token_type::dbl_grtr
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr5()
            );
        }
        return expr;
    }

    // lhs < rhs
    // lhs <= rhs
    // lhs > rhs
    // lhs >= rhs
    std::shared_ptr<expr> parser::parse_expr7()
    {
        auto expr = parse_expr6();
        while(_stream.next_is_one_of({
            token_type::less,
            token_type::less_eq,
            token_type::grtr,
            token_type::grtr_eq
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr6()
            );
        }
        return expr;
    }

    // lhs == rhs
    // lhs != rhs
    std::shared_ptr<expr> parser::parse_expr8()
    {
        auto expr = parse_expr7();
        while(_stream.next_is_one_of({
            token_type::dbl_eq,
            token_type::bang_eq
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr7()
            );
        }
        return expr;
    }

    // lhs & rhs
    std::shared_ptr<expr> parser::parse_expr9()
    {
        auto expr = parse_expr8();
        while(_stream.next_is_one_of({
            token_type::amp
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr8()
            );
        }
        return expr;
    }

    // lhs ^ rhs
    std::shared_ptr<expr> parser::parse_expr10()
    {
        auto expr = parse_expr9();
        while(_stream.next_is_one_of({
            token_type::caret
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr9()
            );
        }
        return expr;
    }

    // lhs | rhs
    std::shared_ptr<expr> parser::parse_expr11()
    {
        auto expr = parse_expr10();
        while(_stream.next_is_one_of({
            token_type::pipe
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr10()
            );
        }
        return expr;
    }

    // lhs && rhs
    std::shared_ptr<expr> parser::parse_expr12()
    {
        auto expr = parse_expr11();
        while(_stream.next_is_one_of({
            token_type::dbl_amp
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr11()
            );
        }
        return expr;
    }

    // lhs || rhs
    std::shared_ptr<expr> parser::parse_expr13()
    {
        auto expr = parse_expr12();
        while(_stream.next_is_one_of({
            token_type::dbl_pipe
        })) {
            auto op = classify_binary_op(_stream.next().type);
            expr = make_binary_expr(
                op,
                expr,
                parse_expr12()
            );
        }
        return expr;
    }

    // lhs = rhs
    // lhs += rhs
    // lhs -= rhs
    // lhs *= rhs
    // lhs /= rhs
    // lhs %= rhs
    // lhs <<= rhs
    // lhs >>= rhs
    // lhs &= rhs
    // lhs ^= rhs
    // lhs |= rhs
    std::shared_ptr<expr> parser::parse_expr14()
    {
        auto expr = parse_expr13();
        if(_stream.next_is_one_of({
            token_type::eq,
            token_type::plus_eq,
            token_type::minus_eq,
            token_type::asterix_eq,
            token_type::slash_eq,
            token_type::percent_eq,
            token_type::dbl_less_eq,
            token_type::dbl_grtr_eq,
            token_type::amp_eq,
            token_type::caret_eq,
            token_type::pipe_eq
        })) {
            auto op = classify_binary_op(_stream.next().type);
            return make_binary_expr(
                op,
                expr,
                parse_expr14()
            );
        }
        return expr;
    }

    std::shared_ptr<expr> parser::parse_expr()
    {
        return parse_expr14();
    }

    std::shared_ptr<expr> parser::parse()
    {
        return parse_expr();
    }

    std::shared_ptr<typespec> parser::parse_typespec()
    {
        auto token = _stream.expect_one_of({
            token_type::asterix,
            token_type::identifier,
            token_type::l_paren
        }, [&]() { throw parse_error("expected a type"); });

        switch(token.type)
        {
        case token_type::identifier: {
            return make_name_typespec(token.val_string());
        } break;
        case token_type::l_paren: {
            // arguments
            std::vector<std::shared_ptr<typespec>> args;
            if(!_stream.next_is(token_type::r_paren))
            {
                args.push_back(parse_typespec());
                while(_stream.next_is(token_type::comma))
                {
                    _stream.next();
                    args.push_back(parse_typespec());
                }
            }
            _stream.expect(token_type::r_paren, [&]() { throw parse_error("expected ')'"); });
            _stream.expect(token_type::colon, [&]() { throw parse_error("expected ':'"); });

            auto return_type = parse_typespec();

            return make_func_typespec(args, return_type);  
        } break;
        case token_type::asterix: {
            return make_pointer_typespec(parse_typespec());
        } break;
        }
    }

    std::shared_ptr<stmt> parser::parse_stmt()
    {
        if(_stream.next_is_one_of({
            token_type::let,
            token_type::const_,
            token_type::return_,
            token_type::if_
        })) {
            switch(_stream.next().type)
            {
            case token_type::let: {
                auto name = _stream.expect(token_type::identifier, [&]() { throw parse_error("expected variable name"); });
                
                std::shared_ptr<typespec> type = nullptr;
                if(_stream.next_is(token_type::colon))
                {
                    _stream.next();
                    type = parse_typespec();
                }

                std::shared_ptr<expr> initializer = nullptr;
                if(_stream.next_is(token_type::eq))
                {
                    _stream.next();
                    initializer = parse_expr();
                }

                _stream.expect(token_type::semi_colon, [&]() { throw parse_error("expected ';'"); });
                return make_let_stmt(name.val_string(), type, initializer);
            } break;
            case token_type::const_: {
                auto name = _stream.expect(token_type::identifier, [&]() { throw parse_error("expected variable name"); });
                
                std::shared_ptr<typespec> type = nullptr;
                if(_stream.next_is(token_type::colon))
                {
                    _stream.next();
                    type = parse_typespec();
                }

                std::shared_ptr<expr> initializer = nullptr;
                if(_stream.next_is(token_type::eq))
                {
                    _stream.next();
                    initializer = parse_expr();
                }

                _stream.expect(token_type::semi_colon, [&]() { throw parse_error("expected ';'"); });
                return make_const_stmt(name.val_string(), type, initializer);
            } break;
            case token_type::return_: {
                std::shared_ptr<expr> ret_expr = nullptr;
                if(!_stream.next_is(token_type::semi_colon))
                {
                    ret_expr = parse_expr();
                }
                _stream.expect(token_type::semi_colon, [&]() { throw parse_error("expected ';'"); });
                return make_return_stmt(ret_expr);
            } break;
            case token_type::if_: {
                std::vector<if_branch> if_branches;
                auto expr = parse_expr();
                auto block = parse_stmt_block();
                if_branches.emplace_back(expr, block);
                while(_stream.next_is(token_type::elif))
                {
                    _stream.next();
                    auto expr = parse_expr();
                    auto block = parse_stmt_block();
                    if_branches.emplace_back(expr, block);
                }

                std::vector<std::shared_ptr<stmt>> else_branch;
                if(_stream.next_is(token_type::else_))
                {
                    _stream.next();
                    else_branch = parse_stmt_block();
                }

                return make_if_stmt(if_branches, else_branch);
            } break;
            }
        } else {
            auto expr = parse_expr();
            _stream.expect(token_type::semi_colon, [&]() { throw parse_error("expected ';'"); });
            return make_expr_stmt(expr);
        }
    }
    
    std::vector<std::shared_ptr<stmt>> parser::parse_stmt_block()
    {
        std::vector<std::shared_ptr<stmt>> block;
        _stream.expect(token_type::l_curly, [&]() { throw parse_error("expected '{'"); });
        while(!_stream.next_is(token_type::r_curly))
        {
            block.push_back(parse_stmt());
        }
        _stream.expect(token_type::r_curly, [&]() { throw parse_error("expected '}'"); });
        return block;
    }
}
