#ifndef POINCARE_EXPRESSION_P_PUSHER_H
#define POINCARE_EXPRESSION_P_PUSHER_H

#include <poincare_junior/src/memory/p_pusher.h>

#define P_ADD(...) _NARY_PUSHER(BlockType::Addition, __VA_ARGS__)

#define P_MULT(...) _NARY_PUSHER(BlockType::Multiplication, __VA_ARGS__)

#define P_POW(A, B) _BINARY_PUSHER(BlockType::Power, A, B)

#define P_CLONE(A) editionPool->clone(A)

#define P_ZERO() editionPool->clone(0_e)

#define P_ONE() editionPool->clone(1_e)

#define P_UNDEF() editionPool->clone(KUndef)

// Wrap arguments with EditionReference::Clone if they have Tree* type to avoid
// mistakes ?

#endif
