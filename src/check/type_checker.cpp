#include "type_checker.h"

#include "../util/casting.h"

#include <iostream>

namespace
{
	std::string operator_to_string(arc::binary_op op)
	{
		switch(op)
		{
		case arc::binary_op::add:                return "+";
		case arc::binary_op::sub:                return "-";
		case arc::binary_op::mul:                return "*";
		case arc::binary_op::div:                return "/";
		case arc::binary_op::mod:                return "%";
		case arc::binary_op::lshift:             return "<<";
		case arc::binary_op::rshift:             return ">>";
		case arc::binary_op::less:               return "<";
		case arc::binary_op::less_eq:            return "<=";
		case arc::binary_op::greater:            return ">";
		case arc::binary_op::greater_eq:         return ">=";
		case arc::binary_op::equality:           return "==";
		case arc::binary_op::inequality:         return "!=";
		case arc::binary_op::bitwise_and:        return "&";
		case arc::binary_op::bitwise_xor:        return "^";
		case arc::binary_op::bitwise_or:         return "|";
		case arc::binary_op::logical_and:        return "&&";
		case arc::binary_op::logical_or:         return "||";
		case arc::binary_op::assign:             return "=";
		case arc::binary_op::add_assign:         return "+=";
		case arc::binary_op::sub_assign:         return "-=";
		case arc::binary_op::mul_assign:         return "*=";
		case arc::binary_op::div_assign:         return "/=";
		case arc::binary_op::mod_assign:         return "%=";
		case arc::binary_op::lshift_assign:      return "<<=";
		case arc::binary_op::rshift_assign:      return ">>=";
		case arc::binary_op::bitwise_and_assign: return "&=";
		case arc::binary_op::bitwise_xor_assign: return "^=";
		case arc::binary_op::bitwise_or_assign:  return "|=";
		}
		return "?";
	}
}

namespace arc
{
	class expr_checker
	{
	private:
		lexical_scope* _scope;
		type_map& _type_map;
		type_checker& _checker;
	public:
		expr_checker(lexical_scope* scope, type_map& type_map, type_checker& checker)
			: _scope(scope), _type_map(type_map), _checker(checker)
		{
		}

		std::shared_ptr<type> check(const std::shared_ptr<expr>& e)
		{
			if(auto expr = arc::is<expr_integer>(e))
			{
				return _type_map.get(make_name_typespec("u64"));
			}

			if(auto expr = arc::is<expr_boolean>(e))
			{
				return _type_map.get(make_name_typespec("bool"));
			}

			if(auto expr = arc::is<expr_name>(e))
			{
				if(auto type = _scope->get(expr->name))
				{
					return type;
				}
				else
				{
					_checker.add_error("could not find variable with name " + expr->name, expr->position);
					return _type_map.get(make_name_typespec("none"));
				}
			}

			if(auto expr = arc::is<expr_binary>(e))
			{
				auto lhs = this->check(expr->lhs);
				auto rhs = this->check(expr->rhs);
				// For now, asssume binary operaters are only implemented for equal types.
				if(lhs == rhs)
				{
					// TODO: Return the actual return type of the boolean operator.
					return lhs;
				}
				else
				{
					_checker.add_error("operator " + operator_to_string(expr->op) + " not implemented for types", expr->position);
					return _type_map.get(make_name_typespec("none"));
				}
			}

			if(auto expr = arc::is<expr_unary>(e))
			{
				auto rhs = this->check(expr->rhs);
				// TODO: Return the actual return type of the unary operator.
				return rhs;
			}

			if(auto expr = arc::is<expr_call>(e))
			{
				auto lhs = this->check(expr->lhs);
				if(auto func_type = arc::is<type_func>(lhs))
				{
					if(func_type->argument_types.size() == expr->args.size())
					{
						for(int i = 0; i < expr->args.size(); i++)
						{
							auto expected_type = func_type->argument_types[i];
							auto got_type = this->check(expr->args[i]);
							if(got_type != expected_type)
							{
								_checker.add_error("parameter type mismatch at index " + std::to_string(i), expr->position);
							}
						}
						return func_type->return_type;
					}
					else
					{
						_checker.add_error(
							"incorrect number of parameters passed to function, expected " +
							std::to_string(func_type->argument_types.size()) +
							", got " +
							std::to_string(expr->args.size()),
						expr->position);
						return _type_map.get(make_name_typespec("none"));
					}
				}
				else
				{
					_checker.add_error("object is not callable", expr->position);
					return _type_map.get(make_name_typespec("none"));
				}
			}

			if(auto expr = arc::is<expr_index>(e))
			{
				std::cout << "not implemented" << std::endl;
				std::exit(1);
			}

			if(auto expr = arc::is<expr_access>(e))
			{
				std::cout << "not implemented" << std::endl;
				std::exit(1);
			}

			if(auto expr = arc::is<expr_cast>(e))
			{
				std::cout << "not implemented" << std::endl;
				std::exit(1);
			}

			throw internal_exception("unreachable");
		}
	};

	class func_checker
	{
	private:
		std::shared_ptr<decl_func> _decl;
		lexical_scope _scope;
		type_map& _type_map;
		type_checker& _checker;
	public:
		func_checker(const std::shared_ptr<decl_func>& decl, lexical_scope* parent_scope, type_map& type_map, type_checker& checker)
			: _decl(decl), _scope(parent_scope), _type_map(type_map), _checker(checker)
		{
		}

		void check()
		{
			for(const auto& arg : _decl->arguments)
			{
				_scope.add(arg.name, _type_map.get(arg.type));
			}

			for(const auto& s : _decl->body)
			{
				if(auto stmt = arc::is<stmt_let>(s))
				{
					if(stmt->initializer == nullptr && stmt->type == nullptr)
					{
						_checker.add_error("cannot deduce variable type", stmt->position);
					}
					
					// _: A = B -> A
					if(stmt->type != nullptr && stmt->initializer != nullptr)
					{
						auto var_type = _type_map.get(stmt->type);
						auto init_type = expr_checker(&_scope, _type_map, _checker).check(stmt->initializer);
						if(init_type != var_type)
						{
							_checker.add_error("types cannot be assigned", stmt->position);
						}

						if(!_scope.add(stmt->name, var_type))
						{
							_checker.add_error("variable name '" + stmt->name + "' already taken", stmt->position);
						}
					}
					
					// _    = B -> B
					if(stmt->type == nullptr && stmt->initializer != nullptr)
					{
						auto init_type = expr_checker(&_scope, _type_map, _checker).check(stmt->initializer);
						if(!_scope.add(stmt->name, init_type))
						{
							_checker.add_error("variable name '" + stmt->name + "' already taken", stmt->position);
						}
					}

					// _: A;    -> A
					if(stmt->type != nullptr && stmt->initializer == nullptr)
					{
						auto var_type = _type_map.get(stmt->type);
						if(!_scope.add(stmt->name, var_type))
						{
							_checker.add_error("variable name '" + stmt->name + "' already taken", stmt->position);
						}
					}
				}
				
				if(auto stmt = arc::is<stmt_const>(s))
				{
					std::cout << "unimplemented" << std::endl;
					std::exit(1);
				}
				
				if(auto stmt = arc::is<stmt_if>(s))
				{
					std::cout << "unimplemented" << std::endl;
					std::exit(1);
				}
				
				if(auto stmt = arc::is<stmt_return>(s))
				{
					std::cout << "unimplemented" << std::endl;
					std::exit(1);
				}
				
				if(auto stmt = arc::is<stmt_expr>(s))
				{
					std::cout << "unimplemented" << std::endl;
					std::exit(1);
				}
			}
		}
	};

    type_checker::type_checker(const std::vector<std::shared_ptr<decl>>& ast, const source_file& source)
		: _ast(ast), _source(source)
	{
		_type_map.add(arc::make_name_typespec("none"), new arc::type_none());
		_type_map.add(arc::make_name_typespec("bool"), new arc::type_bool());
		_type_map.add(arc::make_name_typespec("f32"), new arc::type_float(32));
		_type_map.add(arc::make_name_typespec("f64"), new arc::type_float(64));
		_type_map.add(arc::make_name_typespec("u8"), new arc::type_integer(false, 8));
		_type_map.add(arc::make_name_typespec("u16"), new arc::type_integer(false, 16));
		_type_map.add(arc::make_name_typespec("u32"), new arc::type_integer(false, 32));
		_type_map.add(arc::make_name_typespec("u64"), new arc::type_integer(false, 64));
		_type_map.add(arc::make_name_typespec("i8"), new arc::type_integer(true, 8));
		_type_map.add(arc::make_name_typespec("i16"), new arc::type_integer(true, 16));
		_type_map.add(arc::make_name_typespec("i32"), new arc::type_integer(true, 32));
		_type_map.add(arc::make_name_typespec("i64"), new arc::type_integer(true, 64));
	}
    
	void type_checker::add_error(const std::string& error, source_pos position)
	{
		_errors.push_back(line_exception(error, _source, position));
	}

	std::vector<line_exception> type_checker::check()
	{
		for(const auto& d : _ast)
		{
			if(auto decl = arc::is<decl_func>(d))
			{
				func_checker(decl, &_global_scope, _type_map, *this).check();
			}
		}

		return _errors;
	}
}
