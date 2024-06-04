#include <assert.h>
#include <ion.h>
#include <omg/utf8_decoder.h>
#include <poincare/layout.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/constant.h>
#include <poincare/old/float.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/nonreal.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/symbol.h>

#include <algorithm>
#include <cmath>

namespace Poincare {

ConstantNode::ConstantNode(const char* name, int length)
    : ExpressionNode(),
      m_constantInfo(
          &k_constants[Constant::ConstantInfoIndexFromName(name, length)]) {
  assert(Constant::IsConstant(name, length));
}

OMG::Troolean ConstantNode::isPositive(Context* context) const {
  return isPi() || isExponentialE() ? OMG::Troolean::True
                                    : OMG::Troolean::Unknown;
}

bool ConstantNode::isReal() const { return isPi() || isExponentialE(); }

int ConstantNode::simplificationOrderSameType(const ExpressionNode* e,
                                              bool ascending,
                                              bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(otype() == e->otype());
  return rankOfConstant() -
         static_cast<const ConstantNode*>(e)->rankOfConstant();
}

size_t ConstantNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  return std::min<size_t>(strlcpy(buffer, name(), bufferSize), bufferSize - 1);
}

OExpression ConstantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Constant(this).shallowReduce(reductionContext);
}

bool ConstantNode::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, OExpression symbolValue) {
  return Constant(this).derivate(reductionContext, symbol, symbolValue);
}

bool ConstantNode::isConstant(const char* constantName) const {
  return constantInfo().m_aliasesList.contains(constantName,
                                               strlen(constantName));
}

Constant Constant::Builder(const char* name, int length) {
  void* bufferNode = Pool::sharedPool->alloc(sizeof(ConstantNode));
  ConstantNode* node = new (bufferNode) ConstantNode(name, length);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<Constant&>(h);
}

OExpression Constant::shallowReduce(ReductionContext reductionContext) {
  if (isExponentialE() || isPi()) {
    return *this;
  }
  ConstantNode::ConstantInfo info = constantInfo();
  OExpression result;
  if (isComplexI()) {
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      result = Nonreal::Builder();
    } else if (reductionContext.target() == ReductionTarget::User) {
      result =
          ComplexCartesian::Builder(Rational::Builder(0), Rational::Builder(1));
    } else {
      return *this;
    }
  } else {
    assert(info.m_unit != nullptr);
    assert(false);
    // result = Multiplication::Builder(Float<double>::Builder(info.m_value),
    // OExpression::Parse(info.m_unit, nullptr));
    // result.childAtIndex(1).deepReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

bool Constant::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        OExpression symbolValue) {
  ConstantNode::ConstantInfo info = constantInfo();
  if (info.m_unit == nullptr) {
    if (std::isnan(info.m_value)) {
      replaceWithInPlace(Undefined::Builder());
    } else {
      replaceWithInPlace(Rational::Builder(0));
    }
    return true;
  }
  return false;
}

int Constant::ConstantInfoIndexFromName(const char* name, int length) {
  for (int i = 0; i < ConstantNode::k_numberOfConstants; i++) {
    if (ConstantNode::k_constants[i].m_aliasesList.contains(name, length)) {
      return i;
    }
  }
  return -1;
}

}  // namespace Poincare
