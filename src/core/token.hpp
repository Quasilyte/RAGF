#pragma once

#include <core/typedefs.hpp>

enum class Token: byte {
  //// Top level tokens
  // Special token
  END,

  // Operations
  RETURN,
  PUSH,
  POP,
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  BIT_AND,
  BIT_OR,
  SHIFT_LEFT,
  SHIFT_RIGHT,
  ASSIGN,
  SWAP,
  NEG,
  WHILE,
  IF,
  IF_ELSE,

  //// Context tokens
  NIL,
  DATA,
  COUNTER,

  // Operand types
  INT_REG,
  UINT_REG,
  MEM,
  INT,
  UINT,

  // Relation (binary) conditions
  EQ,
  NEQ,
  LT,
  LTE,
  GT,
  GTE,
};
