#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <ion/unicode/utf8_decoder.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare_junior/src/layout/layout_selection.h>
#include <poincare_junior/src/memory/reference.h>

namespace PoincareJ {

class Expression;

class Layout {
 public:
  constexpr static bool IsEmpty(const Tree* node) {
    assert(node->isRackLayout());
    return node->numberOfChildren() == 0;
  }

  static Tree* EditionPoolTextToLayout(const char* text);

 private:
  static void EditionPoolTextToLayoutRec(UTF8Decoder* decoder, Tree* parent,
                                         const Tree* parentheses);
};

class LayoutReference final : public Reference, public Layout {
  friend class Expression;

 public:
  LayoutReference(const Tree* tree) : Reference(tree) {
    assert(tree->isLayout());
  }
  using Reference::Reference;
  static LayoutReference Parse(const char* text);
  void draw(KDContext* ctx, KDPoint p, KDFont::Size font,
            KDColor expressionColor = KDColorBlack,
            KDColor backgroundColor = KDColorWhite,
            LayoutSelection selection = {}) const;
  KDSize size(KDFont::Size font) const;
  bool isEmpty() const { return IsEmpty(getTree()); }

  static LayoutReference FromExpression(const Expression* expr);
};

static_assert(sizeof(LayoutReference) == sizeof(Reference));

}  // namespace PoincareJ

#endif
