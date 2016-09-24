#include <core/io/token_stream.hpp>

TokenStream::TokenStream(const byte* input):
$pos{input} {}

template<class T>
T TokenStream::read() {
  T result = *reinterpret_cast<T*>(const_cast<byte*>($pos));
  $pos += sizeof(T);
  return  result;
}

template<>
Reg TokenStream::read() {
  uint id = read_byte();
  if (id > 7) {
    throw "invalid register id";
  }

  return Reg{id};
}

template<>
IntReg TokenStream::read() { return IntReg{read<Reg>()}; }

template<>
UintReg TokenStream::read() { return UintReg{read<Reg>()}; }

template<>
Mem64 TokenStream::read() { return Mem64{read<Reg>()}; }

template<>
i8 TokenStream::read() { return read_byte(); }

template<>
u8 TokenStream::read() { return read_byte(); }

byte TokenStream::read_byte() noexcept {
  return *($pos++);
}

template<>
Token TokenStream::read() {
  return static_cast<Token>(read_byte());
}

const byte* TokenStream::read_bytes(int count) noexcept {
  const byte* bytes = $pos;
  $pos += count;
  return bytes;
}

template i32 TokenStream::read<i32>();
template i64 TokenStream::read<i64>();
template u16 TokenStream::read<u16>();
template u32 TokenStream::read<u32>();

