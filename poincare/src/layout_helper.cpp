#include <poincare/layout_helper.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <poincare/vertical_offset_layout_node.h>
#include <assert.h>

namespace Poincare {

LayoutRef LayoutHelper::Infix(const Expression expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  int numberOfChildren = expression.numberOfChildren();
  assert(numberOfChildren > 1);
  HorizontalLayoutRef result;
  result.addOrMergeChildAtIndex(expression.childAtIndex(0).createLayout(floatDisplayMode, numberOfSignificantDigits), 0, 0);
  for (int i = 1; i < numberOfChildren; i++) {
    result.addOrMergeChildAtIndex(String(operatorName, strlen(operatorName)), result.numberOfChildren(), true);
    result.addOrMergeChildAtIndex(
        expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits),
        result.numberOfChildren(),
        true);
  }
  return result;
}

LayoutRef LayoutHelper::Prefix(const Expression expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  HorizontalLayoutRef result;
  // Add the operator name.
  result.addOrMergeChildAtIndex(String(operatorName, strlen(operatorName)), 0, true);

  // Create the layout of arguments separated by commas.
  HorizontalLayoutRef args;
  int numberOfChildren = expression.numberOfChildren();
  if (numberOfChildren > 0) {
    args.addOrMergeChildAtIndex(expression.childAtIndex(0).createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
    for (int i = 1; i < numberOfChildren; i++) {
      args.addChildAtIndex(CharLayoutRef(','), args.numberOfChildren(), args.numberOfChildren(), nullptr);
      args.addOrMergeChildAtIndex(expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits), args.numberOfChildren(), true);
    }
  }
  // Add the parenthesed arguments.
  result.addOrMergeChildAtIndex(Parentheses(args, false), result.numberOfChildren(), true);
  return result;
}

LayoutRef LayoutHelper::Parentheses(LayoutRef layoutRef, bool cloneLayout) {
  HorizontalLayoutRef result;
  result.addChildAtIndex(LeftParenthesisLayoutRef(), 0, 0, nullptr);
  if (layoutRef.isDefined()) {
    result.addOrMergeChildAtIndex(cloneLayout ? layoutRef.clone() : layoutRef, 1, true);
  }
  result.addChildAtIndex(RightParenthesisLayoutRef(), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  return result;
}

HorizontalLayoutRef LayoutHelper::String(const char * buffer, int bufferSize, KDText::FontSize fontSize) {
  assert(bufferSize > 0);
  HorizontalLayoutRef resultLayout;
  for (int i = 0; i < bufferSize; i++) {
    resultLayout.addChildAtIndex(CharLayoutRef(buffer[i], fontSize), i, i, nullptr);
  }
  return resultLayout;
}

LayoutRef LayoutHelper::Logarithm(LayoutRef argument, LayoutRef index) {
  HorizontalLayoutRef resultLayout = String("log", 3);
  VerticalOffsetLayoutRef offsetLayout = VerticalOffsetLayoutRef(index, VerticalOffsetLayoutNode::Type::Subscript);
  resultLayout.addChildAtIndex(offsetLayout, resultLayout.numberOfChildren(), resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false), resultLayout.numberOfChildren(), true);
  return resultLayout;
}

}
