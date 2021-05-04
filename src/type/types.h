#pragma once

#include <memory>
#include <vector>

namespace arc
{
	struct type
	{
		virtual ~type() = default;
	};

	struct type_none : public type
	{
	};

	struct type_bool : public type
	{
	};

	struct type_integer : public type
	{
		const bool is_signed;
		const size_t size;

		type_integer(bool is_signed, size_t size)
			: is_signed(is_signed), size(size)
		{
		}
	};

	struct type_float : public type
	{
		const size_t size;

		type_float(size_t size)
			: size(size)
		{
		}
	};

	struct type_pointer : public type
	{
		const std::shared_ptr<type> base;

		type_pointer(const std::shared_ptr<type>& base)
			: base(base)
		{
		}	
	};

	struct type_func : public type
	{
		const std::shared_ptr<type> return_type;
		const std::vector<std::shared_ptr<type>> argument_types;

		type_func(
			const std::shared_ptr<type>& return_type,
			const std::vector<std::shared_ptr<type>>& argument_types
		) : return_type(return_type), argument_types(argument_types)
		{
		}
	};
}
