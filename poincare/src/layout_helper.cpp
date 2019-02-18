#include <poincare/layout_helper.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <assert.h>

namespace Poincare {

Layout LayoutHelper::Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  int numberOfChildren = expression.numberOfChildren();
  assert(numberOfChildren > 1);
  HorizontalLayout result = HorizontalLayout::Builder();
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

Layout LayoutHelper::Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  HorizontalLayout result = HorizontalLayout::Builder();
  // Add the operator name.
  result.addOrMergeChildAtIndex(String(operatorName, strlen(operatorName)), 0, true);

  // Create the layout of arguments separated by commas.
  HorizontalLayout args = HorizontalLayout::Builder();
  int numberOfChildren = expression.numberOfChildren();
  if (numberOfChildren > 0) {
    args.addOrMergeChildAtIndex(expression.childAtIndex(0).createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
    for (int i = 1; i < numberOfChildren; i++) {
      args.addChildAtIndex(CharLayout::Builder(','), args.numberOfChildren(), args.numberOfChildren(), nullptr);
      args.addOrMergeChildAtIndex(expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits), args.numberOfChildren(), true);
    }
  }
  // Add the parenthesed arguments.
  result.addOrMergeChildAtIndex(Parentheses(args, false), result.numberOfChildren(), true);
  return result;
}

Layout LayoutHelper::Parentheses(Layout layout, bool cloneLayout) {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addChildAtIndex(LeftParenthesisLayout::Builder(), 0, 0, nullptr);
  if (!layout.isUninitialized()) {
    result.addOrMergeChildAtIndex(cloneLayout ? layout.clone() : layout, 1, true);
  }
  result.addChildAtIndex(RightParenthesisLayout::Builder(), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  return result;
}

HorizontalLayout LayoutHelper::String(const char * buffer, int bufferLen, const KDFont * font) {
  assert(bufferLen > 0);
  HorizontalLayout resultLayout = HorizontalLayout::Builder();
  for (int i = 0; i < bufferLen; i++) {
    resultLayout.addChildAtIndex(CharLayout::Builder(buffer[i], font), i, i, nullptr);
  }
  return resultLayout;
}

Layout LayoutHelper::Logarithm(Layout argument, Layout index) {
  HorizontalLayout resultLayout = String("log", 3);
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(index, VerticalOffsetLayoutNode::Type::Subscript);
  resultLayout.addChildAtIndex(offsetLayout, resultLayout.numberOfChildren(), resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false), resultLayout.numberOfChildren(), true);
  return resultLayout;
}

}
