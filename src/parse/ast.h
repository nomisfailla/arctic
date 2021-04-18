#pragma once

#include <memory>
#include <utility>
#include <string>
#include <vector>

namespace arc
{
	//
	// Forward Declarations
	//

	struct decl_import;
	struct decl_namespace;
	struct decl_func;
	struct decl_struct;
	struct decl_alias;

	struct stmt_expr;
	struct stmt_let;
	struct stmt_const;
	struct stmt_return;
	struct stmt_if;

	struct expr_integer;
	struct expr_name;
	struct expr_binary;
	struct expr_unary;
	struct expr_call;
	struct expr_index;
	struct expr_access;
	struct expr_cast;

	//
	// Visitor Stuff
	//

	struct ast_visitor
	{
		virtual void visit(const decl_import&) {}
		virtual void visit(const decl_namespace&) {}
		virtual void visit(const decl_func&) {}
		virtual void visit(const decl_struct&) {}
		virtual void visit(const decl_alias&) {}

		virtual void visit(const stmt_expr&) {}
		virtual void visit(const stmt_let&) {}
		virtual void visit(const stmt_const&) {}
		virtual void visit(const stmt_return&) {}
		virtual void visit(const stmt_if&) {}

		virtual void visit(const expr_integer&) {}
		virtual void visit(const expr_name&) {}
		virtual void visit(const expr_binary&) {}
		virtual void visit(const expr_unary&) {}
		virtual void visit(const expr_call&) {}
		virtual void visit(const expr_index&) {}
		virtual void visit(const expr_access&) {}
		virtual void visit(const expr_cast&) {}
	};

	struct ast_node
	{
		virtual ~ast_node() = default;
		virtual void accept(ast_visitor&) const = 0;
	};

	//
	// Typespecs
	//

	struct typespec
	{
		bool operator==(const typespec& rhs)
		{
			if(typeid(*this) != typeid(rhs)) { return false; }
			return equals(rhs);
		}

		bool operator!=(const typespec& rhs)
		{
			return !(*this == rhs);
		}

		virtual bool equals(const typespec& rhs) const = 0;
	};

	struct typespec_name : public typespec
	{
		const std::string name;

		typespec_name(const std::string& name)
			: name(name)
		{
		}

		bool equals(const typespec& rhs) const
		{
			const typespec_name& r = dynamic_cast<const typespec_name&>(rhs);
			return this->name == r.name;
		}
	};

	struct typespec_pointer : public typespec
	{
		const std::shared_ptr<typespec> base;

		typespec_pointer(const std::shared_ptr<typespec>& base)
			: base(base)
		{
		}

		bool equals(const typespec& rhs) const
		{
			const typespec_pointer& r = dynamic_cast<const typespec_pointer&>(rhs);
			return *this->base == *r.base;
		}
	};

	struct typespec_func : public typespec
	{
		const std::vector<std::shared_ptr<typespec>> argument_types;
		const std::shared_ptr<typespec> return_type;

		typespec_func(const std::vector<std::shared_ptr<typespec>>& argument_types, const std::shared_ptr<typespec>& return_type)
			: argument_types(argument_types), return_type(return_type)
		{
		}

		bool equals(const typespec& rhs) const
		{
			const typespec_func& r = dynamic_cast<const typespec_func&>(rhs);

			if(*this->return_type != *r.return_type) { return false; }
			if(this->argument_types.size() != r.argument_types.size()) { return false; }

			for(int i = 0; i < this->argument_types.size(); i++)
			{
				if(*this->argument_types[i] != *r.argument_types[i]) { return false; }
			}

			return true;
		}
	};

	//
	// Expressions
	//

	struct expr : public ast_node
	{
		bool operator==(const expr& rhs)
		{
			if(typeid(*this) != typeid(rhs)) { return false; }
			return equals(rhs);
		}

		bool operator!=(const expr& rhs)
		{
			return !(*this == rhs);
		}

		virtual bool equals(const expr& rhs) const = 0;
	};

	struct expr_integer : public expr
	{
		const uint64_t value;

		expr_integer(uint64_t value)
			: value(value)
		{
		}

		bool equals(const expr& rhs) const
		{
			const expr_integer& r = dynamic_cast<const expr_integer&>(rhs);
			return this->value == r.value;
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

		bool equals(const expr& rhs) const
		{
			const expr_name& r = dynamic_cast<const expr_name&>(rhs);
			return this->name == r.name;
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

		bool equals(const expr& rhs) const
		{
			const expr_binary& r = dynamic_cast<const expr_binary&>(rhs);
			return this->op == r.op && *this->lhs == *r.lhs && *this->rhs == *r.rhs;
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

		bool equals(const expr& rhs) const
		{
			const expr_unary& r = dynamic_cast<const expr_unary&>(rhs);
			return this->op == r.op && *this->rhs == *r.rhs;
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

		bool equals(const expr& rhs) const
		{
			const expr_call& r = dynamic_cast<const expr_call&>(rhs);
			
			if(*this->lhs != *r.lhs) { return false; }
			if(this->args.size() != r.args.size()) { return false; }

			for(int i = 0; i < this->args.size(); i++)
			{
				if(*this->args[i] != *r.args[i]) { return false; }
			}

			return true;
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

		bool equals(const expr& rhs) const
		{
			const expr_index& r = dynamic_cast<const expr_index&>(rhs);
			return *this->lhs == *r.lhs && *this->index == *r.index;
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

		bool equals(const expr& rhs) const
		{
			const expr_access& r = dynamic_cast<const expr_access&>(rhs);
			return *this->lhs == *r.lhs && this->field == r.field;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_cast : public expr
	{
		const std::shared_ptr<expr> lhs;
		const std::shared_ptr<typespec> to_type;

		expr_cast(const std::shared_ptr<expr>& lhs, const std::shared_ptr<typespec>& to_type)
			: lhs(lhs), to_type(to_type)
		{
		}

		bool equals(const expr& rhs) const
		{
			const expr_cast& r = dynamic_cast<const expr_cast&>(rhs);
			return *this->lhs == *r.lhs && *this->to_type == *r.to_type;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
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

	struct stmt_if : public stmt
	{
		const std::shared_ptr<expr> if_condition;
		const std::vector<std::shared_ptr<expr>> else_if_conditions;
		const std::shared_ptr<expr> else_condition;

		stmt_if(
			const std::shared_ptr<expr>& if_condition,
			const std::vector<std::shared_ptr<expr>>& else_if_conditions,
			const std::shared_ptr<expr>& else_condition
		) : if_condition(if_condition), else_if_conditions(else_if_conditions), else_condition(else_condition)
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

	struct decl_alias : public decl
	{
		const std::string name;
		const std::shared_ptr<typespec> type;

		decl_alias(const std::string& name, const std::shared_ptr<typespec>& type)
			: name(name), type(type)
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

	static auto make_cast_expr(const std::shared_ptr<expr>& lhs, const std::shared_ptr<typespec>& to_type)
	{
		return std::shared_ptr<expr_cast>(new expr_cast(lhs, to_type));
	}

	static auto inline make_name_typespec(const std::string& name)
	{
		return std::shared_ptr<typespec_name>(new typespec_name(name));
	}

	static auto inline make_pointer_typespec(const std::shared_ptr<typespec>& base)
	{
		return std::shared_ptr<typespec_pointer>(new typespec_pointer(base));
	}

	static auto inline make_func_typespec(const std::vector<std::shared_ptr<typespec>>& argument_types, const std::shared_ptr<typespec>& return_type)
	{
		return std::shared_ptr<typespec_func>(new typespec_func(argument_types, return_type));
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

	static auto inline make_if_stmt(
		const std::shared_ptr<expr>& if_condition,
		const std::vector<std::shared_ptr<expr>>& else_if_conditions,
		const std::shared_ptr<expr>& else_condition
	) {
		return std::shared_ptr<stmt_if>(new stmt_if(if_condition, else_if_conditions, else_condition));
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

	static auto inline make_alias_decl(const std::string& name, const std::shared_ptr<typespec>& type)
	{
		return std::shared_ptr<decl_alias>(new decl_alias(name, type));
	}
}
