#include "division.h"
#include <poincare_junior/src/expression/expression.h>
#include <poincare_junior/src/memory/node.h>

namespace Poincare {

void Division::BasicReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Division);
  EExpression::ProjectionReduction(block,
      []() { return Node::Push<Multiplication>(2).block(); },
      []() { return Node::Push<Power>().block(); }
    );
}
}

