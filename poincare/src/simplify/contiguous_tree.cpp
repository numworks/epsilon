#include "contiguous_tree.h"
extern "C" {
#include <assert.h>
}

ContiguousTree * ContiguousTree::child(uint8_t index) {
  assert(index>=0 && index<m_numberOfChildren);
  if (index == 0) {
    return (this+1); // Pointer arithmetics
  } else {
    ContiguousTree * previousChild = this->child(index-1);
    return previousChild+previousChild->m_numberOfChildren+1; // Pointer arithm.
  }
}
