#ifndef POINCARE_LAYOUT_VERTICAL_OFFSET_H
#define POINCARE_LAYOUT_VERTICAL_OFFSET_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
namespace VerticalOffset {
inline bool IsSubscript(const Tree* l) {
  return l->toVerticalOffsetLayoutNode()->isSubscript;
}

inline bool IsSuperscript(const Tree* l) { return !IsSubscript(l); }

inline bool IsPrefix(const Tree* l) {
  return l->toVerticalOffsetLayoutNode()->isPrefix;
}

inline bool IsSuffix(const Tree* l) { return !IsPrefix(l); }

inline void SetSuperscript(Tree* l, bool superscript) {
  l->toVerticalOffsetLayoutNode()->isSubscript = !superscript;
}

inline void SetSuffix(Tree* l, bool suffix) {
  l->toVerticalOffsetLayoutNode()->isPrefix = !suffix;
}

inline bool IsSuffixSuperscript(const Tree* l) {
  assert(l->isVerticalOffsetLayout());
  return IsSuffix(l) && IsSuperscript(l);
}
}  // namespace VerticalOffset

}  // namespace Poincare::Internal

#endif
