#pragma once

#include <utility>

namespace wa {

template <class F> struct YCombinator {
  F f;

  template <class... Args> decltype(auto) operator()(Args &&...args) const {
    return f(*this, std::forward<Args>(args)...);
  }
};

template <class F> YCombinator<std::decay_t<F>> make_y_combinator(F &&f) { return {std::forward<F>(f)}; }

template <class F> YCombinator(F) -> YCombinator<F>;

} // namespace wa
