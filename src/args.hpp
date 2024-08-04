#pragma once

#include <argparse.hpp>

namespace wa {

class Args {
public:
  static argparse::ArgumentParser &get_arg_parser() {
    static argparse::ArgumentParser program{"wasm-analyzer"};
    return program;
  }
};

template <class T> struct Arg {
  T m_v;
  Arg(const char *name, T default_value) : m_v(default_value) {
    Args::get_arg_parser().add_argument(name).store_into(m_v);
  }
  operator T() const { return m_v; }
};

} // namespace wa
