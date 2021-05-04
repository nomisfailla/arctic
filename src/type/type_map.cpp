#include "type_map.h"

#include "../util/casting.h"

namespace arc
{
    std::shared_ptr<type> type_map::get(const std::shared_ptr<typespec>& key)
	{
		auto f = _map.find(key->hash());
		if(f != _map.end())
		{
			return f->second;
		}

		if(auto spec = arc::is<typespec_name>(key))
		{
            return nullptr;
		}

		if(auto spec = arc::is<typespec_pointer>(key))
		{
			auto base = get(spec->base);
			return add(key, new type_pointer(base));
		}

		if(auto spec = arc::is<typespec_func>(key))
		{
			auto return_type = get(spec->return_type);
			std::vector<std::shared_ptr<type>> argument_types;
			for(const auto& a : spec->argument_types)
			{
				argument_types.push_back(get(a));
			}
			return add(key, new type_func(return_type, argument_types));
		}

        return nullptr;
	}
}
