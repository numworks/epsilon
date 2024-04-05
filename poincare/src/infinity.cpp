#include <poincare/complex.h>
#include <poincare/infinity.h>
#include <poincare/layout.h>
#include <poincare/symbol.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

size_t InfinityNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  return PrintFloat::ConvertFloatToText<float>(
             m_negative ? -INFINITY : INFINITY, buffer, bufferSize,
             PrintFloat::k_maxFloatGlyphLength, numberOfSignificantDigits,
             floatDisplayMode)
      .CharLength;
}

template <typename T>
Evaluation<T> InfinityNode::templatedApproximate() const {
  return Complex<T>::Builder(m_negative ? -INFINITY : INFINITY);
}

bool InfinityNode::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, OExpression symbolValue) {
  return Infinity(this).derivate(reductionContext, symbol, symbolValue);
}

Infinity Infinity::Builder(bool negative) {
  void* bufferNode = Pool::sharedPool->alloc(sizeof(InfinityNode));
  InfinityNode* node = new (bufferNode) InfinityNode(negative);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Infinity&>(h);
}

bool Infinity::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        OExpression symbolValue) {
  replaceWithUndefinedInPlace();
  return true;
}

template Evaluation<float> InfinityNode::templatedApproximate<float>() const;
template Evaluation<double> InfinityNode::templatedApproximate() const;
}  // namespace Poincare
