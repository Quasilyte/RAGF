#include <core/arch/x86_64/ops/ops.hpp>

#include <core/io/code_buf.hpp>
#include <core/arch/x86_64/encode.hpp>
#include <core/codegen/numerics.hpp>

template<>
void Sub::write(CodeBuf* output, Reg dst, i64 src) {
  if (fits_i8(src)) {
    *output << BinaryValue<4>{
      REX_WB,
      opcode(0x83),
      mod_reg_rm(Mod::REG, 5, dst),
      (i8)src
    };
  }
  else if (fits_i32(src)) {
    *output << BinaryValue<8>{
      REX_WB,
      opcode(0x81),
      mod_reg_rm(Mod::REG, 5, dst),
      (i32)src
    };
  }
  else {
    throw "sub: no support for int64 immediates";
  }
}

template<>
void Sub::write(CodeBuf* output, Reg dst, Reg src) {
  *output << BinaryValue<4>{
    REX_WRB,
    opcode(0x29),
    mod_reg_rm(Mod::REG, src, dst),
  };
}

template<>
void Sub::write(CodeBuf* output, Reg dst, PtrReg src) {
  if (src.obj_size == 8) {
    if (src.disp == 0) {
      *output << BinaryValue<4>{
        REX_WRB,
        opcode(0x2B),
        mod_reg_rm(Mod::SIB, dst, src.ptr)
      };
    }
    else {
      throw "sub: invalid displacement";
    }
  }
  else {
    throw "sub: invalid memory cell size";
  }
}

template<>
void Sub::write(CodeBuf* output, Gpr dst, Reg src) {
  *output << BinaryValue<4>{
    REX_WR,
    opcode(0x29),
    mod_reg_rm(Mod::REG, src, dst)
  };
}
