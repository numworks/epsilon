#ifndef POINCARE_SIMPLIFY_CONTIGUOUS_TREE_H
#define POINCARE_SIMPLIFY_CONTIGUOUS_TREE_H

extern "C" {
#include <stdint.h>
}

class ContiguousTree {
public:
  constexpr ContiguousTree(uint8_t numberOfChildren);
  ContiguousTree * child(uint8_t i);
protected:
  uint8_t m_numberOfChildren;
};

constexpr ContiguousTree::ContiguousTree(uint8_t numberOfChildren) :
  m_numberOfChildren(numberOfChildren) {
}

#endif
