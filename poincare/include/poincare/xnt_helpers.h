#ifndef POINCARE_XNT_HELPERS_H
#define POINCARE_XNT_HELPERS_H

#include <ion/unicode/utf8_decoder.h>
#include <poincare_expressions.h>
#include <poincare_layouts.h>

namespace Poincare {

namespace XNTHelpers {

constexpr static struct {
  AliasesList aliasesList;
  LayoutNode::Type layoutType;
  char defaultXNT;
} k_parameteredFunctions[] = {
    {Derivative::s_functionHelper.aliasesList(),
     LayoutNode::Type::FirstOrderDerivativeLayout,
     Derivative::k_defaultXNTChar},
    {Derivative::s_functionHelper.aliasesList(),
     LayoutNode::Type::HigherOrderDerivativeLayout,
     Derivative::k_defaultXNTChar},
    {Integral::s_functionHelper.aliasesList(), LayoutNode::Type::IntegralLayout,
     Integral::k_defaultXNTChar},
    {Sum::s_functionHelper.aliasesList(), LayoutNode::Type::SumLayout,
     Sum::k_defaultXNTChar},
    {Product::s_functionHelper.aliasesList(), LayoutNode::Type::ProductLayout,
     Product::k_defaultXNTChar},
    {ListSequence::s_functionHelper.aliasesList(),
     LayoutNode::Type::ListSequenceLayout, ListSequence::k_defaultXNTChar},
};
constexpr static int k_numberOfFunctions = std::size(k_parameteredFunctions);
constexpr static int k_indexOfMainExpression = 0;
constexpr static int k_indexOfParameter = 1;

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize);

bool FindXNTSymbol2D(Layout layout, char* buffer, size_t bufferSize);

}  // namespace XNTHelpers

}  // namespace Poincare

#endif
