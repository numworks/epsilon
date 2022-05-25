#include <poincare/constant.h>
#include <poincare/code_point_layout.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/nonreal.h>
#include <poincare/symbol.h>
#include <ion.h>
#include <cmath>
#include <algorithm>
#include <assert.h>
#include <ion/unicode/utf8_decoder.h>

namespace Poincare {

constexpr ConstantNode::ConstantInfo Constant::k_constants[];

ConstantNode::ConstantNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

ExpressionNode::Sign ConstantNode::sign(Context * context) const {
  ConstantInfo info = constantInfo();
  if (isConstant("π", info) || isConstant("e", info)) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

bool ConstantNode::isReal() const {
  ConstantInfo info = constantInfo();
  return isConstant("π", info) || isConstant("e", info);
}

int ConstantNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(type() == e->type());
  return rankOfConstant() - static_cast<const ConstantNode *>(e)->rankOfConstant();
}

int ConstantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  return SymbolAbstractNode::serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

Layout ConstantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  static constexpr size_t bufferSize = 10; // TODO: optimize buffer size
  char buffer[bufferSize];
  return LayoutHelper::StringLayoutOfSerialization(Constant(this), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

template<typename T>
Evaluation<T> ConstantNode::templatedApproximate() const {
  ConstantInfo info = constantInfo();
  if (isConstant("i", info)) {
    return Complex<T>::Builder(0.0, 1.0);
  }
  if (info.unit() == nullptr) {
    return Complex<T>::Builder(info.value());
  }
  return Complex<T>::Undefined();
}

Expression ConstantNode::shallowReduce(ReductionContext reductionContext) {
  return Constant(this).shallowReduce(reductionContext);
}

bool ConstantNode::derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  return Constant(this).derivate(reductionContext, symbol, symbolValue);
}

ConstantNode::ConstantInfo ConstantNode::constantInfo() const {
  for (ConstantNode::ConstantInfo info : Constant::k_constants) {
    if (strcmp(info.name(), m_name) == 0) {
      return info;
    }
  }
  return ConstantInfo("", -1);
}

bool ConstantNode::isConstant(const char * constantName, ConstantInfo info) const {
  if (info.name() == nullptr) {
    info = constantInfo();
  }
  return strcmp(info.name(), constantName) == 0;
}

bool Constant::IsConstant(const char * name, size_t length) {
  for (ConstantNode::ConstantInfo info : Constant::k_constants) {
    size_t constantLength = strlen(info.name());
    if (length < constantLength) {
     continue;
    }
    if (strncmp(info.name(), name, length) == 0) {
      return true;
    }
  }
  return false;
}

Expression Constant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  ConstantNode::ConstantInfo info = constantInfo();
  if (isConstant("e", info) || isConstant("π", info)) {
    return *this;
  }
  Expression result;
  if (isConstant("i", info)) {
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      result = Nonreal::Builder();
    } else if (reductionContext.target() == ExpressionNode::ReductionTarget::User) {
      result = ComplexCartesian::Builder(Rational::Builder(0), Rational::Builder(1));
    } else {
      return *this;
    }
  } else {
    assert(info.unit() != nullptr);
    result = Multiplication::Builder(
        Float<double>::Builder(info.value()),
        Expression::Parse(info.unit(), nullptr));
    result.childAtIndex(1).deepReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

bool Constant::derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  ConstantNode::ConstantInfo info = constantInfo();
  if (info.unit() == nullptr && !std::isnan(info.value())) {
    replaceWithInPlace(Rational::Builder(0));
    return true;
  }
  return false;
}

}
