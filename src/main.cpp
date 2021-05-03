#include <iostream>
#include <stack>
#include <cstring>

#include "lex/lexer.h"
#include "parse/parser.h"
#include "util/source_file.h"

#include "test/test_main.h"

static std::string format_error(const arc::line_exception& ex)
{
	std::string error = "";

	error += "error: " + ex.error + " at " + std::to_string(ex.position.line) + ":" + std::to_string(ex.position.column) + "\n";
	error += std::to_string(ex.position.line) + " | " + ex.file.get_line(ex.position.line) + "\n";

	return error;
}

static std::string binary_op_to_string(arc::binary_op op)
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
}

static std::string unary_op_to_string(arc::unary_op op)
{
	switch(op)
	{
		case arc::unary_op::positive:    return "+";
		case arc::unary_op::negative:    return "-";
		case arc::unary_op::deref:       return "*";
		case arc::unary_op::address:     return "&";
		case arc::unary_op::bitwise_not: return "~";
		case arc::unary_op::logical_not: return "!";
		case arc::unary_op::postfix_add: return "++";
		case arc::unary_op::postfix_sub: return "--";
		case arc::unary_op::prefix_add:  return "++";
		case arc::unary_op::prefix_sub:  return "--";
	}
}

class expression_printer : public arc::ast_visitor
{
public:
	void visit(const arc::expr_integer& expr)
	{
		std::cout << "integer(" << expr.value << ")";
	}

	void visit(const arc::expr_name& expr)
	{
		std::cout << "name(" << expr.name << ")";
	}

	void visit(const arc::expr_binary& expr)
	{
		std::cout << "binary(" << std::endl;
		std::cout << binary_op_to_string(expr.op) << std::endl;
		expr.lhs->accept(*this); std::cout << std::endl;
		expr.rhs->accept(*this); std::cout << std::endl;
		std::cout << ")";
	}

	void visit(const arc::expr_unary& expr)
	{
		std::cout << "unary(" << std::endl;
		std::cout << unary_op_to_string(expr.op) << std::endl;
		expr.rhs->accept(*this); std::cout << std::endl;
		std::cout << ")";
	}

	void visit(const arc::expr_call& expr)
	{
		std::cout << "call(" << std::endl;
		expr.lhs->accept(*this); std::cout << std::endl;
		std::cout << "{" << std::endl;;
		for(const auto& a : expr.args)
		{
			a->accept(*this);
			std::cout << std::endl;
		}
		std::cout << "}";
	}

	void visit(const arc::expr_index& expr)
	{
		std::cout << "index(" << std::endl;
		expr.lhs->accept(*this); std::cout << std::endl;
		expr.index->accept(*this); std::cout << std::endl;
		std::cout << ")";
	}

	void visit(const arc::expr_access& expr)
	{
		std::cout << "access(" << std::endl;
		expr.lhs->accept(*this); std::cout << std::endl;
		std::cout << expr.field << std::endl;
		std::cout << ")";
	}
};

static void process(const arc::source_file& input)
{
	if(input.exists())
	{
		try
		{
			arc::lexer lexer(input);
			auto lexer_result = lexer.lex();
			if(lexer_result.succeeded())
			{
				arc::parser parser(lexer_result.tokens, input);
				auto decls = parser.parse_module();
			}
			else
			{
				for(const auto& error : lexer_result.errors)
				{
					std::cout << format_error(error);
				}
			}
		}
		catch(const arc::line_exception& ex)
		{
			std::cout << format_error(ex);
		}
	}
	else
	{
		std::cout << "'" << input.path() << "' not found" << std::endl;
	}
}

int main(int argc, const char** argv)
{
	if(argc > 1 && std::strcmp(argv[1], "test") == 0)
	{
		return arc_test_main(argc, argv);
	}

	if(argc == 2)
	{
		arc::source_file input(argv[1]);
		process(input);
	}
	else
	{
		while(true)
		{
			std::cout << "> ";
			std::string src;
			std::getline(std::cin, src);
			arc::source_file input(src, true);
			process(input);
		}
	}
}
