#pragma once

#include <unordered_map>
#include <memory>

#include "../parse/ast.h"
#include "../type/types.h"
#include "../type/type_map.h"

namespace arc
{
    class lexical_scope
	{
	private:
		lexical_scope* _parent;
		std::unordered_map<std::string, std::shared_ptr<type>> _symbols;
	public:
		lexical_scope(lexical_scope* parent = nullptr)
			: _parent(parent)
		{
		}

		bool add(const std::string& name, const std::shared_ptr<type>& type)
		{
			if(_symbols.find(name) == _symbols.end())
			{
				_symbols[name] = type;
				return true;
			}
			return false;
		}

		std::shared_ptr<type> get(const std::string& name, bool recursive = true)
		{
			auto sym = _symbols.find(name);
			if(sym != _symbols.end())
			{
				return sym->second;
			}

			return recursive && _parent != nullptr ? _parent->get(name) : nullptr;
		}
	};

	class type_checker
	{
	private:
		lexical_scope _global_scope;
		type_map _type_map;

		std::vector<std::shared_ptr<decl>> _ast;
	public:
		type_checker(const std::vector<std::shared_ptr<decl>>& ast);

		void check();
	};
}
