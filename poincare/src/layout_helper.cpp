#include <poincare/layout_helper.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <assert.h>

namespace Poincare {

LayoutReference LayoutHelper::Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  int numberOfChildren = expression.numberOfChildren();
  assert(numberOfChildren > 1);
  HorizontalLayoutReference result;
  result.addOrMergeChildAtIndex(expression.childAtIndex(0).createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
  for (int i = 1; i < numberOfChildren; i++) {
    result.addOrMergeChildAtIndex(String(operatorName, strlen(operatorName)), result.numberOfChildren(), true);
    result.addOrMergeChildAtIndex(
        expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits),
        result.numberOfChildren(),
        true);
  }
  return result;
}

LayoutReference LayoutHelper::Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  HorizontalLayoutReference result;
  // Add the operator name.
  result.addOrMergeChildAtIndex(String(operatorName, strlen(operatorName)), 0, true);

  // Create the layout of arguments separated by commas.
  HorizontalLayoutReference args;
  int numberOfChildren = expression.numberOfChildren();
  if (numberOfChildren > 0) {
    args.addOrMergeChildAtIndex(expression.childAtIndex(0).createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
    for (int i = 1; i < numberOfChildren; i++) {
      args.addChildAtIndex(CharLayoutReference(','), args.numberOfChildren(), args.numberOfChildren(), nullptr);
      args.addOrMergeChildAtIndex(expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits), args.numberOfChildren(), true);
    }
  }
  // Add the parenthesed arguments.
  result.addOrMergeChildAtIndex(Parentheses(args, false), result.numberOfChildren(), true);
  return result;
}

LayoutReference LayoutHelper::Parentheses(LayoutReference layoutRef, bool cloneLayout) {
  HorizontalLayoutReference result;
  result.addChildAtIndex(LeftParenthesisLayoutReference(), 0, 0, nullptr);
  if (!layoutRef.isUninitialized()) {
    result.addOrMergeChildAtIndex(cloneLayout ? layoutRef.clone() : layoutRef, 1, true);
  }
  result.addChildAtIndex(RightParenthesisLayoutReference(), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  return result;
}

HorizontalLayoutReference LayoutHelper::String(const char * buffer, int bufferSize, KDText::FontSize fontSize) {
  assert(bufferSize > 0);
  HorizontalLayoutReference resultLayout;
  for (int i = 0; i < bufferSize; i++) {
    resultLayout.addChildAtIndex(CharLayoutReference(buffer[i], fontSize), i, i, nullptr);
  }
  return resultLayout;
}

LayoutReference LayoutHelper::Logarithm(LayoutReference argument, LayoutReference index) {
  HorizontalLayoutReference resultLayout = String("log", 3);
  VerticalOffsetLayoutReference offsetLayout = VerticalOffsetLayoutReference(index, VerticalOffsetLayoutNode::Type::Subscript);
  resultLayout.addChildAtIndex(offsetLayout, resultLayout.numberOfChildren(), resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false), resultLayout.numberOfChildren(), true);
  return resultLayout;
}

}
