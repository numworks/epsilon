#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/infinity.h>
#include <poincare/old/symbol.h>

extern "C" {
#include <string.h>

#include <cmath>
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

bool InfinityNode::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, OExpression symbolValue) {
  return Infinity(this).derivate(reductionContext, symbol, symbolValue);
}

Infinity Infinity::Builder(bool negative) {
  void* bufferNode = Pool::sharedPool->alloc(sizeof(InfinityNode));
  InfinityNode* node = new (bufferNode) InfinityNode(negative);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<Infinity&>(h);
}

bool Infinity::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        OExpression symbolValue) {
  replaceWithUndefinedInPlace();
  return true;
}

}  // namespace Poincare
