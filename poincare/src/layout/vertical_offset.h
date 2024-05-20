#ifndef POINCARE_LAYOUT_VERTICAL_OFFSET_H
#define POINCARE_LAYOUT_VERTICAL_OFFSET_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
namespace VerticalOffset {
inline bool IsSubscript(const Tree* node) {
  return node->toVerticalOffsetLayoutNode()->isSubscript;
}

inline bool IsSuperscript(const Tree* node) { return !IsSubscript(node); }

inline bool IsPrefix(const Tree* node) {
  return node->toVerticalOffsetLayoutNode()->isPrefix;
}

inline bool IsSuffix(const Tree* node) { return !IsPrefix(node); }

inline void SetSuperscript(Tree* node, bool superscript) {
  node->toVerticalOffsetLayoutNode()->isSubscript = !superscript;
}

inline void SetSuffix(Tree* node, bool suffix) {
  node->toVerticalOffsetLayoutNode()->isPrefix = !suffix;
}

inline bool IsSuffixSuperscript(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return IsSuffix(node) && IsSuperscript(node);
}
}  // namespace VerticalOffset

}  // namespace Poincare::Internal

#endif
