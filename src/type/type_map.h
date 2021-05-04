#pragma once

#include <unordered_map>
#include <memory>

#include "types.h"
#include "../parse/ast.h"

namespace arc
{
    class type_map
	{
	private:
		std::unordered_map<size_t, std::shared_ptr<type>> _map;
	public:
		template<typename T>
		std::shared_ptr<type> add(const std::shared_ptr<typespec>& key, T* value)
		{
			return _map[key->hash()] = std::shared_ptr<T>(value);
		}

		std::shared_ptr<type> get(const std::shared_ptr<typespec>& key);
	};
}
