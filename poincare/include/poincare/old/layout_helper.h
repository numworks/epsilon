#ifndef POINCARE_LAYOUT_HELPER_H
#define POINCARE_LAYOUT_HELPER_H

#include <poincare/layout.h>

#include "horizontal_layout.h"
#include "old_expression.h"

/* WARNING : "String" creates a stringLayout if the buffer is more than 1
 * codepoint long. If you want to specifically create CodePoints layouts, use
 * the function "StringToCodePointsLayout".
 * */

namespace Poincare {

namespace LayoutHelper {
/* OExpression to OLayout */
typedef OLayout (*OperatorLayoutForInfix)(const char* operatorName,
                                          OExpression left, OExpression right,
                                          OLayout rightLayout);
OLayout DefaultCreateOperatorLayoutForInfix(const char* operatorName,
                                            OExpression left, OExpression right,
                                            OLayout rightLayout);

/* Create special layouts */
OLayout Parentheses(OLayout layout, bool cloneLayout);
/* Create StringLayout from buffer (or CodePointLayout if bufferLen = 1) */
OLayout String(const char* buffer, int bufferLen = -1);
/* Create StringLayout from expression */
OLayout StringLayoutOfSerialization(
    const OExpression& expression, char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits);
/* Create HorizontalLayout with CodePointLayouts from buffer */
OLayout StringToCodePointsLayout(const char* buffer, int bufferLen);
/* Create StringLayout from buffer */
OLayout StringToStringLayout(const char* buffer, int bufferLen);
/* Create HorizontalLayout with CodePointLayouts from buffer */
OLayout CodePointsToLayout(const CodePoint* buffer, int bufferLen);

OLayout Logarithm(OLayout argument, OLayout index);
HorizontalLayout CodePointSubscriptCodePointLayout(CodePoint base,
                                                   CodePoint subscript);
};  // namespace LayoutHelper

}  // namespace Poincare

#endif
