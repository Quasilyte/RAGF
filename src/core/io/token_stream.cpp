#include <core/io/token_stream.hpp>

TokenStream::TokenStream(const byte* input):
$input{input},
$pos{input} {}

template<class T>
T TokenStream::read() noexcept {
  T result = *reinterpret_cast<T*>(const_cast<byte*>($pos));
  $pos += sizeof(T);
  return  result;
}

template<>
RegIndex TokenStream::read() noexcept {
  uint index = read_byte();
  if (index > 7) {
    throw "invalid register index";
  }

  return RegIndex{index};
}

byte TokenStream::read_byte() noexcept {
  return *($pos++);
}

Token TokenStream::read_token() noexcept {
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

