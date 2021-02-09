#pragma once

#include <memory>
#include <utility>

namespace arc
{
	struct decl_func;
	struct decl_struct;

	struct stmt_return;

	struct expr_integer;
	struct expr_binary;

	struct ast_visitor
	{
		virtual void visit(const decl_func&) {}
		virtual void visit(const decl_struct&) {}

		virtual void visit(const stmt_return&) {}

		virtual void visit(const expr_integer&) {}
		virtual void visit(const expr_binary&) {}
	};

	struct ast_node
	{
		virtual ~ast_node() = default;
		virtual void accept(ast_visitor&) const = 0;
	};

	struct expr : public ast_node
	{

	};

	struct expr_integer : public expr
	{
		const uint64_t value;

		expr_integer(uint64_t value)
			: value(value)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	enum class binary_op
	{
		add,
		sub,
		mul,
		div
	};

	struct expr_binary : public expr
	{
		const binary_op op;
		const std::unique_ptr<expr> lhs;
		const std::unique_ptr<expr> rhs;

		expr_binary(binary_op op, std::unique_ptr<expr>& lhs, std::unique_ptr<expr>& rhs)
			: op(op), lhs(std::move(lhs)), rhs(std::move(rhs))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt : public ast_node
	{

	};

	struct stmt_return : public stmt
	{
		const std::unique_ptr<expr> ret_expr;

		stmt_return(std::unique_ptr<expr>& expr)
			: ret_expr(std::move(expr))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct decl : public ast_node
	{

	};

	struct decl_func : public decl
	{
		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct decl_struct : public decl
	{
		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	static auto inline make_integer_expr(uint64_t value)
	{
		return std::make_unique<expr_integer>(value);
	}

	static auto inline make_binary_expr(binary_op op, std::unique_ptr<expr> lhs, std::unique_ptr<expr> rhs)
	{
		return std::make_unique<expr_binary>(op, lhs, rhs);
	}
}
