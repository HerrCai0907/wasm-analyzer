#pragma once

#include <stdexcept>
#include <string>

namespace wa {

class Todo : public std::runtime_error {
public:
  explicit Todo(std::string const &message) : std::runtime_error("TODO: " + message) {}
};

} // namespace wa
