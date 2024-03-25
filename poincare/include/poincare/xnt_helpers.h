#ifndef POINCARE_XNT_HELPERS_H
#define POINCARE_XNT_HELPERS_H

#include <ion/unicode/utf8_decoder.h>
#include <poincare_expressions.h>

#include "symbol.h"

namespace Poincare {

namespace XNTHelpers {

// Cycles
constexpr int k_maxCycleSize = 5;
constexpr CodePoint k_defaultXNTCycle[] = {
    Symbol::k_cartesianSymbol,
    Symbol::k_sequenceSymbol,
    Symbol::k_parametricSymbol,
    Symbol::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultContinuousXNTCycle[] = {
    Symbol::k_cartesianSymbol,
    Symbol::k_parametricSymbol,
    Symbol::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultDiscreteXNTCycle[] = {
    'k',
    'n',
    UCodePointNull,
};

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t* cycleSize);
CodePoint CodePointAtIndexInCycle(int index, const CodePoint* cycle,
                                  size_t* cycleSize);

// Parametered functions
constexpr struct {
  AliasesList aliasesList;
  LayoutNode::Type layoutType;
  const CodePoint* XNTcycle;
} k_parameteredFunctions[] = {
    {Derivative::s_functionHelper.aliasesList(),
     LayoutNode::Type::FirstOrderDerivativeLayout, k_defaultContinuousXNTCycle},
    {Derivative::s_functionHelper.aliasesList(),
     LayoutNode::Type::HigherOrderDerivativeLayout,
     k_defaultContinuousXNTCycle},
    {Integral::s_functionHelper.aliasesList(), LayoutNode::Type::IntegralLayout,
     k_defaultContinuousXNTCycle},
    {Sum::s_functionHelper.aliasesList(), LayoutNode::Type::SumLayout,
     k_defaultDiscreteXNTCycle},
    {Product::s_functionHelper.aliasesList(), LayoutNode::Type::ProductLayout,
     k_defaultDiscreteXNTCycle},
    {ListSequence::s_functionHelper.aliasesList(),
     LayoutNode::Type::ListSequenceLayout, k_defaultDiscreteXNTCycle},
};
constexpr int k_numberOfFunctions = std::size(k_parameteredFunctions);
constexpr int k_indexOfMainExpression =
    ParameteredExpression::ParameteredChildIndex();
constexpr int k_indexOfParameter = ParameteredExpression::ParameterChildIndex();

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize);

bool FindXNTSymbol2D(Layout layout, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize);

}  // namespace XNTHelpers

}  // namespace Poincare

#endif
