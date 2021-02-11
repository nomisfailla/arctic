#pragma once

#include <memory>
#include <utility>
#include <string>

namespace arc
{
	//
	// Forward Declarations
	//

	struct decl_import;
	struct decl_namespace;
	struct decl_func;
	struct decl_struct;

	struct stmt_expr;
	struct stmt_let;
	struct stmt_const;
	struct stmt_return;

	struct expr_integer;
	struct expr_name;
	struct expr_binary;
	struct expr_unary;
	struct expr_call;
	struct expr_index;
	struct expr_access;

	//
	// Visitor Stuff
	//

	struct ast_visitor
	{
		virtual void visit(const decl_import&) {}
		virtual void visit(const decl_namespace&) {}
		virtual void visit(const decl_func&) {}
		virtual void visit(const decl_struct&) {}

		virtual void visit(const stmt_expr&) {}
		virtual void visit(const stmt_let&) {}
		virtual void visit(const stmt_const&) {}
		virtual void visit(const stmt_return&) {}

		virtual void visit(const expr_integer&) {}
		virtual void visit(const expr_name&) {}
		virtual void visit(const expr_binary&) {}
		virtual void visit(const expr_unary&) {}
		virtual void visit(const expr_call&) {}
		virtual void visit(const expr_index&) {}
		virtual void visit(const expr_access&) {}
	};

	struct ast_node
	{
		virtual ~ast_node() = default;
		virtual void accept(ast_visitor&) const = 0;
	};

	//
	// Expressions
	//

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

	struct expr_name : public expr
	{
		const std::string name;

		expr_name(const std::string& name)
			: name(name)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	enum class binary_op
	{
		add,                // +
		sub,                // -
		mul,                // *
		div,                // /
		mod,                // %
		lshift,             // <<
		rshift,             // >>
		less,               // <
		less_eq,            // <=
		greater,            // >
		greater_eq,         // >=
		equality,           // ==
		inequality,         // !=
		bitwise_and,        // &
		bitwise_xor,        // ^
		bitwise_or,         // |
		logical_and,        // &&
		logical_or,         // ||
		assign,             // =
		add_assign,         // +=
		sub_assign,         // -=
		mul_assign,         // *=
		div_assign,         // /=
		mod_assign,         // %=
		lshift_assign,      // <<=
		rshift_assign,      // >>=
		bitwise_and_assign, // &=
		bitwise_xor_assign, // ^=
		bitwise_or_assign,  // |=
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

	enum class unary_op
	{
		positive,    // +
		negative,    // -
		deref,       // *
		address,     // &
		bitwise_not, // ~
		logical_not  // !
	};

	struct expr_unary : public expr
	{
		const unary_op op;
		const std::unique_ptr<expr> rhs;

		expr_unary(unary_op op, std::unique_ptr<expr>& rhs)
			: op(op), rhs(std::move(rhs))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_call : public expr
	{
		const std::unique_ptr<expr> lhs;
		const std::vector<std::unique_ptr<expr>> args;

		expr_call(std::unique_ptr<expr>& lhs, std::vector<std::unique_ptr<expr>> args)
			: lhs(std::move(lhs)), args(std::move(args))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_index : public expr
	{
		const std::unique_ptr<expr> lhs;
		const std::unique_ptr<expr> index;

		expr_index(std::unique_ptr<expr>& lhs, std::unique_ptr<expr>& index)
			: lhs(std::move(lhs)), index(std::move(index))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_access : public expr
	{
		const std::unique_ptr<expr> lhs;
		const std::string field;

		expr_access(std::unique_ptr<expr>& lhs, const std::string& field)
			: lhs(std::move(lhs)), field(field)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};
	
	//
	// Typespecs
	//

	struct typespec
	{

	};

	struct typespec_name : public typespec
	{
		const std::string name;

		typespec_name(const std::string& name)
			: name(name)
		{
		}
	};

	struct typespec_pointer : public typespec
	{
		const std::unique_ptr<typespec> base;

		typespec_pointer(std::unique_ptr<typespec>& base)
			: base(std::move(base))
		{
		}
	};

	//
	// Statements
	//

	struct stmt : public ast_node
	{

	};

	struct stmt_expr : public expr
	{
		const std::unique_ptr<expr> expression;

		stmt_expr(std::unique_ptr<expr>& expression)
			: expression(std::move(expression))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_let : public stmt
	{
		const std::string name;
		const std::unique_ptr<typespec> type;
		const std::unique_ptr<expr> initializer;

		stmt_let(const std::string& name, std::unique_ptr<typespec>& type, std::unique_ptr<expr>& initializer)
			: name(name), type(std::move(type)), initializer(std::move(initializer))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_const : public stmt
	{
		const std::string name;
		const std::unique_ptr<typespec> type;
		const std::unique_ptr<expr> initializer;

		stmt_const(const std::string& name, std::unique_ptr<typespec>& type, std::unique_ptr<expr>& initializer)
			: name(name), type(std::move(type)), initializer(std::move(initializer))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
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
	
	//
	// Declarations
	//

	struct decl : public ast_node
	{

	};

	struct decl_import : public decl
	{
		const std::string path;

		decl_import(const std::string& path)
			: path(path)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct decl_namespace : public decl
	{
		const std::string name;

		decl_namespace(const std::string& name)
			: name(name)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct func_arg
	{
		const std::string name;
		const std::unique_ptr<typespec> type;

		func_arg(const std::string& name, std::unique_ptr<typespec>& type)
			: name(name), type(std::move(type))
		{
		}
	};

	struct decl_func : public decl
	{
		const std::string name;
		const std::vector<func_arg> arguments;
		const std::unique_ptr<typespec> ret_type;
		const std::vector<std::unique_ptr<stmt>> body;

		decl_func(const std::string& name, const std::vector<func_arg>& arguments, std::unique_ptr<typespec>& ret_type, std::vector<std::unique_ptr<stmt>>& body)
			: name(name), arguments(arguments), ret_type(std::move(ret_type)), body(std::move(body))
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct struct_field
	{
		const std::string name;
		const std::unique_ptr<typespec> type;
		const bool is_const;

		struct_field(const std::string& name, std::unique_ptr<typespec>& type, bool is_const)
			: name(name), type(std::move(type)), is_const(is_const)
		{
		}
	};

	struct decl_struct : public decl
	{
		const std::string name;
		const std::vector<struct_field> fields;

		decl_struct(const std::string& name, const std::vector<struct_field>& fields)
			: name(name), fields(fields)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	//
	// Utilities
	//

	static auto inline make_integer_expr(uint64_t value)
	{
		return std::make_unique<expr_integer>(value);
	}

	static auto inline make_name_expr(const std::string& name)
	{
		return std::make_unique<expr_name>(name);
	}

	static auto inline make_binary_expr(binary_op op, std::unique_ptr<expr> lhs, std::unique_ptr<expr> rhs)
	{
		return std::make_unique<expr_binary>(op, lhs, rhs);
	}

	static auto inline make_unary_expr(unary_op op, std::unique_ptr<expr> rhs)
	{
		return std::make_unique<expr_unary>(op, rhs);
	}

	static auto inline make_call_expr(std::unique_ptr<expr> lhs, std::vector<std::unique_ptr<expr>> args)
	{
		return std::make_unique<expr_call>(lhs, args);
	}

	static auto inline make_index_expr(std::unique_ptr<expr> lhs, std::unique_ptr<expr> index)
	{
		return std::make_unique<expr_index>(lhs, index);
	}

	static auto inline make_access_expr(std::unique_ptr<expr> lhs, const std::string& field)
	{
		return std::make_unique<expr_access>(lhs, field);
	}

	static auto inline make_name_typespec(const std::string& name)
	{
		return std::make_unique<typespec_name>(name);
	}

	static auto inline make_pointer_typespec(std::unique_ptr<typespec> base)
	{
		return std::make_unique<typespec_pointer>(base);
	}

	static auto inline make_expr_stmt(std::unique_ptr<expr> expression)
	{
		return std::make_unique<stmt_expr>(expression);
	}

	static auto inline make_let_stmt(const std::string& name, std::unique_ptr<typespec> type, std::unique_ptr<expr> initializer)
	{
		return std::make_unique<stmt_let>(name, type, initializer);
	}

	static auto inline make_const_stmt(const std::string& name, std::unique_ptr<typespec> type, std::unique_ptr<expr> initializer)
	{
		return std::make_unique<stmt_const>(name, type, initializer);
	}

	static auto inline make_return_stmt(std::unique_ptr<expr> ret_expr)
	{
		return std::make_unique<expr>(ret_expr);
	}

	static auto inline make_import_decl(const std::string& path)
	{
		return std::make_unique<decl_import>(path);
	}

	static auto inline make_namespace_decl(const std::string& name)
	{
		return std::make_unique<decl_namespace>(name);
	}

	static auto inline make_func_decl(const std::string& name, const std::vector<func_arg>& arguments, std::unique_ptr<typespec> ret_type, std::vector<std::unique_ptr<stmt>> body)
	{
		return std::make_unique<decl_func>(name, arguments, ret_type, body);
	}
	
	static auto inline make_struct_decl(const std::string name, const std::vector<struct_field>& fields)
	{
		return std::make_unique<decl_struct>(name, fields);
	}
}
