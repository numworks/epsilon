#include <poincare/layout.h>
#include <poincare/old_expression.h>
#include <poincare/symbol_abstract.h>
#include <poincare_junior/src/layout/rack_layout.h>

namespace Poincare {

#define Layout OLayout

KDSize Layout::layoutSize(KDFont::Size font,
                          PoincareJ::LayoutCursor *cursor) const {
  return (*this)->layoutSize(font, cursor);
}

KDCoordinate Layout::baseline(KDFont::Size font,
                              PoincareJ::LayoutCursor *cursor) const {
  PoincareJ::RackLayout::s_layoutCursor = cursor;
  return (*this)->baseline(font, cursor);
}

Layout Layout::clone() const {
  if (isUninitialized()) {
    return Layout();
  }
  TreeHandle c = TreeHandle::clone();
  Layout cast = Layout(static_cast<LayoutNode *>(c.node()));
  cast->invalidAllSizesPositionsAndBaselines();
  return cast;
}

size_t Layout::serializeParsedExpression(char *buffer, size_t bufferSize,
                                         Context *context) const {
  /* This method fixes the following problem:
   * Some layouts have a special serialization so they can be parsed afterwards,
   * such has logBase3(2) that serializes as log_{3}(2). When handling the
   * layout text, we want to use log(2,3) because we might paste the text in a
   * LinearEdition textfield, so what we really want is the parsed expression's
   * serialization. */
  if (bufferSize <= 0) {
    return 0;
  }
  serializeForParsing(buffer, bufferSize);
  Poincare::OExpression e = Poincare::OExpression::Parse(buffer, context);
  if (e.isUninitialized()) {
    buffer[0] = 0;
    return 0;
  }
  return e.serialize(buffer, bufferSize,
                     Poincare::Preferences::SharedPreferences()->displayMode());
}

}  // namespace Poincare
