#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_selection.h>
#include <poincare/square_bracket_pair_layout.h>
#include <poincare/symbol_abstract.h>

namespace Poincare {

Layout Layout::clone() const {
  if (isUninitialized()) {
    return Layout();
  }
  TreeHandle c = TreeHandle::clone();
  Layout cast = Layout(static_cast<LayoutNode *>(c.node()));
  cast.invalidAllSizesPositionsAndBaselines();
  return cast;
}

Layout Layout::LayoutFromAddress(const void *address, size_t size) {
  if (address == nullptr || size == 0) {
    return Layout();
  }
  return Layout(static_cast<LayoutNode *>(
      TreePool::sharedPool->copyTreeFromAddress(address, size)));
}

void Layout::draw(KDContext *ctx, KDPoint p, KDFont::Size font,
                  KDColor expressionColor, KDColor backgroundColor,
                  LayoutSelection selection, KDColor selectionColor) {
  node()->draw(ctx, p, font, expressionColor, backgroundColor, selection,
               selectionColor);
}

void Layout::draw(KDContext *ctx, KDPoint p, KDFont::Size font,
                  KDColor expressionColor, KDColor backgroundColor) {
  draw(ctx, p, font, expressionColor, backgroundColor, LayoutSelection());
}

int Layout::serializeParsedExpression(char *buffer, int bufferSize,
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
  Poincare::Expression e = Poincare::Expression::Parse(buffer, context);
  if (e.isUninitialized()) {
    buffer[0] = 0;
    return 0;
  }
  return e.serialize(buffer, bufferSize,
                     Poincare::Preferences::sharedPreferences->displayMode());
}

Layout Layout::recursivelyMatches(LayoutTest test) const {
  TrinaryBoolean testResult = test(*this);
  if (testResult == TrinaryBoolean::True) {
    return *this;
  }
  if (testResult == TrinaryBoolean::False) {
    return Layout();
  }
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    Layout childResult = childAtIndex(i).recursivelyMatches(test);
    if (!childResult.isUninitialized()) {
      return childResult;
    }
  }
  return Layout();
}

Layout Layout::XNTLayout() const {
  Layout xntLayout = const_cast<Layout *>(this)->node()->XNTLayout();
  assert(xntLayout.isUninitialized() ||
         xntLayout.numberOfDescendants(true) >= 0);
  if (!xntLayout.isUninitialized() &&
      static_cast<size_t>(xntLayout.numberOfDescendants(true)) <=
          SymbolAbstract::k_maxNameSize &&
      xntLayout
          .recursivelyMatches([](const Layout l) {
            if (l.type() != LayoutNode::Type::CodePointLayout) {
              return l.isHorizontal() ? TrinaryBoolean::Unknown
                                      : TrinaryBoolean::True;
            }
            CodePoint c = static_cast<const CodePointLayout &>(l).codePoint();
            return (c.isDecimalDigit() || c.isLatinLetter() || c == '_')
                       ? TrinaryBoolean::Unknown
                       : TrinaryBoolean::True;
          })
          .isUninitialized()) {
    /* Return xnt if :
     * - it is initialized and only contains horizontal layouts and code points
     * - all code points are letters, numbers or _
     * - There are less than k_maxNameSize descendants */
    return xntLayout;
  }
  return Layout();
}

bool Layout::shouldCollapseSiblingsOnRight() const {
  return type() == LayoutNode::Type::ConjugateLayout ||
         type() == LayoutNode::Type::FractionLayout ||
         type() == LayoutNode::Type::NthRootLayout ||
         // SquareBracketPairLayouts
         type() == LayoutNode::Type::AbsoluteValueLayout ||
         type() == LayoutNode::Type::CeilingLayout ||
         type() == LayoutNode::Type::FloorLayout ||
         type() == LayoutNode::Type::VectorNormLayout;
}

Layout Layout::childAtIndex(int i) const {
  assert(i >= 0 && i < numberOfChildren());
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Layout &>(c);
}

bool Layout::privateHasTopLevelComparisonSymbol(
    bool includingNotEqualSymbol) const {
  if (type() != Poincare::LayoutNode::Type::HorizontalLayout) {
    return false;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Poincare::Layout child = childAtIndex(i);
    if ((child.type() == Poincare::LayoutNode::Type::CodePointLayout &&
         static_cast<Poincare::CodePointLayout &>(child)
             .codePoint()
             .isEquationOperator()) ||
        (includingNotEqualSymbol &&
         child.type() == Poincare::LayoutNode::Type::CombinedCodePointsLayout &&
         static_cast<Poincare::CombinedCodePointsLayout &>(child)
             .node()
             ->isNotEqualOperator())) {
      return true;
    }
  }
  return false;
}

}  // namespace Poincare
