#include <ion/unicode/code_point.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/layout/layout_selection.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/rack_from_text.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>
#include <string.h>

namespace PoincareJ {

LayoutReference LayoutReference::Parse(const char *textInput) {
  return LayoutReference([](const char *text) { RackFromText(text); },
                         textInput);
}

LayoutReference LayoutReference::FromExpression(const Expression *expr) {
  return LayoutReference([](Tree *node) { Layoutter::LayoutExpression(node); },
                         expr);
}

void LayoutReference::draw(KDContext *ctx, KDPoint p, KDFont::Size font,
                           KDColor expressionColor, KDColor backgroundColor,
                           LayoutSelection selection) const {
  Render::Draw(getTree(), ctx, p, font, expressionColor, backgroundColor,
               nullptr);
}

KDSize LayoutReference::size(KDFont::Size font) const {
  return Render::Size(getTree(), font);
}

}  // namespace PoincareJ
