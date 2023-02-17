#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <poincare_junior/src/memory/reference.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <kandinsky/context.h>

namespace PoincareJ {

class Layout final : public Reference {
friend class Expression;
public:
  Layout(const Node tree) : Reference(tree) { assert(tree.block()->isLayout()); }
  using Reference::Reference;
  static Layout Parse(const char * text);
  void toText(char * buffer, size_t bufferSize) const;
  void draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) const;
  KDSize size(KDFont::Size font) const;
private:
  static EditionReference EditionPoolTextToLayout(const char * text);
};

static_assert(sizeof(Layout) == sizeof(Reference));

}

#endif
