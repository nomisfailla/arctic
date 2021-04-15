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
		const std::shared_ptr<expr> lhs;
		const std::shared_ptr<expr> rhs;

		expr_binary(binary_op op, const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& rhs)
			: op(op), lhs(lhs), rhs(rhs)
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
		logical_not, // !
		postfix_add, // ++
		postfix_sub, // --
		prefix_add,  // ++
		prefix_sub   // --
	};

	struct expr_unary : public expr
	{
		const unary_op op;
		const std::shared_ptr<expr> rhs;

		expr_unary(unary_op op, const std::shared_ptr<expr>& rhs)
			: op(op), rhs(rhs)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_call : public expr
	{
		const std::shared_ptr<expr> lhs;
		const std::vector<std::shared_ptr<expr>> args;

		expr_call(const std::shared_ptr<expr>& lhs, const std::vector<std::shared_ptr<expr>>& args)
			: lhs(lhs), args(args)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_index : public expr
	{
		const std::shared_ptr<expr> lhs;
		const std::shared_ptr<expr> index;

		expr_index(const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& index)
			: lhs(lhs), index(index)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_access : public expr
	{
		const std::shared_ptr<expr> lhs;
		const std::string field;

		expr_access(const std::shared_ptr<expr>& lhs, const std::string& field)
			: lhs(lhs), field(field)
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
		const std::shared_ptr<typespec> base;

		typespec_pointer(const std::shared_ptr<typespec>& base)
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

	struct stmt_expr : public stmt
	{
		const std::shared_ptr<expr> expression;

		stmt_expr(const std::shared_ptr<expr>& expression)
			: expression(expression)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_let : public stmt
	{
		const std::string name;
		const std::shared_ptr<typespec> type;
		const std::shared_ptr<expr> initializer;

		stmt_let(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer)
			: name(name), type(type), initializer(initializer)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_const : public stmt
	{
		const std::string name;
		const std::shared_ptr<typespec> type;
		const std::shared_ptr<expr> initializer;

		stmt_const(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer)
			: name(name), type(type), initializer(initializer)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_return : public stmt
	{
		const std::shared_ptr<expr> expression;

		stmt_return(const std::shared_ptr<expr>& expression)
			: expression(expression)
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
		const std::shared_ptr<typespec> type;

		func_arg(const std::string& name, const std::shared_ptr<typespec>& type)
			: name(name), type(type)
		{
		}
	};

	struct decl_func : public decl
	{
		const std::string name;
		const std::vector<func_arg> arguments;
		const std::shared_ptr<typespec> ret_type;
		const std::vector<std::shared_ptr<stmt>> body;

		decl_func(const std::string& name, const std::vector<func_arg>& arguments, const std::shared_ptr<typespec>& ret_type, const std::vector<std::shared_ptr<stmt>>& body)
			: name(name), arguments(arguments), ret_type(ret_type), body(body)
		{
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct struct_field
	{
		const std::string name;
		const bool is_const;
		const std::shared_ptr<typespec> type;

		struct_field(const std::string& name, const std::shared_ptr<typespec>& type, bool is_const)
			: name(name), type(type), is_const(is_const)
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
		return std::shared_ptr<expr_integer>(new expr_integer(value));
	}

	static auto inline make_name_expr(const std::string& name)
	{
		return std::shared_ptr<expr_name>(new expr_name(name));
	}

	static auto inline make_binary_expr(binary_op op, const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& rhs)
	{
		return std::shared_ptr<expr_binary>(new expr_binary(op, lhs, rhs));
	}

	static auto inline make_unary_expr(unary_op op, const std::shared_ptr<expr>& rhs)
	{
		return std::shared_ptr<expr_unary>(new expr_unary(op, rhs));
	}

	static auto inline make_call_expr(const std::shared_ptr<expr>& lhs, const std::vector<std::shared_ptr<expr>>& args)
	{
		return std::shared_ptr<expr_call>(new expr_call(lhs, args));
	}

	static auto inline make_index_expr(const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& index)
	{
		return std::shared_ptr<expr_index>(new expr_index(lhs, index));
	}

	static auto inline make_access_expr(const std::shared_ptr<expr>& lhs, const std::string& field)
	{
		return std::shared_ptr<expr_access>(new expr_access(lhs, field));
	}

	static auto inline make_name_typespec(const std::string& name)
	{
		return std::shared_ptr<typespec_name>(new typespec_name(name));
	}

	static auto inline make_pointer_typespec(const std::shared_ptr<typespec>& base)
	{
		return std::shared_ptr<typespec_pointer>(new typespec_pointer(base));
	}

	static auto inline make_expr_stmt(const std::shared_ptr<expr>& expression)
	{
		return std::shared_ptr<stmt_expr>(new stmt_expr(expression));
	}

	static auto inline make_let_stmt(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer)
	{
		return std::shared_ptr<stmt_let>(new stmt_let(name, type, initializer));
	}

	static auto inline make_const_stmt(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer)
	{
		return std::shared_ptr<stmt_const>(new stmt_const(name, type, initializer));
	}

	static auto inline make_return_stmt(const std::shared_ptr<expr>& ret_expr)
	{
		return std::shared_ptr<stmt_return>(new stmt_return(ret_expr));
	}

	static auto inline make_import_decl(const std::string& path)
	{
		return std::shared_ptr<decl_import>(new decl_import(path));
	}

	static auto inline make_namespace_decl(const std::string& name)
	{
		return std::shared_ptr<decl_namespace>(new decl_namespace(name));
	}

	static auto inline make_func_decl(const std::string& name, const std::vector<func_arg>& arguments, const std::shared_ptr<typespec>& ret_type, const std::vector<std::shared_ptr<stmt>>& body)
	{
		return std::shared_ptr<decl_func>(new decl_func(name, arguments, ret_type, body));
	}
	
	static auto inline make_struct_decl(const std::string name, const std::vector<struct_field>& fields)
	{
		return std::shared_ptr<decl_struct>(new decl_struct(name, fields));
	}
}
