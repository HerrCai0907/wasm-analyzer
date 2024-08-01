#pragma once

#include <type_traits>

namespace wa {

template <typename T>
concept Integral = std::is_integral_v<T>;

template <class T, class U>
concept Derived = std::is_base_of_v<U, T>;

} // namespace wa
