#ifndef POINCARE_LAYOUT_HELPER_H
#define POINCARE_LAYOUT_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/horizontal_layout.h>

namespace Poincare {

namespace LayoutHelper {
  /* Expression to Layout */
  Layout Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);
  Layout Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  Layout Parentheses(Layout layout, bool cloneLayout);
  Layout String(const char * buffer, int bufferLen, const KDFont * font = KDFont::LargeFont);
  Layout CodePointString(const CodePoint * buffer, int bufferLen, const KDFont * font = KDFont::LargeFont);
  Layout Logarithm(Layout argument, Layout index);
};

}

#endif
