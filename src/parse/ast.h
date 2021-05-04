#pragma once

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <unordered_map>

#include "../util/source_file.h"

namespace arc
{
	//
	// Forward Declarations
	//

	struct typespec_func;
	struct typespec_name;
	struct typespec_pointer;

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
	struct expr_boolean;
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
		virtual void visit(const typespec_func&) {}
		virtual void visit(const typespec_name&) {}
		virtual void visit(const typespec_pointer&) {}

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
		virtual void visit(const expr_boolean&) {}
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
        const source_pos position;

		ast_node(source_pos position)
			: position(position)
		{
		}

		virtual ~ast_node() = default;
		virtual void accept(ast_visitor&) const = 0;
	};

	//
	// Typespecs
	//

	struct typespec : public ast_node
	{
		typespec(source_pos position)
			: ast_node(position)
		{
		}

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
		virtual size_t hash() const = 0;
	};

	struct typespec_name : public typespec
	{
		const std::string name;

		typespec_name(const std::string& name, source_pos position)
			: name(name), typespec(position)
		{
		}

		bool equals(const typespec& rhs) const
		{
			const typespec_name& r = dynamic_cast<const typespec_name&>(rhs);
			return this->name == r.name;
		}

		size_t hash() const
		{
			return std::hash<std::string>()(name);
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct typespec_pointer : public typespec
	{
		const std::shared_ptr<typespec> base;

		typespec_pointer(const std::shared_ptr<typespec>& base, source_pos position)
			: base(base), typespec(position)
		{
		}

		bool equals(const typespec& rhs) const
		{
			const typespec_pointer& r = dynamic_cast<const typespec_pointer&>(rhs);
			return *this->base == *r.base;
		}

		size_t hash() const
		{
			return 217 + this->base->hash();
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct typespec_func : public typespec
	{
		const std::vector<std::shared_ptr<typespec>> argument_types;
		const std::shared_ptr<typespec> return_type;

		typespec_func(const std::vector<std::shared_ptr<typespec>>& argument_types, const std::shared_ptr<typespec>& return_type, source_pos position)
			: argument_types(argument_types), return_type(return_type), typespec(position)
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

		size_t hash() const
		{
			size_t h = 71;
			h = 113 * h + this->return_type->hash();
			for(const auto& a : this->argument_types)
			{
				h = 113 * h + a->hash();
			}
			return h;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	//
	// Expressions
	//

	struct expr : public ast_node
	{
		expr(source_pos position)
			: ast_node(position)
		{
		}

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

		expr_integer(uint64_t value, source_pos position)
			: value(value), expr(position)
		{
		}

		bool equals(const expr& rhs) const
		{
			const expr_integer& r = dynamic_cast<const expr_integer&>(rhs);
			return this->value == r.value;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_boolean : public expr
	{
		const bool value;

		expr_boolean(bool value, source_pos position)
			: value(value), expr(position)
		{
		}
		
		bool equals(const expr& rhs) const
		{
			const expr_boolean& r = dynamic_cast<const expr_boolean&>(rhs);
			return this->value == r.value;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct expr_name : public expr
	{
		const std::string name;

		expr_name(const std::string& name, source_pos position)
			: name(name), expr(position)
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

		expr_binary(binary_op op, const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& rhs, source_pos position)
			: op(op), lhs(lhs), rhs(rhs), expr(position)
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

		expr_unary(unary_op op, const std::shared_ptr<expr>& rhs, source_pos position)
			: op(op), rhs(rhs), expr(position)
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

		expr_call(const std::shared_ptr<expr>& lhs, const std::vector<std::shared_ptr<expr>>& args, source_pos position)
			: lhs(lhs), args(args), expr(position)
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

		expr_index(const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& index, source_pos position)
			: lhs(lhs), index(index), expr(position)
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

		expr_access(const std::shared_ptr<expr>& lhs, const std::string& field, source_pos position)
			: lhs(lhs), field(field), expr(position)
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

		expr_cast(const std::shared_ptr<expr>& lhs, const std::shared_ptr<typespec>& to_type, source_pos position)
			: lhs(lhs), to_type(to_type), expr(position)
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
		stmt(source_pos position)
			: ast_node(position)
		{
		}

		bool operator==(const stmt& rhs)
		{
			if(typeid(*this) != typeid(rhs)) { return false; }
			return equals(rhs);
		}

		bool operator!=(const stmt& rhs)
		{
			return !(*this == rhs);
		}

		virtual bool equals(const stmt& rhs) const = 0;
	};

	struct stmt_expr : public stmt
	{
		const std::shared_ptr<expr> expression;

		stmt_expr(const std::shared_ptr<expr>& expression, source_pos position)
			: expression(expression), stmt(position)
		{
		}

		bool equals(const stmt& rhs) const
		{
			const stmt_expr& r = dynamic_cast<const stmt_expr&>(rhs);
			return *this->expression == *r.expression;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_let : public stmt
	{
		const std::string name;
		const std::shared_ptr<typespec> type;
		const std::shared_ptr<expr> initializer;

		stmt_let(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer, source_pos position)
			: name(name), type(type), initializer(initializer), stmt(position)
		{
		}

		bool equals(const stmt& rhs) const
		{
			const stmt_let& r = dynamic_cast<const stmt_let&>(rhs);

			if(this->name != r.name) { return false; }

			if(this->type != nullptr && r.type != nullptr)
			{
				if(*this->type != *r.type) { return false; }
			}
			else
			{
				if(this->type != r.type) { return false; }
			}

			if(this->initializer != nullptr && r.initializer != nullptr)
			{
				if(*this->initializer != *r.initializer) { return false; }
			}
			else
			{
				if(this->initializer != r.initializer) { return false; }
			}

			return true;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_const : public stmt
	{
		const std::string name;
		const std::shared_ptr<typespec> type;
		const std::shared_ptr<expr> initializer;

		stmt_const(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer, source_pos position)
			: name(name), type(type), initializer(initializer), stmt(position)
		{
		}

		bool equals(const stmt& rhs) const
		{
			const stmt_const& r = dynamic_cast<const stmt_const&>(rhs);
			
			if(this->name != r.name) { return false; }

			if(this->type != nullptr && r.type != nullptr)
			{
				if(*this->type != *r.type) { return false; }
			}
			else
			{
				if(this->type != r.type) { return false; }
			}

			if(this->initializer != nullptr && r.initializer != nullptr)
			{
				if(*this->initializer != *r.initializer) { return false; }
			}
			else
			{
				if(this->initializer != r.initializer) { return false; }
			}

			return true;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct stmt_return : public stmt
	{
		const std::shared_ptr<expr> expression;

		stmt_return(const std::shared_ptr<expr>& expression, source_pos position)
			: expression(expression), stmt(position)
		{
		}

		bool equals(const stmt& rhs) const
		{
			const stmt_return& r = dynamic_cast<const stmt_return&>(rhs);

			if(this->expression != nullptr && r.expression != nullptr)
			{
				if(*this->expression != *r.expression) { return false; }
			}
			else
			{
				if(this->expression != r.expression) { return false; }
			}

			return true;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct if_branch
	{
		const std::shared_ptr<expr> condition;
		const std::vector<std::shared_ptr<stmt>> body;

		if_branch(const std::shared_ptr<expr> condition, const std::vector<std::shared_ptr<stmt>> body)
			: condition(condition), body(body)
		{
		}

		bool equals(const if_branch& rhs) const
		{
			if(*this->condition != *rhs.condition) { return false; }

			if(this->body.size() != rhs.body.size()) { return false; }

			for(int i = 0; i < this->body.size(); i++)
			{
				if(*this->body[i] != *rhs.body[i]) { return false; }
			}

			return true;
		}
	};

	struct stmt_if : public stmt
	{
		const std::vector<if_branch> if_branches;
		const std::vector<std::shared_ptr<stmt>> else_branch;

		stmt_if(const std::vector<if_branch>& if_branches, const std::vector<std::shared_ptr<stmt>>& else_branch, source_pos position)
			: if_branches(if_branches), else_branch(else_branch), stmt(position)
		{
		}
		
		bool equals(const stmt& rhs) const
		{
			const stmt_if& r = dynamic_cast<const stmt_if&>(rhs);

			if(this->if_branches.size() != r.if_branches.size()) { return false; }
			if(this->else_branch.size() != r.else_branch.size()) { return false; }

			for(int i = 0; i < if_branches.size(); i++)
			{
				if(!this->if_branches[i].equals(r.if_branches[i])) { return false; }
			}

			return true;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};
	
	//
	// Declarations
	//

	struct decl : public ast_node
	{
		decl(source_pos position)
			: ast_node(position)
		{
		}

		bool operator==(const decl& rhs)
		{
			if(typeid(*this) != typeid(rhs)) { return false; }
			return equals(rhs);
		}

		bool operator!=(const decl& rhs)
		{
			return !(*this == rhs);
		}

		virtual bool equals(const decl& rhs) const = 0;
	};

	struct decl_import : public decl
	{
		const std::string path;

		decl_import(const std::string& path, source_pos position)
			: path(path), decl(position)
		{
		}

		bool equals(const decl& rhs) const
		{
			const decl_import& r = dynamic_cast<const decl_import&>(rhs);
			return this->path == r.path;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct decl_namespace : public decl
	{
		const std::string name;

		decl_namespace(const std::string& name, source_pos position)
			: name(name), decl(position)
		{
		}

		bool equals(const decl& rhs) const
		{
			const decl_namespace& r = dynamic_cast<const decl_namespace&>(rhs);
			return this->name == r.name;
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

		bool equals(const func_arg& rhs) const
		{
			return this->name == rhs.name && *this->type == *rhs.type;
		}
	};

	struct decl_func : public decl
	{
		const std::string name;
		const std::vector<func_arg> arguments;
		const std::shared_ptr<typespec> ret_type;
		const std::vector<std::shared_ptr<stmt>> body;

		decl_func(
			const std::string& name,
			const std::vector<func_arg>& arguments,
			const std::shared_ptr<typespec>& ret_type,
			const std::vector<std::shared_ptr<stmt>>& body,
			source_pos position
		) : name(name), arguments(arguments), ret_type(ret_type), body(body), decl(position)
		{
		}

		bool equals(const decl& rhs) const
		{
			const decl_func& r = dynamic_cast<const decl_func&>(rhs);

			if(this->name != r.name) { return false; }
		
			if(this->arguments.size() != r.arguments.size()) { return false; }

			if(this->body.size() != r.body.size()) { return false; }

			for(int i = 0; i < this->arguments.size(); i++)
			{
				if(!this->arguments[i].equals(r.arguments[i])) { return false; }
			}
			
			for(int i = 0; i < this->body.size(); i++)
			{
				if(*this->body[i] != *r.body[i]) { return false; }
			}

			return true;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct struct_field
	{
		const std::string name;
		const std::shared_ptr<typespec> type;

		struct_field(const std::string& name, const std::shared_ptr<typespec>& type)
			: name(name), type(type)
		{
		}

		bool equals(const struct_field& rhs) const
		{
			return this->name == rhs.name && *this->type == *rhs.type;
		}
	};

	struct decl_struct : public decl
	{
		const std::string name;
		const std::vector<struct_field> fields;
		const std::vector<std::shared_ptr<decl_func>> functions;

		decl_struct(const std::string& name, const std::vector<struct_field>& fields, const std::vector<std::shared_ptr<decl_func>>& functions, source_pos position)
			: name(name), fields(fields), functions(functions), decl(position)
		{
		}

		bool equals(const decl& rhs) const
		{
			const decl_struct& r = dynamic_cast<const decl_struct&>(rhs);

			if(this->name != r.name) { return false; }

			if(this->fields.size() != r.fields.size()) { return false; }

			if(this->functions.size() != r.functions.size()) { return false; }

			for(int i = 0; i < this->fields.size(); i++)
			{
				if(!this->fields[i].equals(r.fields[i])) { return false; }
			}
			
			for(int i = 0; i < this->functions.size(); i++)
			{
				if(*this->functions[i] != *r.functions[i]) { return false; }
			}

			return true;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	struct decl_alias : public decl
	{
		const std::string name;
		const std::shared_ptr<typespec> type;

		decl_alias(const std::string& name, const std::shared_ptr<typespec>& type, source_pos position)
			: name(name), type(type), decl(position)
		{
		}

		bool equals(const decl& rhs) const
		{
			const decl_alias& r = dynamic_cast<const decl_alias&>(rhs);
			return this->name == r.name && *this->type == *r.type;
		}

		void accept(ast_visitor& v) const { v.visit(*this); }
	};

	//
	// Utilities
	//

	static auto inline make_integer_expr(uint64_t value, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_integer>(new expr_integer(value, position));
	}

	static auto inline make_boolean_expr(bool value, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_boolean>(new expr_boolean(value, position));
	}

	static auto inline make_name_expr(const std::string& name, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_name>(new expr_name(name, position));
	}

	static auto inline make_binary_expr(binary_op op, const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& rhs, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_binary>(new expr_binary(op, lhs, rhs, position));
	}

	static auto inline make_unary_expr(unary_op op, const std::shared_ptr<expr>& rhs, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_unary>(new expr_unary(op, rhs, position));
	}

	static auto inline make_call_expr(const std::shared_ptr<expr>& lhs, const std::vector<std::shared_ptr<expr>>& args, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_call>(new expr_call(lhs, args, position));
	}

	static auto inline make_index_expr(const std::shared_ptr<expr>& lhs, const std::shared_ptr<expr>& index, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_index>(new expr_index(lhs, index, position));
	}

	static auto inline make_access_expr(const std::shared_ptr<expr>& lhs, const std::string& field, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_access>(new expr_access(lhs, field, position));
	}

	static auto make_cast_expr(const std::shared_ptr<expr>& lhs, const std::shared_ptr<typespec>& to_type, source_pos position = source_pos())
	{
		return std::shared_ptr<expr_cast>(new expr_cast(lhs, to_type, position));
	}

	static auto inline make_name_typespec(const std::string& name, source_pos position = source_pos())
	{
		return std::shared_ptr<typespec_name>(new typespec_name(name, position));
	}

	static auto inline make_pointer_typespec(const std::shared_ptr<typespec>& base, source_pos position = source_pos())
	{
		return std::shared_ptr<typespec_pointer>(new typespec_pointer(base, position));
	}

	static auto inline make_func_typespec(const std::vector<std::shared_ptr<typespec>>& argument_types, const std::shared_ptr<typespec>& return_type, source_pos position = source_pos())
	{
		return std::shared_ptr<typespec_func>(new typespec_func(argument_types, return_type, position));
	}

	static auto inline make_expr_stmt(const std::shared_ptr<expr>& expression, source_pos position = source_pos())
	{
		return std::shared_ptr<stmt_expr>(new stmt_expr(expression, position));
	}

	static auto inline make_let_stmt(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer, source_pos position = source_pos())
	{
		return std::shared_ptr<stmt_let>(new stmt_let(name, type, initializer, position));
	}

	static auto inline make_const_stmt(const std::string& name, const std::shared_ptr<typespec>& type, const std::shared_ptr<expr>& initializer, source_pos position = source_pos())
	{
		return std::shared_ptr<stmt_const>(new stmt_const(name, type, initializer, position));
	}

	static auto inline make_return_stmt(const std::shared_ptr<expr>& ret_expr, source_pos position = source_pos())
	{
		return std::shared_ptr<stmt_return>(new stmt_return(ret_expr, position));
	}

	static auto inline make_if_stmt(const std::vector<if_branch>& if_branches, const std::vector<std::shared_ptr<stmt>>& else_branch, source_pos position = source_pos())
	{
		return std::shared_ptr<stmt_if>(new stmt_if(if_branches, else_branch, position));
	}

	static auto inline make_import_decl(const std::string& path, source_pos position = source_pos())
	{
		return std::shared_ptr<decl_import>(new decl_import(path, position));
	}

	static auto inline make_namespace_decl(const std::string& name, source_pos position = source_pos())
	{
		return std::shared_ptr<decl_namespace>(new decl_namespace(name, position));
	}

	static auto inline make_func_decl(const std::string& name, const std::vector<func_arg>& arguments, const std::shared_ptr<typespec>& ret_type, const std::vector<std::shared_ptr<stmt>>& body, source_pos position = source_pos())
	{
		return std::shared_ptr<decl_func>(new decl_func(name, arguments, ret_type, body, position));
	}
	
	static auto inline make_struct_decl(const std::string name, const std::vector<struct_field>& fields, const std::vector<std::shared_ptr<decl_func>>& functions, source_pos position = source_pos())
	{
		return std::shared_ptr<decl_struct>(new decl_struct(name, fields, functions, position));
	}

	static auto inline make_alias_decl(const std::string& name, const std::shared_ptr<typespec>& type, source_pos position = source_pos())
	{
		return std::shared_ptr<decl_alias>(new decl_alias(name, type, position));
	}
}
