#ifndef POINCARE_LAYOUT_HELPER_H
#define POINCARE_LAYOUT_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/string_format.h>

/* WARNING : "String" creates a stringLayout if the buffer is more than 1 codepoint long.
 * If you want to specifically create CodePoints layouts, use the function
 * "StringToCodePointsLayout".
 * */

namespace Poincare {

namespace LayoutHelper {
  /* Expression to Layout */
  typedef bool (*OperatorTest)(Expression left, Expression right);
  Layout Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName, OperatorTest forbidOperator = nullptr);
  Layout Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName);

  /* Create special layouts */
  Layout Parentheses(Layout layout, bool cloneLayout);
  /* Create StringLayout from buffer (or CodePointLayout if bufferLen = 1) */
  Layout String(const char * buffer, int bufferLen = -1, const KDFont * font = StringFormat::k_defaultFont);
  /* Create StringLayout from expression */
  Layout StringLayoutOfSerialization(const Expression & expression, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits);
  /* Create HorizontalLayout with CodePointLayouts from buffer */
  Layout StringToCodePointsLayout(const char * buffer, int bufferLen, const KDFont * font = StringFormat::k_defaultFont);
  /* Create StringLayout from buffer */
  Layout StringToStringLayout(const char * buffer, int bufferLen, const KDFont * font = StringFormat::k_defaultFont);
  /* Create HorizontalLayout with CodePointLayouts from buffer */
  Layout CodePointsToLayout(const CodePoint * buffer, int bufferLen, const KDFont * font = StringFormat::k_defaultFont);

  Layout Logarithm(Layout argument, Layout index);
  HorizontalLayout CodePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript);
};

}

#endif
