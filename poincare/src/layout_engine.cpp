#include <poincare/layout_engine.h>
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include<assert.h>
}

namespace Poincare {

ExpressionLayout * LayoutEngine::createInfixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  assert(numberOfOperands > 1);
  ExpressionLayout** children_layouts = new ExpressionLayout * [2*numberOfOperands-1];
  children_layouts[0] = expression->operand(0)->createLayout();
  for (int i=1; i<numberOfOperands; i++) {
    children_layouts[2*i-1] = new StringLayout(operatorName, 1);
    children_layouts[2*i] = expression->operand(i)->type() == Expression::Type::Opposite ? new ParenthesisLayout(expression->operand(i)->createLayout(floatDisplayMode, complexFormat)) : expression->operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  /* HorizontalLayout holds the children layouts so they do not need to be
   * deleted here. */
  ExpressionLayout * layout = new HorizontalLayout(children_layouts, 2*numberOfOperands-1);
  delete[] children_layouts;
  return layout;
}

ExpressionLayout * LayoutEngine::createPrefixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  ExpressionLayout ** grandChildrenLayouts = new ExpressionLayout *[2*numberOfOperands-1];
  int layoutIndex = 0;
  grandChildrenLayouts[layoutIndex++] = expression->operand(0)->createLayout(floatDisplayMode, complexFormat);
  for (int i = 1; i < numberOfOperands; i++) {
    grandChildrenLayouts[layoutIndex++] = new StringLayout(",", 1);
    grandChildrenLayouts[layoutIndex++] = expression->operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  /* HorizontalLayout holds the grand children layouts so they do not need to
   * be deleted */
  ExpressionLayout * argumentLayouts = new HorizontalLayout(grandChildrenLayouts, 2*numberOfOperands-1);
  delete [] grandChildrenLayouts;
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout(operatorName, strlen(operatorName));
  childrenLayouts[1] = new ParenthesisLayout(argumentLayouts);
  /* Same comment as above */
  return new HorizontalLayout(childrenLayouts, 2);
}

}
