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

template<typename T, typename U, int N, typename Parent>
class LayoutHandle : public Parent {
public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<Layout, Parent>::value);
#endif
  static T Builder() {
    static_assert(N == 0);
    TreeHandle h = TreeHandle::Maker(Initializer<U>, sizeof(U), {});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child) {
    static_assert(N == 1);
    TreeHandle h = TreeHandle::Maker(Initializer<U>, sizeof(U), {child});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child1, Layout child2) {
    static_assert(N == 2);
    TreeHandle h = TreeHandle::Maker(Initializer<U>, sizeof(U), {child1, child2});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child1, Layout child2, Layout child3) {
    static_assert(N == 3);
    TreeHandle h = TreeHandle::Maker(Initializer<U>, sizeof(U), {child1, child2, child3});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child1, Layout child2, Layout child3, Layout child4) {
    static_assert(N == 4);
    TreeHandle h = TreeHandle::Maker(Initializer<U>, sizeof(U), {child1, child2, child3, child4});
    return static_cast<T&>(h);
  }
};

template<typename T, typename U, typename P = Layout> using LayoutNoChildren = LayoutHandle<T,U,0,P>;
template<typename T, typename U, typename P = Layout> using LayoutOneChild = LayoutHandle<T,U,1,P>;
template<typename T, typename U, typename P = Layout> using LayoutTwoChildren = LayoutHandle<T,U,2,P>;
template<typename T, typename U, typename P = Layout> using LayoutThreeChildren = LayoutHandle<T,U,3,P>;
template<typename T, typename U, typename P = Layout> using LayoutFourChildren = LayoutHandle<T,U,4,P>;

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
