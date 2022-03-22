#ifndef POINCARE_LAYOUT_HELPER_H
#define POINCARE_LAYOUT_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/horizontal_layout.h>

/* "String" creates a stringLayout if the buffer is more than 1 codepoint long.
 * If you want to specifically create CodePoints layouts, use the function
 * "StringToCodePointsLayout".
 * */

namespace Poincare {

namespace LayoutHelper {
  /* Expression to Layout */
  Layout Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);
  Layout Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  Layout Parentheses(Layout layout, bool cloneLayout);
  Layout String(const char * buffer, int bufferLen = -1, const KDFont * font = KDFont::LargeFont);
  Layout StringLayoutOfSerialization(const Expression & expression, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits);
  Layout StringToCodePointsLayout(const char * buffer, int bufferLen, const KDFont * font = KDFont::LargeFont);
  Layout StringToStringLayout(const char * buffer, int bufferLen, const KDFont * font = KDFont::LargeFont);
  Layout CodePointsToLayout(const CodePoint * buffer, int bufferLen, const KDFont * font = KDFont::LargeFont);
  Layout Logarithm(Layout argument, Layout index);
  HorizontalLayout CodePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript);
};

}

#endif
