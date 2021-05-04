#pragma once

#include <memory>

namespace arc
{
    template<typename T, typename B>
    std::shared_ptr<T> is(const std::shared_ptr<B>& b)
    {
        return std::dynamic_pointer_cast<T>(b);
    }
}
