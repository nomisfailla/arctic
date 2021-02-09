#include <iostream>
#include <stack>

#include "lex/lexer.h"
#include "parse/ast.h"
#include "util/source_file.h"

static void usage(int argc, const char** argv)
{
	std::cout << "usage: " << argv[0] << " <file>" << std::endl;
}

class ast_evaluator : public arc::ast_visitor
{
private:
	std::stack<uint64_t> _stack;
public:
	void visit(const arc::expr_integer& expr)
	{
		_stack.push(expr.value);
	}

	void visit(const arc::expr_binary& expr)
	{
		expr.lhs->accept(*this);
		auto lhs = _stack.top(); _stack.pop();

		expr.rhs->accept(*this);
		auto rhs = _stack.top(); _stack.pop();

		switch (expr.op)
		{
		case arc::binary_op::add: { _stack.push(lhs + rhs); } break;
		case arc::binary_op::sub: { _stack.push(lhs - rhs); } break;
		case arc::binary_op::mul: { _stack.push(lhs * rhs); } break;
		case arc::binary_op::div: { _stack.push(lhs / rhs); } break;
		}
	}

	uint64_t result() const
	{
		return _stack.top();
	}
};

int main(int argc, const char** argv)
{
	auto expr = arc::make_binary_expr(
		arc::binary_op::sub,
		arc::make_binary_expr(
			arc::binary_op::mul,
			arc::make_integer_expr(2),
			arc::make_integer_expr(7)
		),
		arc::make_integer_expr(5)
	);

	ast_evaluator eval;
	expr->accept(eval);
	std::cout << eval.result() << std::endl;

	if(argc == 2)
	{
		arc::source_file input(argv[1]);
		if(input.exists())
		{
			arc::lexer lexer(input);
			lexer.lex();
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
