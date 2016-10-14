#include <core/arch/x86_64/code_writer.hpp>

#include <core/arch/x86_64/ops/ops.hpp>
#include <core/compile/compiler.hpp>
#include <core/codegen/numerics.hpp>

#define CodeWriter x86_64::CodeWriter

void CodeWriter::write_return() {
  Ret::write(&$output);
}

void CodeWriter::write_return(Reg r) {
  Mov::write(&$output, RAX, r);
  Ret::write(&$output);
}

void CodeWriter::write_push(const Reg* regs, int count) {
  auto q = regs + count;
  while (regs < q) {
    Push::write(&$output, *regs++);
  }
}

void CodeWriter::write_pop(const Reg* regs, int count) {
  auto q = regs + count;
  while (regs < q) {
    Pop::write(&$output, *--q);
  }
}

void CodeWriter::write_swap(Reg a, Reg b) {
  // It is faster than `xchg a, b`
  Mov::write(&$output, RAX, a);
  Mov::write(&$output, a, b);
  Mov::write(&$output, b, RAX);
}

void CodeWriter::write_swap(PtrReg a, PtrReg b) {
  Mov::write(&$output, RAX, a);
  Mov::write(&$output, RCX, b);
  Mov::write(&$output, a, RCX);
  Mov::write(&$output, b, RAX);
}

void CodeWriter::write_neg(Reg r) {
  Neg::write(&$output, r);
}

void CodeWriter::write_assign(Reg dst, Reg src) {
  Mov::write(&$output, dst, src);
}

void CodeWriter::write_assign(Reg dst, Imm src) {
  if (src == 0) {
    Xor::write(&$output, dst, dst);
  } else {
    Mov::write(&$output, dst, src);
  }
}

void CodeWriter::write_assign(Reg dst, PtrReg src) {
  Mov::write(&$output, dst, src);
}

void CodeWriter::write_assign(PtrReg dst, Reg src) {
  Mov::write(&$output, dst, src);
}

void CodeWriter::write_assign(Reg dst, DataReg) {
  Mov::write(&$output, dst, RDI);
}

void CodeWriter::write_add(Reg dst, Reg a, Imm b) {
  if (b == 0) return;

  if (dst == a) {
    Add::write(&$output, dst, b);
  } else {
    Lea::write(&$output, dst, a, b);
  }
}

void CodeWriter::write_add(Reg dst, Reg a, Reg b) {
  if (dst == a) {
    Add::write(&$output, dst, b);
  } else {
    Lea::write(&$output, dst, a, b);
  }
}

void CodeWriter::write_add(Reg dst, PtrReg src) {
  Add::write(&$output, dst, src);
}

void CodeWriter::write_sub(Reg dst, Reg a, Imm b) {
  if (b == 0) return;

  if (dst == a) {
    Sub::write(&$output, dst, b);
  } else {
    throw "unimplemented";
//    Mov::write(&$output, RAX, b);
//    Sub::write(&$output, RAX, b);
//    Mov::write(&$output, dst, RAX);
  }
}

void CodeWriter::write_sub(Reg dst, Reg a, Reg b) {
  if (dst == a) {
    Sub::write(&$output, dst, b);
  } else {
    Mov::write(&$output, RAX, b);
    Sub::write(&$output, RAX, b);
    Mov::write(&$output, dst, RAX);
  }
}

void CodeWriter::write_sub(Reg dst, PtrReg src) {
  Sub::write(&$output, dst, src);
}

void CodeWriter::write_mul(IntReg dst, Imm src) {
  Imul::write(&$output, dst, dst, src);
}

void CodeWriter::write_mul(IntReg dst, IntReg src) {
  Imul::write(&$output, dst, src);
}

void CodeWriter::write_div(IntReg dst, IntReg src) {
  Mov::write(&$output, RAX, Reg{dst});
  Cqo::write(&$output);
  Idiv::write(&$output, src);
  Mov::write(&$output, Reg{dst}, RAX);
}

void CodeWriter::write_mod(IntReg dst, IntReg src) {
  // Like div, but uses `rdx' idiv result, instead of `rax'
  Mov::write(&$output, RAX, Reg{dst});
  Cqo::write(&$output);
  Idiv::write(&$output, src);
  Mov::write(&$output, Reg{dst}, RDX);
}

void CodeWriter::write_bit_and(Reg dst, Reg src) {
  And::write(&$output, dst, src);
}

void CodeWriter::write_bit_or(Reg dst, Reg src) {
  Or::write(&$output, dst, src);
}

void CodeWriter::write_shift_left(UintReg r, u8 count) {
  Shl::write(&$output, Reg{r}, count);
}

void CodeWriter::write_shift_left(IntReg r, u8 count) {
  // Because `Sal` & `Shl` are the same
  write_shift_left(UintReg{r}, count);
}

void CodeWriter::write_shift_right(UintReg r, u8 count) {
  Shr::write(&$output, r, count);
}

void CodeWriter::write_shift_right(IntReg r, u8 count) {
  Sar::write(&$output, r, count);
}

void CodeWriter::write_while_neq(Reg a, Imm b) {
  auto jmp_block = $output.preserve(Jmp::size(i32{}));
  int body_size = write_block();
  Jmp::write(jmp_block, body_size);

  Cmp::write(&$output, a, b);
  Jne::write(&$output, -(body_size + Jne::size(i32{}) + Cmp::size(a, b)));
}

void CodeWriter::write_if_eq(Reg a, Imm b) {
  Cmp::write(&$output, a, b);

  auto jne_block = $output.preserve(Jne::size(i32{}));
  int on_true_size = write_block();
  Jne::write(jne_block, on_true_size);
}

void CodeWriter::write_if_else_eq(Reg a, Imm b) {
  Cmp::write(&$output, a, b);

  auto jne_block = $output.preserve(Jne::size(i32{}));
  int on_true_size = write_block();
  Jne::write(jne_block, on_true_size + Jmp::size(i32{}));

  auto jmp_block = $output.preserve(Jmp::size(i32{}));
  int on_false_size = write_block();
  Jmp::write(jmp_block, on_false_size);
}
