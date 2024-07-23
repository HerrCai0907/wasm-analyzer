#include "parser.hpp"
#include "adt.hpp"
#include "concept.hpp"
#include "module.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace wa {

Parser::Parser(const char *path) {
  std::ifstream f{path, std::ios::binary | std::ios::in};
  f.seekg(0, std::ios::end);
  m_binary.resize(f.tellg());
  f.seekg(0, std::ios::beg);
  f.read(reinterpret_cast<char *>(m_binary.data()), m_binary.size());
}

template <class T, class U, class... Args> static bool start_with(std::span<T> a, U arg, Args... args) {
  if (a[0] != arg) {
    return false;
  }
  if constexpr (sizeof...(args) == 0) {
    return true;
  } else {
    return start_with<T, Args...>(a.subspan(1), args...);
  }
}

static void check_magic_number(std::span<const uint8_t> &binary) {
  if (!start_with(binary, 0x00, 0x61, 0x73, 0x6d)) {
    throw "Invalid magic number";
  }
  binary = binary.subspan(4U);
}

static void check_version(std::span<const uint8_t> &binary) {
  if (!start_with(binary, 0x01, 0x00, 0x00, 0x00)) {
    throw "Invalid version number";
  }
  binary = binary.subspan(4U);
}

enum class SectionKind : uint8_t {
  CustomSection,
  TypeSection,
  ImportSection,
  FunctionSection,
  TableSection,
  MemorySection,
  GlobalSection,
  ExportSection,
  StartSection,
  ElementSection,
  CodeSection,
  DataSection,
  DataCountSection,
};

std::ostream &operator<<(std::ostream &os, SectionKind kind) {
  static const std::unordered_map<SectionKind, std::string_view> section_names = {
      {SectionKind::CustomSection, "CustomSection"},       {SectionKind::TypeSection, "TypeSection"},
      {SectionKind::ImportSection, "ImportSection"},       {SectionKind::FunctionSection, "FunctionSection"},
      {SectionKind::TableSection, "TableSection"},         {SectionKind::MemorySection, "MemorySection"},
      {SectionKind::GlobalSection, "GlobalSection"},       {SectionKind::ExportSection, "ExportSection"},
      {SectionKind::StartSection, "StartSection"},         {SectionKind::ElementSection, "ElementSection"},
      {SectionKind::CodeSection, "CodeSection"},           {SectionKind::DataSection, "DataSection"},
      {SectionKind::DataCountSection, "DataCountSection"},
  };
  return os << section_names.at(kind);
}

static uint8_t consume_byte(std::span<const uint8_t> &binary) {
  if (binary.empty()) {
    throw std::runtime_error{"empty_binary"};
  }
  uint8_t byte = binary[0];
  binary = binary.subspan(1);
  return byte;
}

template <size_t N> static uint64_t consume_unsigned_leb128(std::span<const uint8_t> &binary) {
  constexpr uint64_t pow_2_7 = 1ULL << 7U;
  constexpr uint64_t pow_2_N = 1ULL << N;
  uint64_t const n = static_cast<uint64_t>(consume_byte(binary));
  if ((n < pow_2_7) && (n < pow_2_N)) {
    return n;
  }
  if constexpr (N > 7) {
    if (n >= pow_2_7) {
      return (n - pow_2_7) + pow_2_7 * consume_unsigned_leb128<N - 7U>(binary);
    }
  }
  throw std::runtime_error{"invalid number"};
}

template <size_t N> static int64_t consume_signed_leb128(std::span<const uint8_t> &binary) {
  constexpr int64_t pow_2_6 = 1ULL << 6ULL;
  constexpr int64_t pow_2_7 = 1ULL << 7ULL;
  constexpr int64_t pow_2_N_1 = 1ULL << (N - 1ULL);
  int64_t const n = static_cast<int64_t>(static_cast<uint64_t>(consume_byte(binary)));
  if ((n < pow_2_6) && (n < pow_2_N_1)) {
    return n;
  }
  if ((n >= pow_2_6) && (n < pow_2_7)) {
    // n >= 0 -> if N >= 8, n >= pow_2_7 - pow_2_N_1 is true
    if constexpr (N >= 8) {
      return n - pow_2_7;
    } else {
      if ((n >= pow_2_7 - pow_2_N_1)) {
        return n - pow_2_7;
      }
    }
  }
  if constexpr (N > 7) {
    if (n >= pow_2_7) {
      return (n - pow_2_7) + pow_2_7 * consume_signed_leb128<N - 7U>(binary);
    }
  }
  throw std::runtime_error{"invalid number"};
}

template <Integral T> T consume_leb128(std::span<const uint8_t> &binary) {
  if constexpr (std::is_signed_v<T>) {
    return static_cast<T>(consume_signed_leb128<sizeof(T) * 8U>(binary));
  } else {
    return static_cast<T>(consume_unsigned_leb128<sizeof(T) * 8U>(binary));
  }
}

static std::pair<SectionKind, std::span<const uint8_t>> consume_section(std::span<const uint8_t> &binary) {
  SectionKind const kind = static_cast<SectionKind>(consume_byte(binary));
  uint32_t const size = consume_leb128<uint32_t>(binary);
  std::span<const uint8_t> const section{binary.data(), size};
  binary = binary.subspan(size);
  return {kind, section};
}

static WasmType consume_ref_type(std::span<const uint8_t> &binary) {
  WasmType const type = static_cast<WasmType>(consume_byte(binary));
  switch (type) {
  case WasmType::FuncRef:
  case WasmType::ExternRef:
    break;
  default:
    throw std::runtime_error("invalid ref type");
  }
  return type;
}

static WasmType consume_val_type(std::span<const uint8_t> &binary) {
  WasmType const type = static_cast<WasmType>(consume_byte(binary));
  switch (type) {
  case WasmType::I32:
  case WasmType::I64:
  case WasmType::F32:
  case WasmType::F64:
  case WasmType::V128:
  case WasmType::FuncRef:
  case WasmType::ExternRef:
    break;
  default:
    throw std::runtime_error("invalid type");
  }
  return type;
}

static std::vector<WasmType> consume_result_type(std::span<const uint8_t> &binary) {
  std::vector<WasmType> ret{};
  uint32_t const n = consume_leb128<uint32_t>(binary);
  ret.resize(n);
  for (WasmType &t : ret) {
    t = consume_val_type(binary);
  }
  return ret;
}

static std::shared_ptr<FunctionType> consume_func_type(std::span<const uint8_t> &binary) {
  uint8_t const prefix = consume_byte(binary);
  if (prefix != 0x60) {
    throw std::runtime_error{"invalid function type"};
  }
  std::vector<WasmType> arguments = consume_result_type(binary);
  std::vector<WasmType> results = consume_result_type(binary);
  return std::make_shared<FunctionType>(std::move(arguments), std::move(results));
}

static Limit consume_limit(std::span<const uint8_t> &binary) {
  Limit ret{};
  uint8_t const limit_kind = consume_byte(binary);
  switch (limit_kind) {
  case 0: {
    ret.min = consume_leb128<uint32_t>(binary);
    break;
  }
  case 1: {
    ret.min = consume_leb128<uint32_t>(binary);
    ret.max = consume_leb128<uint32_t>(binary);
    break;
  }
  default:
    throw std::runtime_error("invalid limit kind");
  }
  return ret;
}

static void consume_table_type(std::span<const uint8_t> &binary) {
  consume_ref_type(binary);
  consume_limit(binary);
}

static void consume_mem_type(std::span<const uint8_t> &binary) { consume_limit(binary); }

static Global consume_global_type(std::span<const uint8_t> &binary) {
  WasmType const type = consume_val_type(binary);
  uint8_t mut_kind = consume_byte(binary);
  bool const is_mut = mut_kind == 0x01;
  return Global{type, is_mut};
}

static std::string consume_name(std::span<const uint8_t> &binary) {
  std::string name{};
  uint32_t const n = consume_leb128<uint32_t>(binary);
  name.resize(n);
  for (char &c : name) {
    c = static_cast<char>(consume_byte(binary));
  }
  return name;
}

static void parse_type_section(Module &m, std::span<const uint8_t> binary) {
  uint32_t const n = consume_leb128<uint32_t>(binary);
  m.m_function_types.resize(n);
  for (std::shared_ptr<FunctionType> &t : m.m_function_types) {
    t = consume_func_type(binary);
  }
}

static void parse_import_section(Module &m, std::span<const uint8_t> binary) {
  uint32_t const n = consume_leb128<uint32_t>(binary);
  for (size_t i : Range{n}) {
    std::string mod = consume_name(binary);
    std::string nm = consume_name(binary);
    uint8_t const import_desc_kind = consume_byte(binary);
    switch (import_desc_kind) {
    case 0: {
      uint32_t const type_index = consume_leb128<uint32_t>(binary);
      m.m_functions.push_back(std::make_shared<Function>());
      m.m_functions.back()->set_type(m.m_function_types.at(type_index));
      break;
    }
    case 1: {
      consume_table_type(binary);
      break;
    }
    case 2: {
      consume_mem_type(binary);
      break;
    }
    case 3: {
      consume_global_type(binary);
      break;
    } break;
    default:
      throw std::runtime_error("invalid import desc kind");
    }
  }
}

static void parse_function_section(Module &m, std::span<const uint8_t> binary) {
  uint32_t const n = consume_leb128<uint32_t>(binary);
  for (size_t i : Range{n}) {
    uint32_t const type_index = consume_leb128<uint32_t>(binary);
    m.m_functions.push_back(std::make_shared<Function>());
    m.m_functions.back()->set_type(m.m_function_types.at(type_index));
  }
}

static void parse_table_section(Module &m, std::span<const uint8_t> binary) {}

static void parse_memory_section(Module &m, std::span<const uint8_t> binary) {}

static void parse_global_section(Module &m, std::span<const uint8_t> binary) {}

static void parse_export_section(Module &m, std::span<const uint8_t> binary) {}

static void parse_element_section(Module &m, std::span<const uint8_t> binary) {}

static std::shared_ptr<FunctionType> consume_block_type(Module const &m, std::span<const uint8_t> &binary) {
  std::span<const uint8_t> forked_binary = binary;
  uint8_t const byte = consume_byte(forked_binary);
  if (byte == 0x40) {
    binary = forked_binary;
    return std::make_shared<FunctionType>(std::vector<WasmType>{}, std::vector<WasmType>{});
  }
  switch (static_cast<WasmType>(byte)) {
  case WasmType::I32:
  case WasmType::I64:
  case WasmType::F32:
  case WasmType::F64:
  case WasmType::V128:
  case WasmType::FuncRef:
  case WasmType::ExternRef:
    forked_binary = binary;
    return std::make_shared<FunctionType>(std::vector<WasmType>{}, std::vector<WasmType>{static_cast<WasmType>(byte)});
    break;
  }
  int64_t const index = consume_signed_leb128<33>(binary);
  if (index < 0)
    throw std::runtime_error("negative block type");
  return m.m_function_types.at(static_cast<size_t>(index));
}

static Instr consume_instr(Module const &m, std::span<const uint8_t> &binary) {
  uint16_t code = static_cast<uint16_t>(consume_byte(binary));
  if (code == SATURATING_TRUNCATION_PREFIX) {
    uint32_t const postfix = consume_leb128<uint32_t>(binary);
    code = (code << 8U) + postfix;
  }
  Instr instr{static_cast<InstrCode>(code)};
  switch (instr.get_code()) {
  case InstrCode::UNREACHABLE:
  case InstrCode::NOP:
    break;
  case InstrCode::BLOCK:
  case InstrCode::LOOP:
  case InstrCode::IF:
    instr.set_function_type(consume_block_type(m, binary));
    break;
  case InstrCode::ELSE:
  case InstrCode::END:
    break;
  case InstrCode::BR:
  case InstrCode::BR_IF:
    instr.set_index(consume_leb128<uint32_t>(binary));
    break;
  case InstrCode::BR_TABLE: {
    uint8_t const n = consume_leb128<uint32_t>(binary);
    for (size_t i : Range{n}) {
      uint32_t const label_index = consume_leb128<uint32_t>(binary);
    }
    uint32_t const label_index = consume_leb128<uint32_t>(binary);
    break;
  }
  case InstrCode::RETURN:
    break;
  case InstrCode::CALL:
    instr.set_index(consume_leb128<uint32_t>(binary));
    break;
  case InstrCode::CALL_INDIRECT: {
    uint32_t const type_index = consume_leb128<uint32_t>(binary);
    uint32_t const table_index = consume_leb128<uint32_t>(binary);
    instr.set_function_type(m.m_function_types.at(type_index));
    break;
  }
  case InstrCode::DROP:
  case InstrCode::SELECT:
    break;

  case InstrCode::LOCAL_GET:
  case InstrCode::LOCAL_SET:
  case InstrCode::LOCAL_TEE:
  case InstrCode::GLOBAL_GET:
  case InstrCode::GLOBAL_SET:
    instr.set_index(consume_leb128<uint32_t>(binary));
    break;

  case InstrCode::I32_LOAD:
  case InstrCode::I64_LOAD:
  case InstrCode::F32_LOAD:
  case InstrCode::F64_LOAD:
  case InstrCode::I32_LOAD8_S:
  case InstrCode::I32_LOAD8_U:
  case InstrCode::I32_LOAD16_S:
  case InstrCode::I32_LOAD16_U:
  case InstrCode::I64_LOAD8_S:
  case InstrCode::I64_LOAD8_U:
  case InstrCode::I64_LOAD16_S:
  case InstrCode::I64_LOAD16_U:
  case InstrCode::I64_LOAD32_S:
  case InstrCode::I64_LOAD32_U:
  case InstrCode::I32_STORE:
  case InstrCode::I64_STORE:
  case InstrCode::F32_STORE:
  case InstrCode::F64_STORE:
  case InstrCode::I32_STORE8:
  case InstrCode::I32_STORE16:
  case InstrCode::I64_STORE8:
  case InstrCode::I64_STORE16:
  case InstrCode::I64_STORE32: {
    uint32_t const align = consume_leb128<uint32_t>(binary);
    uint32_t const offset = consume_leb128<uint32_t>(binary);
    instr.set_memarg(align, offset);
  }
  case InstrCode::MEMORY_SIZE:
  case InstrCode::MEMORY_GROW:
    if (0x00 != consume_byte(binary))
      throw std::runtime_error("invliad memory instruction");
    break;

  case InstrCode::I32CONST:
    instr.set_value(consume_leb128<int32_t>(binary));
    break;
  case InstrCode::I64CONST:
    instr.set_value(consume_leb128<int64_t>(binary));
    break;
  case InstrCode::F32CONST: {
    std::array<uint8_t, 4U> v{};
    for (uint8_t &b : v)
      b = consume_byte(binary);
    instr.set_value(std::bit_cast<float>(v));
    break;
  }
  case InstrCode::F64CONST: {
    std::array<uint8_t, 8U> v{};
    for (uint8_t &b : v)
      b = consume_byte(binary);
    instr.set_value(std::bit_cast<double>(v));
    break;
  }

  case InstrCode::I32EQZ:
  case InstrCode::I32EQ:
  case InstrCode::I32NE:
  case InstrCode::I32LT_S:
  case InstrCode::I32LT_U:
  case InstrCode::I32GT_S:
  case InstrCode::I32GT_U:
  case InstrCode::I32LE_S:
  case InstrCode::I32LE_U:
  case InstrCode::I32GE_S:
  case InstrCode::I32GE_U:
  case InstrCode::I64EQZ:
  case InstrCode::I64EQ:
  case InstrCode::I64NE:
  case InstrCode::I64LT_S:
  case InstrCode::I64LT_U:
  case InstrCode::I64GT_S:
  case InstrCode::I64GT_U:
  case InstrCode::I64LE_S:
  case InstrCode::I64LE_U:
  case InstrCode::I64GE_S:
  case InstrCode::I64GE_U:
  case InstrCode::F32EQ:
  case InstrCode::F32NE:
  case InstrCode::F32LT:
  case InstrCode::F32GT:
  case InstrCode::F32LE:
  case InstrCode::F32GE:
  case InstrCode::F64EQ:
  case InstrCode::F64NE:
  case InstrCode::F64LT:
  case InstrCode::F64GT:
  case InstrCode::F64LE:
  case InstrCode::F64GE:
  case InstrCode::I32CLZ:
  case InstrCode::I32CTZ:
  case InstrCode::I32POPCNT:
  case InstrCode::I32ADD:
  case InstrCode::I32SUB:
  case InstrCode::I32MUL:
  case InstrCode::I32DIV_S:
  case InstrCode::I32DIV_U:
  case InstrCode::I32REM_S:
  case InstrCode::I32REM_U:
  case InstrCode::I32AND:
  case InstrCode::I32OR:
  case InstrCode::I32XOR:
  case InstrCode::I32SHL:
  case InstrCode::I32SHR_S:
  case InstrCode::I32SHR_U:
  case InstrCode::I32ROTL:
  case InstrCode::I32ROTR:
  case InstrCode::I64CLZ:
  case InstrCode::I64CTZ:
  case InstrCode::I64POPCNT:
  case InstrCode::I64ADD:
  case InstrCode::I64SUB:
  case InstrCode::I64MUL:
  case InstrCode::I64DIV_S:
  case InstrCode::I64DIV_U:
  case InstrCode::I64REM_S:
  case InstrCode::I64REM_U:
  case InstrCode::I64AND:
  case InstrCode::I64OR:
  case InstrCode::I64XOR:
  case InstrCode::I64SHL:
  case InstrCode::I64SHR_S:
  case InstrCode::I64SHR_U:
  case InstrCode::I64ROTL:
  case InstrCode::I64ROTR:
  case InstrCode::F32ABS:
  case InstrCode::F32NEG:
  case InstrCode::F32CEIL:
  case InstrCode::F32FLOOR:
  case InstrCode::F32TRUNC:
  case InstrCode::F32NEAREST:
  case InstrCode::F32SQRT:
  case InstrCode::F32ADD:
  case InstrCode::F32SUB:
  case InstrCode::F32MUL:
  case InstrCode::F32DIV:
  case InstrCode::F32MIN:
  case InstrCode::F32MAX:
  case InstrCode::F32COPYSIGN:
  case InstrCode::F64ABS:
  case InstrCode::F64NEG:
  case InstrCode::F64CEIL:
  case InstrCode::F64FLOOR:
  case InstrCode::F64TRUNC:
  case InstrCode::F64NEAREST:
  case InstrCode::F64SQRT:
  case InstrCode::F64ADD:
  case InstrCode::F64SUB:
  case InstrCode::F64MUL:
  case InstrCode::F64DIV:
  case InstrCode::F64MIN:
  case InstrCode::F64MAX:
  case InstrCode::F64COPYSIGN:
  case InstrCode::I32WRAP_I64:
  case InstrCode::I32TRUNC_S_F32:
  case InstrCode::I32TRUNC_U_F32:
  case InstrCode::I32TRUNC_S_F64:
  case InstrCode::I32TRUNC_U_F64:
  case InstrCode::I64EXTEND_S_I32:
  case InstrCode::I64EXTEND_U_I32:
  case InstrCode::I64TRUNC_S_F32:
  case InstrCode::I64TRUNC_U_F32:
  case InstrCode::I64TRUNC_S_F64:
  case InstrCode::I64TRUNC_U_F64:
  case InstrCode::F32CONVERT_S_I32:
  case InstrCode::F32CONVERT_U_I32:
  case InstrCode::F32CONVERT_S_I64:
  case InstrCode::F32CONVERT_U_I64:
  case InstrCode::F32DEMOTE_F64:
  case InstrCode::F64CONVERT_S_I32:
  case InstrCode::F64CONVERT_U_I32:
  case InstrCode::F64CONVERT_S_I64:
  case InstrCode::F64CONVERT_U_I64:
  case InstrCode::F64PROMOTE_F32:
  case InstrCode::I32REINTERPRET_F32:
  case InstrCode::I64REINTERPRET_F64:
  case InstrCode::F32REINTERPRET_I32:
  case InstrCode::F64REINTERPRET_I64:
  case InstrCode::I32EXTEND8_S:
  case InstrCode::I32EXTEND16_S:
  case InstrCode::I64EXTEND8_S:
  case InstrCode::I64EXTEND16_S:
  case InstrCode::I64EXTEND32_S:
  case InstrCode::I32_TRUNC_SAT_F32_S:
  case InstrCode::I32_TRUNC_SAT_F32_U:
  case InstrCode::I32_TRUNC_SAT_F64_S:
  case InstrCode::I32_TRUNC_SAT_F64_U:
  case InstrCode::I64_TRUNC_SAT_F32_S:
  case InstrCode::I64_TRUNC_SAT_F32_U:
  case InstrCode::I64_TRUNC_SAT_F64_S:
  case InstrCode::I64_TRUNC_SAT_F64_U:
    break;
  }
  std::cout << (int)instr.get_code() << "\n";
  return instr;
}

static void consume_code(Module const &m, std::span<const uint8_t> binary) {
  size_t const local_size = static_cast<size_t>(consume_leb128<uint32_t>(binary));
  std::vector<WasmType> locals{};
  for (size_t i : Range{local_size}) {
    uint32_t const count = consume_leb128<uint32_t>(binary);
    WasmType const type = consume_val_type(binary);
    for (size_t _ : Range{count})
      locals.push_back(type);
  }
  std::vector<Instr> instrs{};
  while (binary.size() > 0)
    instrs.push_back(consume_instr(m, binary));

  if (instrs.empty() || instrs.back().get_code() != InstrCode::END)
    throw std::runtime_error("code does not end with OP::END");
}

static void parse_code_section(Module &m, std::span<const uint8_t> binary) {
  size_t const n = static_cast<size_t>(consume_leb128<uint32_t>(binary));
  size_t const importFuncNumber =
      std::count_if(m.m_functions.begin(), m.m_functions.end(),
                    [](std::shared_ptr<Function> const &func) { return func->is_import(); });
  if (importFuncNumber + n != m.m_functions.size())
    throw std::runtime_error("not matched code count");
  for (size_t i : Range{n}) {
    uint32_t const size = consume_leb128<uint32_t>(binary);
    std::span<const uint8_t> code_binary = binary.subspan(0, size);

    consume_code(m, code_binary);

    binary = binary.subspan(size);
  }
}

static void parse_data_section(Module &m, std::span<const uint8_t> binary) {}

Module Parser::parse() {
  Module m{};

  constexpr std::array<uint8_t, 4U> version_number{};
  std::span<const uint8_t> binary(m_binary);

  check_magic_number(binary);
  check_version(binary);

  while (!binary.empty()) {
    auto const [kind, span] = consume_section(binary);
    switch (kind) {
    case SectionKind::TypeSection:
      parse_type_section(m, span);
      break;
    case SectionKind::ImportSection:
      parse_import_section(m, span);
      break;
    case SectionKind::FunctionSection:
      parse_function_section(m, span);
      break;
    case SectionKind::TableSection:
      parse_table_section(m, span);
      break;
    case SectionKind::MemorySection:
      parse_memory_section(m, span);
      break;
    case SectionKind::GlobalSection:
      parse_global_section(m, span);
      break;
    case SectionKind::ExportSection:
      parse_export_section(m, span);
      break;
    case SectionKind::ElementSection:
      parse_element_section(m, span);
      break;
    case SectionKind::CodeSection:
      parse_code_section(m, span);
      break;
    case SectionKind::DataSection:
      parse_data_section(m, span);
      break;
    default:
      break;
    }
  }
  return m;
}

} // namespace wa
