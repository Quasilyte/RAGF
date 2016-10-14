#include <core/compile/compiler.hpp>

#include <core/codegen/code_writer.hpp>
#include <core/compile/parsers_macro.hpp>
#include <core/compile/helpers.hpp>

Compiler::Compiler(TokenStream& tokens):
$input{tokens} {}

void Compiler::set_writer(CodeWriter* writer) {
  $writer = writer;
}

void Compiler::parse_return() {
  switch ($input.read<Token>()) {
  case T1(NIL):
    return $writer->write_return();
  case T1(REG):
    return $writer->write_return($input.read<Reg>());

  default:
    throw "return: invalid arg token";
  }
}

void Compiler::parse_swap() {
  switch ($input.read<u16>()) {
  case T2(REG, REG):
    return encode<Reg, Reg>(&CodeWriter::write_swap);
  case T2(MEM, MEM):
    return encode<PtrReg, PtrReg>(&CodeWriter::write_swap);
  }
}

void Compiler::parse_neg() {
  $writer->write_neg($input.read<Reg>());
}

void Compiler::parse_add() {
  switch ($input.read<u32>()) {
  case T4(REG, REG, INT, NIL):
    return encode<Reg, Reg, i64>(&CodeWriter::write_add);
  case T4(REG, REG, REG, NIL):
    return encode<Reg, Reg, Reg>(&CodeWriter::write_add);
  case T4(REG, MEM, NIL, NIL): // #FIXME
    return encode<Reg, PtrReg>(&CodeWriter::write_add);

  default:
    throw "add: invalid dst/src token";
  }
}

void Compiler::parse_sub() {
  switch ($input.read<u32>()) {
  case T4(REG, REG, INT, NIL):
    return encode<Reg, Reg, i64>(&CodeWriter::write_sub);
  case T4(REG, REG, REG, NIL):
    return encode<Reg, Reg, Reg>(&CodeWriter::write_sub);
  case T4(REG, MEM, NIL, NIL): // #FIXME
    return encode<Reg, PtrReg>(&CodeWriter::write_sub);

  default:
    throw "sub: invalid dst/src token";
  }
}

void Compiler::parse_mul() {
  switch ($input.read<u16>()) {
  case T2(INT_REG, INT):
    return encode<IntReg, Imm>(&CodeWriter::write_mul);
  case T2(INT_REG, INT_REG):
    return encode<IntReg, IntReg>(&CodeWriter::write_mul);

  default:
    throw "mul: invalid dst/src token";
  }
}

void Compiler::parse_div() {
  switch ($input.read<u16>()) {
  case T2(INT_REG, INT_REG):
    return encode<IntReg, IntReg>(&CodeWriter::write_div);

  default:
    throw "div: invalid dst/src token";
  }
}

void Compiler::parse_mod() {
  switch ($input.read<u16>()) {
  case T2(INT_REG, INT_REG):
    return encode<IntReg, IntReg>(&CodeWriter::write_mod);

  default:
    throw "mod: invalid dst/src token";
  }
}

void Compiler::parse_bit_and() {
  switch ($input.read<u16>()) {
  case T2(REG, REG):
    return encode<Reg, Reg>(&CodeWriter::write_bit_and);

  default:
    throw "bit_and: invalid dst/src token";
  }
}

void Compiler::parse_bit_or() {
  switch ($input.read<u16>()) {
  case T2(REG, REG):
    return encode<Reg, Reg>(&CodeWriter::write_bit_or);

  default:
    throw "bit_or: invalid dst/src token";
  }
}

void Compiler::parse_shift_left() {
  switch ($input.read<Token>()) {
  case T1(INT_REG):
    return encode<IntReg, u8>(&CodeWriter::write_shift_left);
  case T1(UINT_REG):
    return encode<UintReg, u8>(&CodeWriter::write_shift_left);

  default:
    throw "shift_left: invalid dst/src token";
  }
}

void Compiler::parse_shift_right() {
  switch ($input.read<Token>()) {
  case T1(INT_REG):
    return encode<IntReg, u8>(&CodeWriter::write_shift_right);
  case T1(UINT_REG):
    return encode<UintReg, u8>(&CodeWriter::write_shift_right);

  default:
    throw "shift_right: invalid dst/src token";
  }
}

void Compiler::parse_assign() {
  switch ($input.read<u16>()) {
  case T2(REG, REG):
    return encode<Reg, Reg>(&CodeWriter::write_assign);

  case T2(INT_REG, INT):
    return encode<Reg, i64>(&CodeWriter::write_assign);

  case T2(INT_REG, MEM):
    return encode<Reg, PtrReg>(&CodeWriter::write_assign);

  case T2(MEM, INT_REG):
    return encode<PtrReg, Reg>(&CodeWriter::write_assign);

  case T2(REG, DATA):
    return $writer->write_assign($input.read<Reg>(), DataReg{});

  default:
    throw "assign: invalid dst/src token";
  }
}

void Compiler::parse_while() {
  switch ($input.read<u32>()) {
  case T4(NEQ, REG, INT, NIL):
    return encode<Reg, Imm>(&CodeWriter::write_while_neq);

  default:
    throw "while: invalid header tokens";
  }
}

void Compiler::parse_if() {
  switch ($input.read<u32>()) {
  case T4(EQ, REG, INT, NIL):
    return encode<Reg, Imm>(&CodeWriter::write_if_eq);

  default:
    throw "if: invalid header tokens";
  }
}

void Compiler::parse_push() {
  int count = $input.read<u8>();
  if (count > 8) {
    throw "push: count token is too high (8 is max)";
  }

  auto bytes = $input.read_bytes(count);
  auto regs = reinterpret_cast<const Reg*>(bytes);
  $writer->write_push(regs, count);
}

void Compiler::parse_pop() {
  int count = $input.read<u8>();
  if (count > 8) {
    throw "pop: count token is too high (8 is max)";
  }

  auto bytes = $input.read_bytes(count);
  auto regs = reinterpret_cast<const Reg*>(bytes);
  $writer->write_pop(regs, count);
}

void Compiler::parse_if_else() {
  switch ($input.read<u32>()) {
  case T4(EQ, REG, INT, NIL):
    return encode<Reg, Imm>(&CodeWriter::write_if_else_eq);

  default:
    throw "if/else: invalid header tokens";
  }
}

Buf Compiler::compile() {
  BEGIN_PARSERS;
    TERMINATING_PARSER(end_of_input);
    PARSER(return);
    PARSER(push);
    PARSER(pop);
    PARSER(add);
    PARSER(sub);
    PARSER(mul);
    PARSER(div);
    PARSER(mod);
    PARSER(bit_and);
    PARSER(bit_or);
    PARSER(shift_left);
    PARSER(shift_right);
    PARSER(assign);
    PARSER(swap);
    PARSER(neg);
    PARSER(while);
    PARSER(if);
    PARSER(if_else);
  END_PARSERS;
}

Buf Compiler::get_result() {
  return $writer->get_buf();
}
