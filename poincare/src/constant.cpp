#include <assert.h>
#include <ion.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/code_point_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/nonreal.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>

#include <algorithm>
#include <cmath>

namespace Poincare {

ConstantNode::ConstantNode(const char* name, int length)
    : ExpressionNode(),
      m_constantInfo(
          &k_constants[Constant::ConstantInfoIndexFromName(name, length)]) {
  assert(Constant::IsConstant(name, length));
}

TrinaryBoolean ConstantNode::isPositive(Context* context) const {
  return isPi() || isExponentialE() ? TrinaryBoolean::True
                                    : TrinaryBoolean::Unknown;
}

bool ConstantNode::isReal() const { return isPi() || isExponentialE(); }

int ConstantNode::simplificationOrderSameType(const ExpressionNode* e,
                                              bool ascending,
                                              bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(type() == e->type());
  return rankOfConstant() -
         static_cast<const ConstantNode*>(e)->rankOfConstant();
}

int ConstantNode::serialize(char* buffer, int bufferSize,
                            Preferences::PrintFloatMode floatDisplayMode,
                            int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  return std::min<int>(strlcpy(buffer, name(), bufferSize), bufferSize - 1);
}

Layout ConstantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits,
                                  Context* context) const {
  constexpr static size_t bufferSize = Helpers::StringLength("_hplanck") + 1;
  assert(strlen(name()) < bufferSize);
  char buffer[bufferSize];
  return LayoutHelper::StringLayoutOfSerialization(Constant(this), buffer,
                                                   bufferSize, floatDisplayMode,
                                                   numberOfSignificantDigits);
}

template <typename T>
Evaluation<T> ConstantNode::templatedApproximate() const {
  if (isComplexI()) {
    return Complex<T>::Builder(0.0, 1.0);
  }
  ConstantInfo info = constantInfo();
  return info.m_unit ? Complex<T>::Undefined()
                     : Complex<T>::Builder(info.m_value);
}

Expression ConstantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Constant(this).shallowReduce(reductionContext);
}

bool ConstantNode::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, Expression symbolValue) {
  return Constant(this).derivate(reductionContext, symbol, symbolValue);
}

bool ConstantNode::isConstant(const char* constantName) const {
  return constantInfo().m_aliasesList.contains(constantName,
                                               strlen(constantName));
}

Constant Constant::Builder(const char* name, int length) {
  void* bufferNode = TreePool::sharedPool->alloc(sizeof(ConstantNode));
  ConstantNode* node = new (bufferNode) ConstantNode(name, length);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Constant&>(h);
}

Expression Constant::shallowReduce(ReductionContext reductionContext) {
  if (isExponentialE() || isPi()) {
    return *this;
  }
  ConstantNode::ConstantInfo info = constantInfo();
  Expression result;
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
    result = Multiplication::Builder(Float<double>::Builder(info.m_value),
                                     Expression::Parse(info.m_unit, nullptr));
    result.childAtIndex(1).deepReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

bool Constant::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        Expression symbolValue) {
  ConstantNode::ConstantInfo info = constantInfo();
  if (info.m_unit == nullptr && !std::isnan(info.m_value)) {
    replaceWithInPlace(Rational::Builder(0));
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
