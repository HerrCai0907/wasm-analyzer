#pragma once

#include <ranges>
#include <sstream>
#include <string>
#include <string_view>

namespace wa {

class StringOperator {
public:
  template <std::ranges::range Range> static std::string join(Range const &r, std::string_view delimiter) {
    std::stringstream ss;
    bool first = true;
    for (const auto &item : r) {
      if (!first) {
        ss << delimiter;
      }
      ss << item;
      first = false;
    }
    return ss.str();
  }
};

} // namespace wa
