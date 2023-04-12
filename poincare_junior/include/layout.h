#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/reference.h>

namespace PoincareJ {

class Layout final : public Reference {
  friend class Expression;

 public:
  constexpr static bool IsHorizontal(const Node node) {
    return node.type() == BlockType::RackLayout;
  }
  constexpr static bool IsEmpty(const Node node) {
    return IsHorizontal(node) && node.numberOfChildren() == 0;
  }

  Layout(const Node tree) : Reference(tree) {
    assert(tree.block()->isLayout());
  }
  using Reference::Reference;
  static Layout Parse(const char* text);
  size_t toText(char* buffer, size_t bufferSize) const;
  void draw(KDContext* ctx, KDPoint p, KDFont::Size font,
            KDColor expressionColor = KDColorBlack,
            KDColor backgroundColor = KDColorWhite) const;
  KDSize size(KDFont::Size font) const;
  bool isEmpty() const;

 private:
  static EditionReference EditionPoolTextToLayout(const char* text);
};

static_assert(sizeof(Layout) == sizeof(Reference));

}  // namespace PoincareJ

#endif
