#ifndef POINCARE_EXPRESSION_P_PUSHER_H
#define POINCARE_EXPRESSION_P_PUSHER_H

#include <poincare_junior/src/memory/p_pusher.h>

#define P_ADD(...) _NARY_PUSHER(BlockType::Addition, __VA_ARGS__)

#define P_MULT(...) _NARY_PUSHER(BlockType::Multiplication, __VA_ARGS__)

#define P_POW(A, B) _BINARY_PUSHER(BlockType::Power, A, B)

// Wrap arguments with EditionReference::Clone if they have Node type to avoid mistakes ?

#endif
