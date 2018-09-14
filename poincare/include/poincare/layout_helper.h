#ifndef POINCARE_LAYOUT_HELPER_H
#define POINCARE_LAYOUT_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout_reference.h>
#include <poincare/horizontal_layout_node.h>

namespace Poincare {

namespace LayoutHelper {
  /* Expression to Layout */
  LayoutReference Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);
  LayoutReference Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  LayoutReference Parentheses(LayoutReference layout, bool cloneLayout);
  HorizontalLayoutReference String(const char * buffer, int bufferSize, KDText::FontSize fontSize = KDText::FontSize::Large);
  LayoutReference Logarithm(LayoutReference argument, LayoutReference index);
};

}

#endif
