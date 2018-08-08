#ifndef POINCARE_LAYOUT_HELPER_H
#define POINCARE_LAYOUT_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout_reference.h>
#include <poincare/horizontal_layout_node.h>

namespace Poincare {

namespace LayoutHelper {
  /* Expression to Layout */
  static LayoutRef Infix(const Expression expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);
  static LayoutRef Prefix(const Expression expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  static LayoutRef Parentheses(LayoutRef layout, bool cloneLayout);
  static HorizontalLayoutRef String(const char * buffer, int bufferSize, KDText::FontSize fontSize = KDText::FontSize::Large);
  static LayoutRef Logarithm(LayoutRef argument, LayoutRef index);
};

}

#endif
