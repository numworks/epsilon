#include "division.h"
#include "expression.h"
#include "../node.h"

namespace Poincare {

void Division::BasicReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Subtraction);
  Expression::ProjectionReduction(block,
      []() { return Node::Push<Multiplication>(2).block(); },
      []() { return Node::Push<Power>().block(); }
    );
}
}

