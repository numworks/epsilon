#pragma once

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
namespace SequenceLayout {
inline uint8_t FirstRank(const Tree* l) {
  return l->toSequenceLayoutNode()->firstRank;
}

inline void SetFirstRank(Tree* l, uint8_t firstRank) {
  l->toSequenceLayoutNode()->firstRank = firstRank;
}

}  // namespace SequenceLayout

}  // namespace Poincare::Internal
