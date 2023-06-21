#ifndef POINCARE_EXPRESSION_P_PUSHER_H
#define POINCARE_EXPRESSION_P_PUSHER_H

#include <poincare_junior/src/memory/p_pusher.h>

#define P_RACKL(...) _NARY_PUSHER(BlockType::RackLayout, __VA_ARGS__)

#define P_VERTOFFL(...) \
  _UNARY_PUSHER(BlockType::VerticalOffsetLayout, __VA_ARGS__)

// Wrap arguments with EditionReference::Clone if they have Node* type to avoid
// mistakes ?

#endif
