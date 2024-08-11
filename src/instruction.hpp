#pragma once

#include <cstdint>
#include <memory>
#include <ostream>
#include <variant>
#include <vector>

namespace wa {

constexpr const uint16_t SATURATING_TRUNCATION_PREFIX = 0xFC;

enum class InstrCode : uint16_t {
  // CONTROL FLOW OPERATORS
  UNREACHABLE = 0x00,
  NOP = 0x01,
  BLOCK = 0x02,
  LOOP = 0x03,
  IF = 0x04,
  ELSE = 0x05,

  END = 0x0B,
  BR = 0x0C,
  BR_IF = 0x0D,
  BR_TABLE = 0x0E,
  RETURN = 0x0F,

  // CALL OPERATORS
  CALL = 0x10,
  CALL_INDIRECT = 0x11,

  // PARAMETRIC OPERATORS
  DROP = 0x1A,
  SELECT = 0x1B,

  // VARIABLE ACCESS
  LOCAL_GET = 0x20,
  LOCAL_SET = 0x21,
  LOCAL_TEE = 0x22,
  GLOBAL_GET = 0x23,
  GLOBAL_SET = 0x24,

  // MEMORY-RELATED OPERATOR
  I32_LOAD = 0x28,
  I64_LOAD = 0x29,
  F32_LOAD = 0x2A,
  F64_LOAD = 0x2B,
  I32_LOAD8_S = 0x2C,
  I32_LOAD8_U = 0x2D,
  I32_LOAD16_S = 0x2E,
  I32_LOAD16_U = 0x2F,
  I64_LOAD8_S = 0x30,
  I64_LOAD8_U = 0x31,
  I64_LOAD16_S = 0x32,
  I64_LOAD16_U = 0x33,
  I64_LOAD32_S = 0x34,
  I64_LOAD32_U = 0x35,
  I32_STORE = 0x36,
  I64_STORE = 0x37,
  F32_STORE = 0x38,
  F64_STORE = 0x39,
  I32_STORE8 = 0x3A,
  I32_STORE16 = 0x3B,
  I64_STORE8 = 0x3C,
  I64_STORE16 = 0x3D,
  I64_STORE32 = 0x3E,
  MEMORY_SIZE = 0x3F,
  MEMORY_GROW = 0x40,

  // CONSTANTS
  I32_CONST = 0x41,
  I64_CONST = 0x42,
  F32_CONST = 0x43,
  F64_CONST = 0x44,

  // COMPARISON OPERATORS + INVERTED CMP OPCODE
  I32_EQZ = 0x45,  // UNREACHABLE
  I32_EQ = 0x46,   // I32_NE
  I32_NE = 0x47,   // I32_EQ
  I32_LT_S = 0x48, // I32_GE_S
  I32_LT_U = 0x49, // I32_GE_U
  I32_GT_S = 0x4A, // I32_LE_S
  I32_GT_U = 0x4B, // I32_LE_U
  I32_LE_S = 0x4C, // I32_GT_S
  I32_LE_U = 0x4D, // I32_GT_U
  I32_GE_S = 0x4E, // I32_LT_S
  I32_GE_U = 0x4F, // I32_LT_U

  I64_EQZ = 0x50,  // UNREACHABLE
  I64_EQ = 0x51,   // I64_NE
  I64_NE = 0x52,   // I64_EQ
  I64_LT_S = 0x53, // I64_GE_S
  I64_LT_U = 0x54, // I64_GE_U
  I64_GT_S = 0x55, // I64_LE_S
  I64_GT_U = 0x56, // I64_LE_U
  I64_LE_S = 0x57, // I64_GT_S
  I64_LE_U = 0x58, // I64_GT_U
  I64_GE_S = 0x59, // I64_LT_S
  I64_GE_U = 0x5A, // I64_LT_U

  F32_EQ = 0x5B, // F32_NE
  F32_NE = 0x5C, // F32_EQ
  F32_LT = 0x5D, // F32_GE
  F32_GT = 0x5E, // F32_LE
  F32_LE = 0x5F, // F32_GT
  F32_GE = 0x60, // F32_LT

  F64_EQ = 0x61, // F64_NE
  F64_NE = 0x62, // F64_EQ
  F64_LT = 0x63, // F64_GE
  F64_GT = 0x64, // F64_LE
  F64_LE = 0x65, // F64_GT
  F64_GE = 0x66, // F64_LT

  // NUMERIC OPERATORS
  I32_CLZ = 0x67,
  I32_CTZ = 0x68,
  I32_POPCNT = 0x69,
  I32_ADD = 0x6A,
  I32_SUB = 0x6B,
  I32_MUL = 0x6C,
  I32_DIV_S = 0x6D,
  I32_DIV_U = 0x6E,
  I32_REM_S = 0x6F,
  I32_REM_U = 0x70,
  I32_AND = 0x71,
  I32_OR = 0x72,
  I32_XOR = 0x73,
  I32_SHL = 0x74,
  I32_SHR_S = 0x75,
  I32_SHR_U = 0x76,
  I32_ROTL = 0x77,
  I32_ROTR = 0x78,

  I64_CLZ = 0x79,
  I64_CTZ = 0x7A,
  I64_POPCNT = 0x7B,
  I64_ADD = 0x7C,
  I64_SUB = 0x7D,
  I64_MUL = 0x7E,
  I64_DIV_S = 0x7F,
  I64_DIV_U = 0x80,
  I64_REM_S = 0x81,
  I64_REM_U = 0x82,
  I64_AND = 0x83,
  I64_OR = 0x84,
  I64_XOR = 0x85,
  I64_SHL = 0x86,
  I64_SHR_S = 0x87,
  I64_SHR_U = 0x88,
  I64_ROTL = 0x89,
  I64_ROTR = 0x8A,

  // movss
  F32_ABS = 0x8B,
  F32_NEG = 0x8C,
  F32_CEIL = 0x8D,
  F32_FLOOR = 0x8E,
  F32_TRUNC = 0x8F,
  F32_NEAREST = 0x90,
  F32_SQRT = 0x91,
  F32_ADD = 0x92,
  F32_SUB = 0x93,
  F32_MUL = 0x94,
  F32_DIV = 0x95,
  F32_MIN = 0x96,
  F32_MAX = 0x97,
  F32_COPYSIGN = 0x98,

  F64_ABS = 0x99,
  F64_NEG = 0x9A,
  F64_CEIL = 0x9B,
  F64_FLOOR = 0x9C,
  F64_TRUNC = 0x9D,
  F64_NEAREST = 0x9E,
  F64_SQRT = 0x9F,
  F64_ADD = 0xA0,
  F64_SUB = 0xA1,
  F64_MUL = 0xA2,
  F64_DIV = 0xA3,
  F64_MIN = 0xA4,
  F64_MAX = 0xA5,
  F64_COPYSIGN = 0xA6,

  // CONVERSIONS
  I32_WRAP_I64 = 0xA7,
  I32_TRUNC_S_F32 = 0xA8,
  I32_TRUNC_U_F32 = 0xA9,
  I32_TRUNC_S_F64 = 0xAA,
  I32_TRUNC_U_F64 = 0xAB,

  I64_EXTEND_S_I32 = 0xAC,
  I64_EXTEND_U_I32 = 0xAD,
  I64_TRUNC_S_F32 = 0xAE,
  I64_TRUNC_U_F32 = 0xAF,
  I64_TRUNC_S_F64 = 0xB0,
  I64_TRUNC_U_F64 = 0xB1,

  F32_CONVERT_S_I32 = 0xB2,
  F32_CONVERT_U_I32 = 0xB3,
  F32_CONVERT_S_I64 = 0xB4,
  F32_CONVERT_U_I64 = 0xB5,
  F32_DEMOTE_F64 = 0xB6,

  F64_CONVERT_S_I32 = 0xB7,
  F64_CONVERT_U_I32 = 0xB8,
  F64_CONVERT_S_I64 = 0xB9,
  F64_CONVERT_U_I64 = 0xBA,
  F64_PROMOTE_F32 = 0xBB,

  // REINTERPRETATIONS
  I32_REINTERPRET_F32 = 0xBC,
  I64_REINTERPRET_F64 = 0xBD,
  F32_REINTERPRET_I32 = 0xBE,
  F64_REINTERPRET_I64 = 0xBF,

  // SIGN EXTENSIONS
  I32_EXTEND8_S = 0xC0,
  I32_EXTEND16_S = 0xC1,
  I64_EXTEND8_S = 0xC2,
  I64_EXTEND16_S = 0xC3,
  I64_EXTEND32_S = 0xC4,

  // saturating truncation instructions
  I32_TRUNC_SAT_F32_S = (SATURATING_TRUNCATION_PREFIX << 8) + 0,
  I32_TRUNC_SAT_F32_U = (SATURATING_TRUNCATION_PREFIX << 8) + 1,
  I32_TRUNC_SAT_F64_S = (SATURATING_TRUNCATION_PREFIX << 8) + 2,
  I32_TRUNC_SAT_F64_U = (SATURATING_TRUNCATION_PREFIX << 8) + 3,
  I64_TRUNC_SAT_F32_S = (SATURATING_TRUNCATION_PREFIX << 8) + 4,
  I64_TRUNC_SAT_F32_U = (SATURATING_TRUNCATION_PREFIX << 8) + 5,
  I64_TRUNC_SAT_F64_S = (SATURATING_TRUNCATION_PREFIX << 8) + 6,
  I64_TRUNC_SAT_F64_U = (SATURATING_TRUNCATION_PREFIX << 8) + 7,
};

std::ostream &operator<<(std::ostream &os, InstrCode code);

class FunctionType;

struct Index {
  uint32_t m_v;
};

struct MemArg {
  uint32_t m_align;
  uint32_t m_offset;
};

struct None {};

class Instr {
  InstrCode m_code;
  std::variant<std::nullptr_t, std::shared_ptr<FunctionType>, Index, int32_t, int64_t, float, double, MemArg,
               std::vector<Index>>
      m_content;

public:
  Instr(InstrCode code) : m_code(code), m_content(nullptr) {}
  void set_function_type(std::shared_ptr<FunctionType> const &function_type) { m_content = function_type; }
  void set_index(uint32_t index) { m_content = Index{.m_v = index}; }
  void set_indexes(std::vector<Index> indexes) { m_content = indexes; }
  void set_value(int32_t v) { m_content = v; }
  void set_value(int64_t v) { m_content = v; }
  void set_value(float v) { m_content = v; }
  void set_value(double v) { m_content = v; }
  void set_mem_arg(uint32_t align, uint32_t offset) { m_content = MemArg{.m_align = align, .m_offset = offset}; }

  InstrCode get_code() const { return m_code; }
  uint32_t get_index() const { return std::get<Index>(m_content).m_v; }
  std::vector<Index> const &get_indexes() const { return std::get<std::vector<Index>>(m_content); }

  size_t get_operand_count() const;
  size_t get_result_count() const;

  friend std::ostream &operator<<(std::ostream &os, Instr const &instr);
};

} // namespace wa
