#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <escher/palette.h>
#include <omg/directions.h>
#include <poincare/context.h>
#include <poincare/layout_node.h>
#include <poincare/tree_handle.h>
#include <poincare/trinary_boolean.h>

namespace PoincareJ {
class LayoutCursor;
}

namespace Poincare {

class OLayout : public TreeHandle {
 public:
  OLayout() : TreeHandle() {}
  OLayout(const LayoutNode *node) : TreeHandle(node) {}
  OLayout clone() const;
  LayoutNode *node() const {
    assert(isUninitialized() ||
           (TreeHandle::node() && !TreeHandle::node()->isGhost()));
    return static_cast<LayoutNode *>(TreeHandle::node());
  }
  static OLayout LayoutFromAddress(const void *address, size_t size);

  // True if horizontal layout with only code points in it
  bool isCodePointsString() const;
  bool isIdenticalTo(OLayout l, bool makeEditable = false) {
    return isUninitialized() ? l.isUninitialized()
                             : node()->isIdenticalTo(l, makeEditable);
  }

  // Rendering
  void render(KDContext *ctx, KDPoint p, KDGlyph::Style style) {
    return node()->render(ctx, p, style);
  }
  KDSize layoutSize(KDFont::Size font,
                    PoincareJ::LayoutCursor *cursor = nullptr) const;
  KDPoint absoluteOrigin(KDFont::Size font) const {
    return node()->absoluteOrigin(font);
  }
  KDCoordinate baseline(KDFont::Size font,
                        PoincareJ::LayoutCursor *cursor = nullptr);
  void invalidAllSizesPositionsAndBaselines() {
    return node()->invalidAllSizesPositionsAndBaselines();
  }

  // Serialization
  size_t serializeForParsing(char *buffer, size_t bufferSize) const {
    return node()->serialize(buffer, bufferSize);
  }
  size_t serializeParsedExpression(char *buffer, size_t bufferSize,
                                   Context *context) const;

  // Layout properties
  /* Return True if the layout succeeded the test, Unknown if its children
   * could succeed, and False if the recursion should stop. */
  typedef TrinaryBoolean (*LayoutTest)(const OLayout l);
  OLayout recursivelyMatches(LayoutTest test) const;

  // Tree
  OLayout childAtIndex(int i) const;
  OLayout root() const {
    assert(!isUninitialized());
    return OLayout(node()->root());
  }
  OLayout parent() const {
    assert(!isUninitialized());
    return OLayout(node()->parent());
  }
  // Replace strings with codepoints
  OLayout makeEditable() { return node()->makeEditable(); }
 private:
  bool privateHasTopLevelComparisonSymbol(bool includingNotEqualSymbol) const;
};

}  // namespace Poincare

#endif

// TODO remove this hack
#include "junior_layout.h"
