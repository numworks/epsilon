#include "subtraction.h"
#include <poincare_junior/src/memory/node.h>

namespace Poincare {

void Subtraction::BasicReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Subtraction);
  ProjectionReduction(block,
      []() { return Node::Push<BlockType::Addition>(2).block(); },
      []() { return Node::Push<BlockType::Multiplication>(2).block(); }
    );
}

}
