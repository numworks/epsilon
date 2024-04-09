#ifndef POINCARE_LAYOUT_VERTICAL_OFFSET_H
#define POINCARE_LAYOUT_VERTICAL_OFFSET_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
namespace VerticalOffset {
inline bool IsSuperscript(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return !node->nodeValueBlock(0)->getBit(0);
}

inline bool IsSubscript(const Tree* node) { return !IsSuperscript(node); }

inline bool IsSuffix(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return !node->nodeValueBlock(0)->getBit(1);
}

inline bool IsPrefix(const Tree* node) { return !IsSuffix(node); }

inline void SetSuperscript(Tree* node, bool superscript) {
  assert(node->isVerticalOffsetLayout());
  node->nodeValueBlock(0)->setBit(0, !superscript);
}

inline void SetSuffix(Tree* node, bool suffix) {
  assert(node->isVerticalOffsetLayout());
  node->nodeValueBlock(0)->setBit(1, !suffix);
}

inline bool IsSuffixSuperscript(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return IsSuffix(node) && IsSuperscript(node);
}
}  // namespace VerticalOffset

}  // namespace Poincare::Internal

#endif
