#include "integer.h"
#include "polynomial.h"
#include "set.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

namespace Poincare {

EditionReference Polynomial::GetVariables(const Node expression) {
  if (expression.block()->isNumber()) {
    return EditionReference(Set());
  }
  BlockType type = expression.type();
  // TODO: match
  if (type == BlockType::Power) {
    Node base = expression.nextNode();
    Node exponant = base.nextTree();
    if (Integer::IsUint8(exponant)) {
      assert(Integer::Uint8(exponant) > 1);
      EditionReference variables = EditionReference::Push<BlockType::Set>(1);
      EditionReference::Clone(base);
      return variables;
    }
  }
  if (type == BlockType::Addition || type == BlockType::Multiplication) {
    EditionReference variables = EditionReference(Set());
    for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(expression)) {
      Node child = std::get<Node>(indexedNode);
      if (child.type() == BlockType::Addition) {
        variables = Set::Add(variables, child);
      } else {
        variables = Set::Union(variables, GetVariables(child));
      }
    }
    return variables;
  }
  EditionReference variables = EditionReference::Push<BlockType::Set>(1);
  EditionReference::Clone(expression);
  return variables;
}

uint8_t Polynomial::Degree(const Node expression, const Node variables) {
  if (Set::Includes(variables, expression)) {
    return 1;
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Node base = expression.nextNode();
    Node exponant = base.nextTree();
    if (Integer::IsUint8(exponant) && Set::Includes(variables, base)) {
      return Integer::Uint8(exponant);
    }
  }
  uint8_t degree = 0;
  if (type == BlockType::Addition || type == BlockType::Multiplication) {
    for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(expression)) {
      Node child = std::get<Node>(indexedNode);
      uint8_t childDegree = Degree(child, variables);
      if (type == BlockType::Addition) {
        degree = std::max(degree, childDegree);
      } else {
        degree += childDegree;
      }
    }
  }
  // TODO assert(!Number::IsZero(expression));
  return degree;
}

}
