#pragma once

#include <any>
#include <type_traits>

namespace wa {

template <typename T>
concept Integral = std::is_integral_v<T>;

template <class T, class U>
concept Derived = std::is_base_of_v<U, T>;

template <class T>
concept OptionValue = std::is_convertible_v<T, std::any>;

template <class Fn, class Ret, class... Args>
concept Callable = std::is_invocable_r_v<Ret, Fn, Args...>;

} // namespace wa
