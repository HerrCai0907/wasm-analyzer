#pragma once

#include <type_traits>

namespace wa {

template <typename T>
concept Integral = std::is_integral_v<T>;

} // namespace wa
