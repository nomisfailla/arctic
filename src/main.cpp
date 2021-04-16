#include <iostream>
#include <stack>
#include <cstring>

#include "lex/lexer.h"
#include "parse/parser.h"
#include "util/source_file.h"

#include "test/test_main.h"

static void usage(int argc, const char** argv)
{
	std::cout << "usage: " << argv[0] << " <file>" << std::endl;
}

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
		std::cout << expr.value;
	}

	void visit(const arc::expr_name& expr)
	{
		std::cout << expr.name;
	}

	void visit(const arc::expr_binary& expr)
	{
		std::cout << "(";
		expr.lhs->accept(*this);
		std::cout << " " << binary_op_to_string(expr.op) << " ";
		expr.rhs->accept(*this);
		std::cout << ")";
	}

	void visit(const arc::expr_unary& expr)
	{
		std::cout << "(";
		std::cout << unary_op_to_string(expr.op);
		expr.rhs->accept(*this);
		std::cout << ")";
	}

	void visit(const arc::expr_call& expr)
	{
		expr.lhs->accept(*this);
		std::cout << "(";
		for(const auto& a : expr.args)
		{
			a->accept(*this);
			std::cout << " ";
		}
		std::cout << ")";
	}

	void visit(const arc::expr_index& expr)
	{
		expr.lhs->accept(*this);
		std::cout << "[";
		expr.index->accept(*this);
		std::cout << "]";
	}

	void visit(const arc::expr_access& expr)
	{
		expr.lhs->accept(*this);
		std::cout << "." << expr.field;
	}
};

int main(int argc, const char** argv)
{
	if(argc > 1 && std::strcmp(argv[1], "test") == 0)
	{
		return arc_test_main(argc, argv);
	}

	if(argc == 2)
	{

		arc::source_file input(argv[1]);
		if(input.exists())
		{
			try
			{
				arc::lexer lexer(input);
				auto tokens = lexer.lex();
				
				arc::parser parser(tokens, input);
				auto expr = parser.parse();

				expression_printer printer;
				expr->accept(printer);
			}
			catch(const arc::line_exception& ex)
			{
				std::cout << format_error(ex);
			}
		}
		else
		{
			std::cout << "'" << argv[1] << "' not found" << std::endl;
		}
	}
	else
	{
		usage(argc, argv);
	}
}
