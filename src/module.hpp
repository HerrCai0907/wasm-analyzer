#pragma once

#include "adt.hpp"
#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <variant>
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

enum class InstrCode : uint8_t {
  // CONTROL FLOW OPERATORS
  UNREACHABLE, // 0x00
  NOP,         // 0x01
  BLOCK,       // 0x02
  LOOP,        // 0x03
  IF,          // 0x04
  ELSE,        // 0x05
  RESERVED_06, // 0x06
  RESERVED_07, // 0x07
  RESERVED_08, // 0x08
  RESERVED_09, // 0x09
  RESERVED_0A, // 0x0A
  END,         // 0x0B
  BR,          // 0x0C
  BR_IF,       // 0x0D
  BR_TABLE,    // 0x0E
  RETURN,      // 0x0F

  // CALL OPERATORS
  CALL,          // 0x10
  CALL_INDIRECT, // 0x11

  RETURN_CALL, // 0x12 Tail-call feature
  // (https://github.com/WebAssembly/tail-call/blob/master/proposals/tail-call/Overview.md)
  RETURN_CALL_INDIRECT, // 0x13 Tail-call feature
  // (https://github.com/WebAssembly/tail-call/blob/master/proposals/tail-call/Overview.md)
  RESERVED_14, // 0x14
  RESERVED_15, // 0x15
  RESERVED_16, // 0x16
  RESERVED_17, // 0x17
  RESERVED_18, // 0x18
  RESERVED_19, // 0x19

  // PARAMETRIC OPERATORS
  DROP,   // 0x1A
  SELECT, // 0x1B

  RESERVED_1C, // 0x1C
  RESERVED_1D, // 0x1D
  RESERVED_1E, // 0x1E
  RESERVED_1F, // 0x1F

  // VARIABLE ACCESS
  LOCAL_GET,  // 0x20
  LOCAL_SET,  // 0x21
  LOCAL_TEE,  // 0x22
  GLOBAL_GET, // 0x23
  GLOBAL_SET, // 0x24

  RESERVED_25, // 0x25
  RESERVED_26, // 0x26
  RESERVED_27, // 0x27

  // MEMORY-RELATED OPERATOR
  I32LOAD,     // 0x28
  I64LOAD,     // 0x29
  F32LOAD,     // 0x2A
  F64LOAD,     // 0x2B
  I32LOAD8_S,  // 0x2C
  I32LOAD8_U,  // 0x2D
  I32LOAD16_S, // 0x2E
  I32LOAD16_U, // 0x2F
  I64LOAD8_S,  // 0x30
  I64LOAD8_U,  // 0x31
  I64LOAD16_S, // 0x32
  I64LOAD16_U, // 0x33
  I64LOAD32_S, // 0x34
  I64LOAD32_U, // 0x35
  I32STORE,    // 0x36
  I64STORE,    // 0x37
  F32STORE,    // 0x38
  F64STORE,    // 0x39
  I32STORE8,   // 0x3A
  I32STORE16,  // 0x3B
  I64STORE8,   // 0x3C
  I64STORE16,  // 0x3D
  I64STORE32,  // 0x3E
  MEMORY_SIZE, // 0x3F
  MEMORY_GROW, // 0x40

  // CONSTANTS
  I32CONST, // 0x41
  I64CONST, // 0x42
  F32CONST, // 0x43
  F64CONST, // 0x44

  // COMPARISON OPERATORS + INVERTED CMP OPCODE
  I32EQZ,  // 0x45 UNREACHABLE
  I32EQ,   // 0x46 I32NE
  I32NE,   // 0x47 I32EQ
  I32LT_S, // 0x48 I32GE_S
  I32LT_U, // 0x49 I32GE_U
  I32GT_S, // 0x4A I32LE_S
  I32GT_U, // 0x4B I32LE_U
  I32LE_S, // 0x4C I32GT_S
  I32LE_U, // 0x4D I32GT_U
  I32GE_S, // 0x4E I32LT_S
  I32GE_U, // 0x4F I32LT_U

  I64EQZ,  // 0x50 UNREACHABLE
  I64EQ,   // 0x51 I64NE
  I64NE,   // 0x52 I64EQ
  I64LT_S, // 0x53 I64GE_S
  I64LT_U, // 0x54 I64GE_U
  I64GT_S, // 0x55 I64LE_S
  I64GT_U, // 0x56 I64LE_U
  I64LE_S, // 0x57 I64GT_S
  I64LE_U, // 0x58 I64GT_U
  I64GE_S, // 0x59 I64LT_S
  I64GE_U, // 0x5A I64LT_U

  F32EQ, // 0x5B F32NE
  F32NE, // 0x5C F32EQ
  F32LT, // 0x5D F32GE
  F32GT, // 0x5E F32LE
  F32LE, // 0x5F F32GT
  F32GE, // 0x60 F32LT

  F64EQ, // 0x61 F64NE
  F64NE, // 0x62 F64EQ
  F64LT, // 0x63 F64GE
  F64GT, // 0x64 F64LE
  F64LE, // 0x65 F64GT
  F64GE, // 0x66 F64LT

  // NUMERIC OPERATORS
  I32CLZ,    // 0x67
  I32CTZ,    // 0x68
  I32POPCNT, // 0x69
  I32ADD,    // 0x6A
  I32SUB,    // 0x6B
  I32MUL,    // 0x6C
  I32DIV_S,  // 0x6D
  I32DIV_U,  // 0x6E
  I32REM_S,  // 0x6F
  I32REM_U,  // 0x70
  I32AND,    // 0x71
  I32OR,     // 0x72
  I32XOR,    // 0x73
  I32SHL,    // 0x74
  I32SHR_S,  // 0x75
  I32SHR_U,  // 0x76
  I32ROTL,   // 0x77
  I32ROTR,   // 0x78

  I64CLZ,    // 0x79
  I64CTZ,    // 0x7A
  I64POPCNT, // 0x7B
  I64ADD,    // 0x7C
  I64SUB,    // 0x7D
  I64MUL,    // 0x7E
  I64DIV_S,  // 0x7F
  I64DIV_U,  // 0x80
  I64REM_S,  // 0x81
  I64REM_U,  // 0x82
  I64AND,    // 0x83
  I64OR,     // 0x84
  I64XOR,    // 0x85
  I64SHL,    // 0x86
  I64SHR_S,  // 0x87
  I64SHR_U,  // 0x88
  I64ROTL,   // 0x89
  I64ROTR,   // 0x8A

  // movss
  F32ABS,      // 0x8B
  F32NEG,      // 0x8C
  F32CEIL,     // 0x8D
  F32FLOOR,    // 0x8E
  F32TRUNC,    // 0x8F
  F32NEAREST,  // 0x90
  F32SQRT,     // 0x91
  F32ADD,      // 0x92
  F32SUB,      // 0x93
  F32MUL,      // 0x94
  F32DIV,      // 0x95
  F32MIN,      // 0x96
  F32MAX,      // 0x97
  F32COPYSIGN, // 0x98

  F64ABS,      // 0x99
  F64NEG,      // 0x9A
  F64CEIL,     // 0x9B
  F64FLOOR,    // 0x9C
  F64TRUNC,    // 0x9D
  F64NEAREST,  // 0x9E
  F64SQRT,     // 0x9F
  F64ADD,      // 0xA0
  F64SUB,      // 0xA1
  F64MUL,      // 0xA2
  F64DIV,      // 0xA3
  F64MIN,      // 0xA4
  F64MAX,      // 0xA5
  F64COPYSIGN, // 0xA6

  // CONVERSIONS
  I32WRAP_I64,    // 0xA7
  I32TRUNC_S_F32, // 0xA8
  I32TRUNC_U_F32, // 0xA9
  I32TRUNC_S_F64, // 0xAA
  I32TRUNC_U_F64, // 0xAB

  I64EXTEND_S_I32, // 0xAC
  I64EXTEND_U_I32, // 0xAD
  I64TRUNC_S_F32,  // 0xAE
  I64TRUNC_U_F32,  // 0xAF
  I64TRUNC_S_F64,  // 0xB0
  I64TRUNC_U_F64,  // 0xB1

  F32CONVERT_S_I32, // 0xB2
  F32CONVERT_U_I32, // 0xB3
  F32CONVERT_S_I64, // 0xB4
  F32CONVERT_U_I64, // 0xB5
  F32DEMOTE_F64,    // 0xB6

  F64CONVERT_S_I32, // 0xB7
  F64CONVERT_U_I32, // 0xB8
  F64CONVERT_S_I64, // 0xB9
  F64CONVERT_U_I64, // 0xBA
  F64PROMOTE_F32,   // 0xBB

  // REINTERPRETATIONS
  I32REINTERPRET_F32, // 0xBC
  I64REINTERPRET_F64, // 0xBD
  F32REINTERPRET_I32, // 0xBE
  F64REINTERPRET_I64, // 0xBF

  // SIGN EXTENSIONS
  I32EXTEND8_S,  // 0xC0
  I32EXTEND16_S, // 0xC1
  I64EXTEND8_S,  // 0xC2
  I64EXTEND16_S, // 0xC3
  I64EXTEND32_S  // 0xC4
};

class FunctionType;

class Instr {
  InstrCode m_code;
  std::variant<std::shared_ptr<FunctionType>, uint32_t, int32_t, int64_t, float, double> m_content;

public:
  Instr(InstrCode code) : m_code(code) {}
  void set_function_type(std::shared_ptr<FunctionType> const &function_type) { m_content = function_type; }
  void set_index(uint32_t index) { m_content = index; }
  void set_value(int32_t v) { m_content = v; }
  void set_value(int64_t v) { m_content = v; }
  void set_value(float v) { m_content = v; }
  void set_value(double v) { m_content = v; }

  InstrCode get_code() const { return m_code; }
};

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

public:
  void set_type(std::shared_ptr<FunctionType> const &type) { m_type = type; }
  void set_is_import() { m_is_import = true; }
  void set_is_export() { m_is_export = true; }

  bool is_import() const { return m_is_import; }
  bool is_export() const { return m_is_export; }
};

struct Module {
  std::vector<std::shared_ptr<FunctionType>> m_function_types{};
  std::vector<std::shared_ptr<Function>> m_functions{};
};

} // namespace wa
