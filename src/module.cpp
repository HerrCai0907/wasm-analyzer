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
  case InstrCode::I32CONST:
    return os << "i32.const";
  case InstrCode::I64CONST:
    return os << "i64.const";
  case InstrCode::F32CONST:
    return os << "f32.const";
  case InstrCode::F64CONST:
    return os << "f64.const";

  // COMPARISON OPERATORS + INVERTED CMP OPCODE
  case InstrCode::I32EQZ:
    return os << "i32.eqz"; // Note: This is the actual opcode, not inverted.
  case InstrCode::I32EQ:
    return os << "i32.ne"; // Inverted opcode.
  case InstrCode::I32NE:
    return os << "i32.eq"; // Actual opcode.
  case InstrCode::I32LT_S:
    return os << "i32.ge_s"; // Inverted opcode.
  case InstrCode::I32LT_U:
    return os << "i32.ge_u"; // Inverted opcode.
  case InstrCode::I32GT_S:
    return os << "i32.le_s"; // Inverted opcode.
  case InstrCode::I32GT_U:
    return os << "i32.le_u"; // Inverted opcode.
  case InstrCode::I32LE_S:
    return os << "i32.gt_s"; // Inverted opcode.
  case InstrCode::I32LE_U:
    return os << "i32.gt_u"; // Inverted opcode.
  case InstrCode::I32GE_S:
    return os << "i32.lt_s"; // Inverted opcode.
  case InstrCode::I32GE_U:
    return os << "i32.lt_u"; // Inverted opcode.

  case InstrCode::I64EQZ:
    return os << "i64.eqz"; // Note: This is the actual opcode, not inverted.
  case InstrCode::I64EQ:
    return os << "i64.ne"; // Inverted opcode.
  case InstrCode::I64NE:
    return os << "i64.eq"; // Actual opcode.
  case InstrCode::I64LT_S:
    return os << "i64.ge_s"; // Inverted opcode.
  case InstrCode::I64LT_U:
    return os << "i64.ge_u"; // Inverted opcode.
  case InstrCode::I64GT_S:
    return os << "i64.le_s"; // Inverted opcode.
  case InstrCode::I64GT_U:
    return os << "i64.le_u"; // Inverted opcode.
  case InstrCode::I64LE_S:
    return os << "i64.gt_s"; // Inverted opcode.
  case InstrCode::I64LE_U:
    return os << "i64.gt_u"; // Inverted opcode.
  case InstrCode::I64GE_S:
    return os << "i64.lt_s"; // Inverted opcode.
  case InstrCode::I64GE_U:
    return os << "i64.lt_u"; // Inverted opcode.

  case InstrCode::F32EQ:
    return os << "f32.ne"; // Inverted opcode.
  case InstrCode::F32NE:
    return os << "f32.eq"; // Actual opcode.
  case InstrCode::F32LT:
    return os << "f32.ge"; // Inverted opcode.
  case InstrCode::F32GT:
    return os << "f32.le"; // Inverted opcode.
  case InstrCode::F32LE:
    return os << "f32.gt"; // Inverted opcode.
  case InstrCode::F32GE:
    return os << "f32.lt"; // Inverted opcode.

  case InstrCode::F64EQ:
    return os << "f64.ne"; // Inverted opcode.
  case InstrCode::F64NE:
    return os << "f64.eq"; // Actual opcode.
  case InstrCode::F64LT:
    return os << "f64.ge"; // Inverted opcode.
  case InstrCode::F64GT:
    return os << "f64.le"; // Inverted opcode.
  case InstrCode::F64LE:
    return os << "f64.gt"; // Inverted opcode.
  case InstrCode::F64GE:
    return os << "f64.lt"; // Inverted opcode.

  // NUMERIC OPERATORS
  case InstrCode::I32CLZ:
    return os << "i32.clz";
  case InstrCode::I32CTZ:
    return os << "i32.ctz";
  case InstrCode::I32POPCNT:
    return os << "i32.popcnt";
  case InstrCode::I32ADD:
    return os << "i32.add";
  case InstrCode::I32SUB:
    return os << "i32.sub";
  case InstrCode::I32MUL:
    return os << "i32.mul";
  case InstrCode::I32DIV_S:
    return os << "i32.div_s";
  case InstrCode::I32DIV_U:
    return os << "i32.div_u";
  case InstrCode::I32REM_S:
    return os << "i32.rem_s";
  case InstrCode::I32REM_U:
    return os << "i32.rem_u";
  case InstrCode::I32AND:
    return os << "i32.and";
  case InstrCode::I32OR:
    return os << "i32.or";
  case InstrCode::I32XOR:
    return os << "i32.xor";
  case InstrCode::I32SHL:
    return os << "i32.shl";
  case InstrCode::I32SHR_S:
    return os << "i32.shr_s";
  case InstrCode::I32SHR_U:
    return os << "i32.shr_u";
  case InstrCode::I32ROTL:
    return os << "i32.rotl";
  case InstrCode::I32ROTR:
    return os << "i32.rotr";

  case InstrCode::I64CLZ:
    return os << "i64.clz";
  case InstrCode::I64CTZ:
    return os << "i64.ctz";
  case InstrCode::I64POPCNT:
    return os << "i64.popcnt";
  case InstrCode::I64ADD:
    return os << "i64.add";
  case InstrCode::I64SUB:
    return os << "i64.sub";
  case InstrCode::I64MUL:
    return os << "i64.mul";
  case InstrCode::I64DIV_S:
    return os << "i64.div_s";
  case InstrCode::I64DIV_U:
    return os << "i64.div_u";
  case InstrCode::I64REM_S:
    return os << "i64.rem_s";
  case InstrCode::I64REM_U:
    return os << "i64.rem_u";
  case InstrCode::I64AND:
    return os << "i64.and";
  case InstrCode::I64OR:
    return os << "i64.or";
  case InstrCode::I64XOR:
    return os << "i64.xor";
  case InstrCode::I64SHL:
    return os << "i64.shl";
  case InstrCode::I64SHR_S:
    return os << "i64.shr_s";
  case InstrCode::I64SHR_U:
    return os << "i64.shr_u";
  case InstrCode::I64ROTL:
    return os << "i64.rotl";
  case InstrCode::I64ROTR:
    return os << "i64.rotr";

  // Floating point arithmetic
  case InstrCode::F32ABS:
    return os << "f32.abs";
  case InstrCode::F32NEG:
    return os << "f32.neg";
  case InstrCode::F32CEIL:
    return os << "f32.ceil";
  case InstrCode::F32FLOOR:
    return os << "f32.floor";
  case InstrCode::F32TRUNC:
    return os << "f32.trunc";
  case InstrCode::F32NEAREST:
    return os << "f32.nearest";
  case InstrCode::F32SQRT:
    return os << "f32.sqrt";
  case InstrCode::F32ADD:
    return os << "f32.add";
  case InstrCode::F32SUB:
    return os << "f32.sub";
  case InstrCode::F32MUL:
    return os << "f32.mul";
  case InstrCode::F32DIV:
    return os << "f32.div";
  case InstrCode::F32MIN:
    return os << "f32.min";
  case InstrCode::F32MAX:
    return os << "f32.max";
  case InstrCode::F32COPYSIGN:
    return os << "f32.copysign";

  case InstrCode::F64ABS:
    return os << "f64.abs";
  case InstrCode::F64NEG:
    return os << "f64.neg";
  case InstrCode::F64CEIL:
    return os << "f64.ceil";
  case InstrCode::F64FLOOR:
    return os << "f64.floor";
  case InstrCode::F64TRUNC:
    return os << "f64.trunc";
  case InstrCode::F64NEAREST:
    return os << "f64.nearest";
  case InstrCode::F64SQRT:
    return os << "f64.sqrt";
  case InstrCode::F64ADD:
    return os << "f64.add";
  case InstrCode::F64SUB:
    return os << "f64.sub";
  case InstrCode::F64MUL:
    return os << "f64.mul";
  case InstrCode::F64DIV:
    return os << "f64.div";
  case InstrCode::F64MIN:
    return os << "f64.min";
  case InstrCode::F64MAX:
    return os << "f64.max";
  case InstrCode::F64COPYSIGN:
    return os << "f64.copysign";

  // CONVERSIONS
  case InstrCode::I32WRAP_I64:
    return os << "i32.wrap/i64";
  case InstrCode::I32TRUNC_S_F32:
    return os << "i32.trunc_s/f32";
  case InstrCode::I32TRUNC_U_F32:
    return os << "i32.trunc_u/f32";
  case InstrCode::I32TRUNC_S_F64:
    return os << "i32.trunc_s/f64";
  case InstrCode::I32TRUNC_U_F64:
    return os << "i32.trunc_u/f64";

  case InstrCode::I64EXTEND_S_I32:
    return os << "i64.extend_s/i32";
  case InstrCode::I64EXTEND_U_I32:
    return os << "i64.extend_u/i32";
  case InstrCode::I64TRUNC_S_F32:
    return os << "i64.trunc_s/f32";
  case InstrCode::I64TRUNC_U_F32:
    return os << "i64.trunc_u/f32";
  case InstrCode::I64TRUNC_S_F64:
    return os << "i64.trunc_s/f64";
  case InstrCode::I64TRUNC_U_F64:
    return os << "i64.trunc_u/f64";

  case InstrCode::F32CONVERT_S_I32:
    return os << "f32.convert_s/i32";
  case InstrCode::F32CONVERT_U_I32:
    return os << "f32.convert_u/i32";
  case InstrCode::F32CONVERT_S_I64:
    return os << "f32.convert_s/i64";
  case InstrCode::F32CONVERT_U_I64:
    return os << "f32.convert_u/i64";
  case InstrCode::F32DEMOTE_F64:
    return os << "f32.demote/f64";

  case InstrCode::F64CONVERT_S_I32:
    return os << "f64.convert_s/i32";
  case InstrCode::F64CONVERT_U_I32:
    return os << "f64.convert_u/i32";
  case InstrCode::F64CONVERT_S_I64:
    return os << "f64.convert_s/i64";
  case InstrCode::F64CONVERT_U_I64:
    return os << "f64.convert_u/i64";
  case InstrCode::F64PROMOTE_F32:
    return os << "f64.promote/f32";

  // REINTERPRETATIONS
  case InstrCode::I32REINTERPRET_F32:
    return os << "i32.reinterpret/f32";
  case InstrCode::I64REINTERPRET_F64:
    return os << "i64.reinterpret/f64";
  case InstrCode::F32REINTERPRET_I32:
    return os << "f32.reinterpret/i32";
  case InstrCode::F64REINTERPRET_I64:
    return os << "f64.reinterpret/i64";

  // SIGN EXTENSIONS
  case InstrCode::I32EXTEND8_S:
    return os << "i32.extend8_s";
  case InstrCode::I32EXTEND16_S:
    return os << "i32.extend16_s";
  case InstrCode::I64EXTEND8_S:
    return os << "i64.extend8_s";
  case InstrCode::I64EXTEND16_S:
    return os << "i64.extend16_s";
  case InstrCode::I64EXTEND32_S:
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

} // namespace wa
