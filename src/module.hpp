#pragma once

#include "adt/string.hpp"
#include "instruction.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace wa {

enum class WasmType : uint8_t {
  I32 = 0x7F,
  I64 = 0x7E,
  F32 = 0x7D,
  F64 = 0x7C,
  V128 = 0x7B,
  FuncRef = 0x0,
  ExternRef = 0x6F,
};

static inline std::ostream &operator<<(std::ostream &os, WasmType type) {
  static const std::unordered_map<WasmType, std::string_view> wam_type_names = {
      {WasmType::I32, "I32"},
      {WasmType::I64, "I64"},
      {WasmType::F32, "F32"},
      {WasmType::F64, "F64"},
      {WasmType::V128, "V128"},
      {WasmType::FuncRef, "FuncRef"},
      {WasmType::ExternRef, "ExternRef"},
  };
  return os << wam_type_names.at(type);
}

class FunctionType {
  std::vector<WasmType> m_arguments;
  std::vector<WasmType> m_results;

public:
  FunctionType(std::vector<WasmType> arguments, std::vector<WasmType> results)
      : m_arguments(std::move(arguments)), m_results(std::move(results)) {}

  friend std::ostream &operator<<(std::ostream &os, FunctionType const &type) {
    return os << "func (" << StringOperator::join(type.m_arguments, ", ") << ") => ("
              << StringOperator::join(type.m_results, ", ") << ")";
  }
};

struct Limit {
  uint32_t min;
  std::optional<uint32_t> max;
};

class Global {
  WasmType m_type;
  bool m_is_mut;

public:
  Global(WasmType type, bool is_mut) : m_type(type), m_is_mut(is_mut) {}
};

class Function {
  std::shared_ptr<FunctionType> m_type = nullptr;
  bool m_is_import = false;
  bool m_is_export = false;
  std::vector<Instr> m_instr{};

public:
  void set_type(std::shared_ptr<FunctionType> const &type) { m_type = type; }
  void set_is_import() { m_is_import = true; }
  void set_is_export() { m_is_export = true; }
  void set_instr(std::vector<Instr> instr) { m_instr = std::move(instr); }

  bool is_import() const { return m_is_import; }
  bool is_export() const { return m_is_export; }
  FunctionType const *get_type() const { return m_type.get(); }
  std::span<Instr> get_instr() { return {m_instr.begin(), m_instr.size()}; }
};

struct Module {
  std::vector<std::shared_ptr<FunctionType>> m_function_types{};
  std::vector<std::shared_ptr<Function>> m_functions{};
};

} // namespace wa
