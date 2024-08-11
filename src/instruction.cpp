#include "instruction.hpp"
#include "adt/string.hpp"
#include "error.hpp"
#include "module.hpp"
#include <cstddef>
#include <type_traits>
#include <variant>

namespace wa {

std::ostream &operator<<(std::ostream &os, InstrCode code) {
  switch (code) {
  // CONTROL FLOW OPERATORS
  case InstrCode::UNREACHABLE:
    return os << "unreachable";
  case InstrCode::NOP:
    return os << "nop";
  case InstrCode::BLOCK:
    return os << "block";
  case InstrCode::LOOP:
    return os << "loop";
  case InstrCode::IF:
    return os << "if";
  case InstrCode::ELSE:
    return os << "else";

  case InstrCode::END:
    return os << "end";
  case InstrCode::BR:
    return os << "br";
  case InstrCode::BR_IF:
    return os << "br_if";
  case InstrCode::BR_TABLE:
    return os << "br_table";
  case InstrCode::RETURN:
    return os << "return";

  // CALL OPERATORS
  case InstrCode::CALL:
    return os << "call";
  case InstrCode::CALL_INDIRECT:
    return os << "call_indirect";

  // PARAMETRIC OPERATORS
  case InstrCode::DROP:
    return os << "drop";
  case InstrCode::SELECT:
    return os << "select";

  // VARIABLE ACCESS
  case InstrCode::LOCAL_GET:
    return os << "local.get";
  case InstrCode::LOCAL_SET:
    return os << "local.set";
  case InstrCode::LOCAL_TEE:
    return os << "local.tee";
  case InstrCode::GLOBAL_GET:
    return os << "global.get";
  case InstrCode::GLOBAL_SET:
    return os << "global.set";

  // MEMORY-RELATED OPERATOR
  case InstrCode::I32_LOAD:
    return os << "i32.load";
  case InstrCode::I64_LOAD:
    return os << "i64.load";
  case InstrCode::F32_LOAD:
    return os << "f32.load";
  case InstrCode::F64_LOAD:
    return os << "f64.load";
  case InstrCode::I32_LOAD8_S:
    return os << "i32.load8_s";
  case InstrCode::I32_LOAD8_U:
    return os << "i32.load8_u";
  case InstrCode::I32_LOAD16_S:
    return os << "i32.load16_s";
  case InstrCode::I32_LOAD16_U:
    return os << "i32.load16_u";
  case InstrCode::I64_LOAD8_S:
    return os << "i64.load8_s";
  case InstrCode::I64_LOAD8_U:
    return os << "i64.load8_u";
  case InstrCode::I64_LOAD16_S:
    return os << "i64.load16_s";
  case InstrCode::I64_LOAD16_U:
    return os << "i64.load16_u";
  case InstrCode::I64_LOAD32_S:
    return os << "i64.load32_s";
  case InstrCode::I64_LOAD32_U:
    return os << "i64.load32_u";
  case InstrCode::I32_STORE:
    return os << "i32.store";
  case InstrCode::I64_STORE:
    return os << "i64.store";
  case InstrCode::F32_STORE:
    return os << "f32.store";
  case InstrCode::F64_STORE:
    return os << "f64.store";
  case InstrCode::I32_STORE8:
    return os << "i32.store8";
  case InstrCode::I32_STORE16:
    return os << "i32.store16";
  case InstrCode::I64_STORE8:
    return os << "i64.store8";
  case InstrCode::I64_STORE16:
    return os << "i64.store16";
  case InstrCode::I64_STORE32:
    return os << "i64.store32";
  case InstrCode::MEMORY_SIZE:
    return os << "memory.size";
  case InstrCode::MEMORY_GROW:
    return os << "memory.grow";

  // CONSTANTS
  case InstrCode::I32_CONST:
    return os << "i32.const";
  case InstrCode::I64_CONST:
    return os << "i64.const";
  case InstrCode::F32_CONST:
    return os << "f32.const";
  case InstrCode::F64_CONST:
    return os << "f64.const";

  // COMPARISON OPERATORS + INVERTED CMP OPCODE
  case InstrCode::I32_EQZ:
    return os << "i32.eqz"; // Note: This is the actual opcode, not inverted.
  case InstrCode::I32_EQ:
    return os << "i32.ne"; // Inverted opcode.
  case InstrCode::I32_NE:
    return os << "i32.eq"; // Actual opcode.
  case InstrCode::I32_LT_S:
    return os << "i32.ge_s"; // Inverted opcode.
  case InstrCode::I32_LT_U:
    return os << "i32.ge_u"; // Inverted opcode.
  case InstrCode::I32_GT_S:
    return os << "i32.le_s"; // Inverted opcode.
  case InstrCode::I32_GT_U:
    return os << "i32.le_u"; // Inverted opcode.
  case InstrCode::I32_LE_S:
    return os << "i32.gt_s"; // Inverted opcode.
  case InstrCode::I32_LE_U:
    return os << "i32.gt_u"; // Inverted opcode.
  case InstrCode::I32_GE_S:
    return os << "i32.lt_s"; // Inverted opcode.
  case InstrCode::I32_GE_U:
    return os << "i32.lt_u"; // Inverted opcode.

  case InstrCode::I64_EQZ:
    return os << "i64.eqz"; // Note: This is the actual opcode, not inverted.
  case InstrCode::I64_EQ:
    return os << "i64.ne"; // Inverted opcode.
  case InstrCode::I64_NE:
    return os << "i64.eq"; // Actual opcode.
  case InstrCode::I64_LT_S:
    return os << "i64.ge_s"; // Inverted opcode.
  case InstrCode::I64_LT_U:
    return os << "i64.ge_u"; // Inverted opcode.
  case InstrCode::I64_GT_S:
    return os << "i64.le_s"; // Inverted opcode.
  case InstrCode::I64_GT_U:
    return os << "i64.le_u"; // Inverted opcode.
  case InstrCode::I64_LE_S:
    return os << "i64.gt_s"; // Inverted opcode.
  case InstrCode::I64_LE_U:
    return os << "i64.gt_u"; // Inverted opcode.
  case InstrCode::I64_GE_S:
    return os << "i64.lt_s"; // Inverted opcode.
  case InstrCode::I64_GE_U:
    return os << "i64.lt_u"; // Inverted opcode.

  case InstrCode::F32_EQ:
    return os << "f32.ne"; // Inverted opcode.
  case InstrCode::F32_NE:
    return os << "f32.eq"; // Actual opcode.
  case InstrCode::F32_LT:
    return os << "f32.ge"; // Inverted opcode.
  case InstrCode::F32_GT:
    return os << "f32.le"; // Inverted opcode.
  case InstrCode::F32_LE:
    return os << "f32.gt"; // Inverted opcode.
  case InstrCode::F32_GE:
    return os << "f32.lt"; // Inverted opcode.

  case InstrCode::F64_EQ:
    return os << "f64.ne"; // Inverted opcode.
  case InstrCode::F64_NE:
    return os << "f64.eq"; // Actual opcode.
  case InstrCode::F64_LT:
    return os << "f64.ge"; // Inverted opcode.
  case InstrCode::F64_GT:
    return os << "f64.le"; // Inverted opcode.
  case InstrCode::F64_LE:
    return os << "f64.gt"; // Inverted opcode.
  case InstrCode::F64_GE:
    return os << "f64.lt"; // Inverted opcode.

  // NUMERIC OPERATORS
  case InstrCode::I32_CLZ:
    return os << "i32.clz";
  case InstrCode::I32_CTZ:
    return os << "i32.ctz";
  case InstrCode::I32_POPCNT:
    return os << "i32.popcnt";
  case InstrCode::I32_ADD:
    return os << "i32.add";
  case InstrCode::I32_SUB:
    return os << "i32.sub";
  case InstrCode::I32_MUL:
    return os << "i32.mul";
  case InstrCode::I32_DIV_S:
    return os << "i32.div_s";
  case InstrCode::I32_DIV_U:
    return os << "i32.div_u";
  case InstrCode::I32_REM_S:
    return os << "i32.rem_s";
  case InstrCode::I32_REM_U:
    return os << "i32.rem_u";
  case InstrCode::I32_AND:
    return os << "i32.and";
  case InstrCode::I32_OR:
    return os << "i32.or";
  case InstrCode::I32_XOR:
    return os << "i32.xor";
  case InstrCode::I32_SHL:
    return os << "i32.shl";
  case InstrCode::I32_SHR_S:
    return os << "i32.shr_s";
  case InstrCode::I32_SHR_U:
    return os << "i32.shr_u";
  case InstrCode::I32_ROTL:
    return os << "i32.rotl";
  case InstrCode::I32_ROTR:
    return os << "i32.rotr";

  case InstrCode::I64_CLZ:
    return os << "i64.clz";
  case InstrCode::I64_CTZ:
    return os << "i64.ctz";
  case InstrCode::I64_POPCNT:
    return os << "i64.popcnt";
  case InstrCode::I64_ADD:
    return os << "i64.add";
  case InstrCode::I64_SUB:
    return os << "i64.sub";
  case InstrCode::I64_MUL:
    return os << "i64.mul";
  case InstrCode::I64_DIV_S:
    return os << "i64.div_s";
  case InstrCode::I64_DIV_U:
    return os << "i64.div_u";
  case InstrCode::I64_REM_S:
    return os << "i64.rem_s";
  case InstrCode::I64_REM_U:
    return os << "i64.rem_u";
  case InstrCode::I64_AND:
    return os << "i64.and";
  case InstrCode::I64_OR:
    return os << "i64.or";
  case InstrCode::I64_XOR:
    return os << "i64.xor";
  case InstrCode::I64_SHL:
    return os << "i64.shl";
  case InstrCode::I64_SHR_S:
    return os << "i64.shr_s";
  case InstrCode::I64_SHR_U:
    return os << "i64.shr_u";
  case InstrCode::I64_ROTL:
    return os << "i64.rotl";
  case InstrCode::I64_ROTR:
    return os << "i64.rotr";

  // Floating point arithmetic
  case InstrCode::F32_ABS:
    return os << "f32.abs";
  case InstrCode::F32_NEG:
    return os << "f32.neg";
  case InstrCode::F32_CEIL:
    return os << "f32.ceil";
  case InstrCode::F32_FLOOR:
    return os << "f32.floor";
  case InstrCode::F32_TRUNC:
    return os << "f32.trunc";
  case InstrCode::F32_NEAREST:
    return os << "f32.nearest";
  case InstrCode::F32_SQRT:
    return os << "f32.sqrt";
  case InstrCode::F32_ADD:
    return os << "f32.add";
  case InstrCode::F32_SUB:
    return os << "f32.sub";
  case InstrCode::F32_MUL:
    return os << "f32.mul";
  case InstrCode::F32_DIV:
    return os << "f32.div";
  case InstrCode::F32_MIN:
    return os << "f32.min";
  case InstrCode::F32_MAX:
    return os << "f32.max";
  case InstrCode::F32_COPYSIGN:
    return os << "f32.copysign";

  case InstrCode::F64_ABS:
    return os << "f64.abs";
  case InstrCode::F64_NEG:
    return os << "f64.neg";
  case InstrCode::F64_CEIL:
    return os << "f64.ceil";
  case InstrCode::F64_FLOOR:
    return os << "f64.floor";
  case InstrCode::F64_TRUNC:
    return os << "f64.trunc";
  case InstrCode::F64_NEAREST:
    return os << "f64.nearest";
  case InstrCode::F64_SQRT:
    return os << "f64.sqrt";
  case InstrCode::F64_ADD:
    return os << "f64.add";
  case InstrCode::F64_SUB:
    return os << "f64.sub";
  case InstrCode::F64_MUL:
    return os << "f64.mul";
  case InstrCode::F64_DIV:
    return os << "f64.div";
  case InstrCode::F64_MIN:
    return os << "f64.min";
  case InstrCode::F64_MAX:
    return os << "f64.max";
  case InstrCode::F64_COPYSIGN:
    return os << "f64.copysign";

  // CONVERSIONS
  case InstrCode::I32_WRAP_I64:
    return os << "i32.wrap/i64";
  case InstrCode::I32_TRUNC_S_F32:
    return os << "i32.trunc_s/f32";
  case InstrCode::I32_TRUNC_U_F32:
    return os << "i32.trunc_u/f32";
  case InstrCode::I32_TRUNC_S_F64:
    return os << "i32.trunc_s/f64";
  case InstrCode::I32_TRUNC_U_F64:
    return os << "i32.trunc_u/f64";

  case InstrCode::I64_EXTEND_S_I32:
    return os << "i64.extend_s/i32";
  case InstrCode::I64_EXTEND_U_I32:
    return os << "i64.extend_u/i32";
  case InstrCode::I64_TRUNC_S_F32:
    return os << "i64.trunc_s/f32";
  case InstrCode::I64_TRUNC_U_F32:
    return os << "i64.trunc_u/f32";
  case InstrCode::I64_TRUNC_S_F64:
    return os << "i64.trunc_s/f64";
  case InstrCode::I64_TRUNC_U_F64:
    return os << "i64.trunc_u/f64";

  case InstrCode::F32_CONVERT_S_I32:
    return os << "f32.convert_s/i32";
  case InstrCode::F32_CONVERT_U_I32:
    return os << "f32.convert_u/i32";
  case InstrCode::F32_CONVERT_S_I64:
    return os << "f32.convert_s/i64";
  case InstrCode::F32_CONVERT_U_I64:
    return os << "f32.convert_u/i64";
  case InstrCode::F32_DEMOTE_F64:
    return os << "f32.demote/f64";

  case InstrCode::F64_CONVERT_S_I32:
    return os << "f64.convert_s/i32";
  case InstrCode::F64_CONVERT_U_I32:
    return os << "f64.convert_u/i32";
  case InstrCode::F64_CONVERT_S_I64:
    return os << "f64.convert_s/i64";
  case InstrCode::F64_CONVERT_U_I64:
    return os << "f64.convert_u/i64";
  case InstrCode::F64_PROMOTE_F32:
    return os << "f64.promote/f32";

  // REINTERPRETATIONS
  case InstrCode::I32_REINTERPRET_F32:
    return os << "i32.reinterpret/f32";
  case InstrCode::I64_REINTERPRET_F64:
    return os << "i64.reinterpret/f64";
  case InstrCode::F32_REINTERPRET_I32:
    return os << "f32.reinterpret/i32";
  case InstrCode::F64_REINTERPRET_I64:
    return os << "f64.reinterpret/i64";

  // SIGN EXTENSIONS
  case InstrCode::I32_EXTEND8_S:
    return os << "i32.extend8_s";
  case InstrCode::I32_EXTEND16_S:
    return os << "i32.extend16_s";
  case InstrCode::I64_EXTEND8_S:
    return os << "i64.extend8_s";
  case InstrCode::I64_EXTEND16_S:
    return os << "i64.extend16_s";
  case InstrCode::I64_EXTEND32_S:
    return os << "i64.extend32_s";

  // saturating truncation instructions
  case InstrCode::I32_TRUNC_SAT_F32_S:
    return os << "i32.trunc_sat_f32_s";
  case InstrCode::I32_TRUNC_SAT_F32_U:
    return os << "i32.trunc_sat_f32_u";
  case InstrCode::I32_TRUNC_SAT_F64_S:
    return os << "i32.trunc_sat_f64_s";
  case InstrCode::I32_TRUNC_SAT_F64_U:
    return os << "i32.trunc_sat_f64_u";
  case InstrCode::I64_TRUNC_SAT_F32_S:
    return os << "i64.trunc_sat_f32_s";
  case InstrCode::I64_TRUNC_SAT_F32_U:
    return os << "i64.trunc_sat_f32_u";
  case InstrCode::I64_TRUNC_SAT_F64_S:
    return os << "i64.trunc_sat_f64_s";
  case InstrCode::I64_TRUNC_SAT_F64_U:
    return os << "i64.trunc_sat_f64_u";

  default:
    std::ostringstream ss;
    ss << "Unknown instruction: 0x" << std::hex << static_cast<uint16_t>(code);
    return os << ss.str();
  }
}

static std::ostream &operator<<(std::ostream &os, std::shared_ptr<FunctionType> const &type) { return os << *type; }
static std::ostream &operator<<(std::ostream &os, Index const &index) { return os << index.m_v; }
static std::ostream &operator<<(std::ostream &os, std::vector<Index> const &indexes) {
  return os << StringOperator::join(indexes, ", ");
}
static std::ostream &operator<<(std::ostream &os, MemArg const &mem_arg) {
  return os << "align=" << mem_arg.m_align << " offset=" << mem_arg.m_offset;
}
static std::ostream &operator<<(std::ostream &os, None const &none) { return os; }

std::ostream &operator<<(std::ostream &os, Instr const &instr) {
  os << instr.m_code;
  std::visit(
      [&os](auto const &arg) {
        if constexpr (!std::is_same_v<std::nullptr_t, std::remove_cvref_t<decltype(arg)>>) {
          os << " " << arg;
        }
      },
      instr.m_content);
  return os;
}

size_t Instr::get_operand_count() const {
  switch (m_code) {
  case InstrCode::I32_CONST:
  case InstrCode::I64_CONST:
  case InstrCode::F32_CONST:
  case InstrCode::F64_CONST:

  case InstrCode::LOCAL_GET:
  case InstrCode::GLOBAL_GET:
    return 0U;
  case InstrCode::I32_ADD:
  case InstrCode::I32_SUB:
  case InstrCode::I32_MUL:
  case InstrCode::I32_DIV_S:
  case InstrCode::I32_DIV_U:
  case InstrCode::I32_REM_S:
  case InstrCode::I32_REM_U:
  case InstrCode::I32_AND:
  case InstrCode::I32_OR:
  case InstrCode::I32_XOR:
  case InstrCode::I32_SHL:
  case InstrCode::I32_SHR_S:
  case InstrCode::I32_SHR_U:
    return 2U;

  case InstrCode::UNREACHABLE:
  case InstrCode::NOP:
  case InstrCode::BLOCK:
  case InstrCode::LOOP:
  case InstrCode::IF:
  case InstrCode::ELSE:
  case InstrCode::END:
  case InstrCode::BR:
  case InstrCode::BR_IF:
  case InstrCode::BR_TABLE:
  case InstrCode::RETURN:
  case InstrCode::CALL:
  case InstrCode::CALL_INDIRECT:
  case InstrCode::DROP:
  case InstrCode::SELECT:
  case InstrCode::LOCAL_SET:
  case InstrCode::LOCAL_TEE:
  case InstrCode::GLOBAL_SET:
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
  case InstrCode::I64_STORE32:
  case InstrCode::MEMORY_SIZE:
  case InstrCode::MEMORY_GROW:

  case InstrCode::I32_EQZ:
  case InstrCode::I32_EQ:
  case InstrCode::I32_NE:
  case InstrCode::I32_LT_S:
  case InstrCode::I32_LT_U:
  case InstrCode::I32_GT_S:
  case InstrCode::I32_GT_U:
  case InstrCode::I32_LE_S:
  case InstrCode::I32_LE_U:
  case InstrCode::I32_GE_S:
  case InstrCode::I32_GE_U:
  case InstrCode::I64_EQZ:
  case InstrCode::I64_EQ:
  case InstrCode::I64_NE:
  case InstrCode::I64_LT_S:
  case InstrCode::I64_LT_U:
  case InstrCode::I64_GT_S:
  case InstrCode::I64_GT_U:
  case InstrCode::I64_LE_S:
  case InstrCode::I64_LE_U:
  case InstrCode::I64_GE_S:
  case InstrCode::I64_GE_U:
  case InstrCode::F32_EQ:
  case InstrCode::F32_NE:
  case InstrCode::F32_LT:
  case InstrCode::F32_GT:
  case InstrCode::F32_LE:
  case InstrCode::F32_GE:
  case InstrCode::F64_EQ:
  case InstrCode::F64_NE:
  case InstrCode::F64_LT:
  case InstrCode::F64_GT:
  case InstrCode::F64_LE:
  case InstrCode::F64_GE:
  case InstrCode::I32_CLZ:
  case InstrCode::I32_CTZ:
  case InstrCode::I32_POPCNT:

  case InstrCode::I32_ROTL:
  case InstrCode::I32_ROTR:
  case InstrCode::I64_CLZ:
  case InstrCode::I64_CTZ:
  case InstrCode::I64_POPCNT:
  case InstrCode::I64_ADD:
  case InstrCode::I64_SUB:
  case InstrCode::I64_MUL:
  case InstrCode::I64_DIV_S:
  case InstrCode::I64_DIV_U:
  case InstrCode::I64_REM_S:
  case InstrCode::I64_REM_U:
  case InstrCode::I64_AND:
  case InstrCode::I64_OR:
  case InstrCode::I64_XOR:
  case InstrCode::I64_SHL:
  case InstrCode::I64_SHR_S:
  case InstrCode::I64_SHR_U:
  case InstrCode::I64_ROTL:
  case InstrCode::I64_ROTR:
  case InstrCode::F32_ABS:
  case InstrCode::F32_NEG:
  case InstrCode::F32_CEIL:
  case InstrCode::F32_FLOOR:
  case InstrCode::F32_TRUNC:
  case InstrCode::F32_NEAREST:
  case InstrCode::F32_SQRT:
  case InstrCode::F32_ADD:
  case InstrCode::F32_SUB:
  case InstrCode::F32_MUL:
  case InstrCode::F32_DIV:
  case InstrCode::F32_MIN:
  case InstrCode::F32_MAX:
  case InstrCode::F32_COPYSIGN:
  case InstrCode::F64_ABS:
  case InstrCode::F64_NEG:
  case InstrCode::F64_CEIL:
  case InstrCode::F64_FLOOR:
  case InstrCode::F64_TRUNC:
  case InstrCode::F64_NEAREST:
  case InstrCode::F64_SQRT:
  case InstrCode::F64_ADD:
  case InstrCode::F64_SUB:
  case InstrCode::F64_MUL:
  case InstrCode::F64_DIV:
  case InstrCode::F64_MIN:
  case InstrCode::F64_MAX:
  case InstrCode::F64_COPYSIGN:
  case InstrCode::I32_WRAP_I64:
  case InstrCode::I32_TRUNC_S_F32:
  case InstrCode::I32_TRUNC_U_F32:
  case InstrCode::I32_TRUNC_S_F64:
  case InstrCode::I32_TRUNC_U_F64:
  case InstrCode::I64_EXTEND_S_I32:
  case InstrCode::I64_EXTEND_U_I32:
  case InstrCode::I64_TRUNC_S_F32:
  case InstrCode::I64_TRUNC_U_F32:
  case InstrCode::I64_TRUNC_S_F64:
  case InstrCode::I64_TRUNC_U_F64:
  case InstrCode::F32_CONVERT_S_I32:
  case InstrCode::F32_CONVERT_U_I32:
  case InstrCode::F32_CONVERT_S_I64:
  case InstrCode::F32_CONVERT_U_I64:
  case InstrCode::F32_DEMOTE_F64:
  case InstrCode::F64_CONVERT_S_I32:
  case InstrCode::F64_CONVERT_U_I32:
  case InstrCode::F64_CONVERT_S_I64:
  case InstrCode::F64_CONVERT_U_I64:
  case InstrCode::F64_PROMOTE_F32:
  case InstrCode::I32_REINTERPRET_F32:
  case InstrCode::I64_REINTERPRET_F64:
  case InstrCode::F32_REINTERPRET_I32:
  case InstrCode::F64_REINTERPRET_I64:
  case InstrCode::I32_EXTEND8_S:
  case InstrCode::I32_EXTEND16_S:
  case InstrCode::I64_EXTEND8_S:
  case InstrCode::I64_EXTEND16_S:
  case InstrCode::I64_EXTEND32_S:
  case InstrCode::I32_TRUNC_SAT_F32_S:
  case InstrCode::I32_TRUNC_SAT_F32_U:
  case InstrCode::I32_TRUNC_SAT_F64_S:
  case InstrCode::I32_TRUNC_SAT_F64_U:
  case InstrCode::I64_TRUNC_SAT_F32_S:
  case InstrCode::I64_TRUNC_SAT_F32_U:
  case InstrCode::I64_TRUNC_SAT_F64_S:
  case InstrCode::I64_TRUNC_SAT_F64_U:
    throw Todo{__func__};
  }
}

size_t Instr::get_result_count() const {
  switch (m_code) {
  case InstrCode::I32_CONST:
  case InstrCode::I64_CONST:
  case InstrCode::F32_CONST:
  case InstrCode::F64_CONST:

  case InstrCode::GLOBAL_GET:
  case InstrCode::LOCAL_GET:

  case InstrCode::I32_ADD:
  case InstrCode::I32_SUB:
  case InstrCode::I32_MUL:
  case InstrCode::I32_DIV_S:
  case InstrCode::I32_DIV_U:
  case InstrCode::I32_REM_S:
  case InstrCode::I32_REM_U:
  case InstrCode::I32_AND:
  case InstrCode::I32_OR:
  case InstrCode::I32_XOR:
  case InstrCode::I32_SHL:
  case InstrCode::I32_SHR_S:
  case InstrCode::I32_SHR_U:
    return 1U;

  case InstrCode::UNREACHABLE:
  case InstrCode::NOP:
  case InstrCode::BLOCK:
  case InstrCode::LOOP:
  case InstrCode::IF:
  case InstrCode::ELSE:
  case InstrCode::END:
  case InstrCode::BR:
  case InstrCode::BR_IF:
  case InstrCode::BR_TABLE:
  case InstrCode::RETURN:
  case InstrCode::CALL:
  case InstrCode::CALL_INDIRECT:
  case InstrCode::DROP:
  case InstrCode::SELECT:
  case InstrCode::LOCAL_SET:
  case InstrCode::LOCAL_TEE:
  case InstrCode::GLOBAL_SET:
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
  case InstrCode::I64_STORE32:
  case InstrCode::MEMORY_SIZE:
  case InstrCode::MEMORY_GROW:

  case InstrCode::I32_EQZ:
  case InstrCode::I32_EQ:
  case InstrCode::I32_NE:
  case InstrCode::I32_LT_S:
  case InstrCode::I32_LT_U:
  case InstrCode::I32_GT_S:
  case InstrCode::I32_GT_U:
  case InstrCode::I32_LE_S:
  case InstrCode::I32_LE_U:
  case InstrCode::I32_GE_S:
  case InstrCode::I32_GE_U:
  case InstrCode::I64_EQZ:
  case InstrCode::I64_EQ:
  case InstrCode::I64_NE:
  case InstrCode::I64_LT_S:
  case InstrCode::I64_LT_U:
  case InstrCode::I64_GT_S:
  case InstrCode::I64_GT_U:
  case InstrCode::I64_LE_S:
  case InstrCode::I64_LE_U:
  case InstrCode::I64_GE_S:
  case InstrCode::I64_GE_U:
  case InstrCode::F32_EQ:
  case InstrCode::F32_NE:
  case InstrCode::F32_LT:
  case InstrCode::F32_GT:
  case InstrCode::F32_LE:
  case InstrCode::F32_GE:
  case InstrCode::F64_EQ:
  case InstrCode::F64_NE:
  case InstrCode::F64_LT:
  case InstrCode::F64_GT:
  case InstrCode::F64_LE:
  case InstrCode::F64_GE:
  case InstrCode::I32_CLZ:
  case InstrCode::I32_CTZ:
  case InstrCode::I32_POPCNT:

  case InstrCode::I32_ROTL:
  case InstrCode::I32_ROTR:
  case InstrCode::I64_CLZ:
  case InstrCode::I64_CTZ:
  case InstrCode::I64_POPCNT:
  case InstrCode::I64_ADD:
  case InstrCode::I64_SUB:
  case InstrCode::I64_MUL:
  case InstrCode::I64_DIV_S:
  case InstrCode::I64_DIV_U:
  case InstrCode::I64_REM_S:
  case InstrCode::I64_REM_U:
  case InstrCode::I64_AND:
  case InstrCode::I64_OR:
  case InstrCode::I64_XOR:
  case InstrCode::I64_SHL:
  case InstrCode::I64_SHR_S:
  case InstrCode::I64_SHR_U:
  case InstrCode::I64_ROTL:
  case InstrCode::I64_ROTR:
  case InstrCode::F32_ABS:
  case InstrCode::F32_NEG:
  case InstrCode::F32_CEIL:
  case InstrCode::F32_FLOOR:
  case InstrCode::F32_TRUNC:
  case InstrCode::F32_NEAREST:
  case InstrCode::F32_SQRT:
  case InstrCode::F32_ADD:
  case InstrCode::F32_SUB:
  case InstrCode::F32_MUL:
  case InstrCode::F32_DIV:
  case InstrCode::F32_MIN:
  case InstrCode::F32_MAX:
  case InstrCode::F32_COPYSIGN:
  case InstrCode::F64_ABS:
  case InstrCode::F64_NEG:
  case InstrCode::F64_CEIL:
  case InstrCode::F64_FLOOR:
  case InstrCode::F64_TRUNC:
  case InstrCode::F64_NEAREST:
  case InstrCode::F64_SQRT:
  case InstrCode::F64_ADD:
  case InstrCode::F64_SUB:
  case InstrCode::F64_MUL:
  case InstrCode::F64_DIV:
  case InstrCode::F64_MIN:
  case InstrCode::F64_MAX:
  case InstrCode::F64_COPYSIGN:
  case InstrCode::I32_WRAP_I64:
  case InstrCode::I32_TRUNC_S_F32:
  case InstrCode::I32_TRUNC_U_F32:
  case InstrCode::I32_TRUNC_S_F64:
  case InstrCode::I32_TRUNC_U_F64:
  case InstrCode::I64_EXTEND_S_I32:
  case InstrCode::I64_EXTEND_U_I32:
  case InstrCode::I64_TRUNC_S_F32:
  case InstrCode::I64_TRUNC_U_F32:
  case InstrCode::I64_TRUNC_S_F64:
  case InstrCode::I64_TRUNC_U_F64:
  case InstrCode::F32_CONVERT_S_I32:
  case InstrCode::F32_CONVERT_U_I32:
  case InstrCode::F32_CONVERT_S_I64:
  case InstrCode::F32_CONVERT_U_I64:
  case InstrCode::F32_DEMOTE_F64:
  case InstrCode::F64_CONVERT_S_I32:
  case InstrCode::F64_CONVERT_U_I32:
  case InstrCode::F64_CONVERT_S_I64:
  case InstrCode::F64_CONVERT_U_I64:
  case InstrCode::F64_PROMOTE_F32:
  case InstrCode::I32_REINTERPRET_F32:
  case InstrCode::I64_REINTERPRET_F64:
  case InstrCode::F32_REINTERPRET_I32:
  case InstrCode::F64_REINTERPRET_I64:
  case InstrCode::I32_EXTEND8_S:
  case InstrCode::I32_EXTEND16_S:
  case InstrCode::I64_EXTEND8_S:
  case InstrCode::I64_EXTEND16_S:
  case InstrCode::I64_EXTEND32_S:
  case InstrCode::I32_TRUNC_SAT_F32_S:
  case InstrCode::I32_TRUNC_SAT_F32_U:
  case InstrCode::I32_TRUNC_SAT_F64_S:
  case InstrCode::I32_TRUNC_SAT_F64_U:
  case InstrCode::I64_TRUNC_SAT_F32_S:
  case InstrCode::I64_TRUNC_SAT_F32_U:
  case InstrCode::I64_TRUNC_SAT_F64_S:
  case InstrCode::I64_TRUNC_SAT_F64_U:
    throw Todo{__func__};
  }
}

} // namespace wa
