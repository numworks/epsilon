#ifndef POINCARE_XNT_HELPERS_H
#define POINCARE_XNT_HELPERS_H

#include <apps/shared/continuous_function.h>
#include <apps/shared/sequence.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare_expressions.h>
#include <poincare_layouts.h>

namespace Poincare {

namespace XNTHelpers {

constexpr static CodePoint k_defaultXNTCycle[] = {
    Shared::ContinuousFunction::k_cartesianSymbol,
    Shared::Sequence::k_sequenceSymbol,
    Shared::ContinuousFunction::k_parametricSymbol,
    Shared::ContinuousFunction::k_polarSymbol,
};

CodePoint CodePointAtIndexInCycle(int index, CodePoint startingCodePoint);

constexpr static char k_defaultContinuousXNT = 'x';
constexpr static char k_defaultDiscreteXNT = 'k';

constexpr static struct {
  AliasesList aliasesList;
  LayoutNode::Type layoutType;
  char defaultXNT;
} k_parameteredFunctions[] = {
    {Derivative::s_functionHelper.aliasesList(),
     LayoutNode::Type::FirstOrderDerivativeLayout, k_defaultContinuousXNT},
    {Derivative::s_functionHelper.aliasesList(),
     LayoutNode::Type::HigherOrderDerivativeLayout, k_defaultContinuousXNT},
    {Integral::s_functionHelper.aliasesList(), LayoutNode::Type::IntegralLayout,
     k_defaultContinuousXNT},
    {Sum::s_functionHelper.aliasesList(), LayoutNode::Type::SumLayout,
     k_defaultDiscreteXNT},
    {Product::s_functionHelper.aliasesList(), LayoutNode::Type::ProductLayout,
     k_defaultDiscreteXNT},
    {ListSequence::s_functionHelper.aliasesList(),
     LayoutNode::Type::ListSequenceLayout, k_defaultDiscreteXNT},
};
constexpr static int k_numberOfFunctions = std::size(k_parameteredFunctions);
constexpr static int k_indexOfMainExpression = 0;
constexpr static int k_indexOfParameter = 1;

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize);

bool FindXNTSymbol2D(Layout layout, char* buffer, size_t bufferSize);

}  // namespace XNTHelpers

}  // namespace Poincare

#endif
