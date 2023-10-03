#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <ion/unicode/utf8_decoder.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/reference.h>

namespace PoincareJ {

class Expression;

class Layout final : public Reference {
  friend class Expression;

 public:
  constexpr static bool IsHorizontal(const Tree* node) {
    return node->layoutType() == LayoutType::Rack;
  }
  constexpr static bool IsEmpty(const Tree* node) {
    return IsHorizontal(node) && node->numberOfChildren() == 0;
  }

  Layout(const Tree* tree) : Reference(tree) {
    assert(tree->type().isLayout());
  }
  using Reference::Reference;
  static Layout Parse(const char* text);
  void draw(KDContext* ctx, KDPoint p, KDFont::Size font,
            KDColor expressionColor = KDColorBlack,
            KDColor backgroundColor = KDColorWhite) const;
  KDSize size(KDFont::Size font) const;
  bool isEmpty() const;

  static char* Serialize(const Tree* layout, char* buffer, char* end);
  static Layout FromExpression(const Expression* expr);

  static Tree* EditionPoolTextToLayout(const char* text);

 private:
  static void EditionPoolTextToLayoutRec(UTF8Decoder* decoder, Tree* parent,
                                         const Tree* parentheses);
};

static_assert(sizeof(Layout) == sizeof(Reference));

}  // namespace PoincareJ

#endif
