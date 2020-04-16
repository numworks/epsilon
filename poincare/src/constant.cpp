#include <poincare/constant.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/unreal.h>
#include <poincare/unreal.h>
#include <ion.h>
#include <cmath>
#include <assert.h>
#include <ion/unicode/utf8_decoder.h>

namespace Poincare {

ConstantNode::ConstantNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

ExpressionNode::Sign ConstantNode::sign(Context * context) const {
  if (isPi() || isExponential()) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

bool ConstantNode::isReal() const {
  return !isIComplex();
}

int rankOfConstant(CodePoint c) {
  switch (c) {
    case UCodePointMathematicalBoldSmallI :
      return 0;
    case UCodePointGreekSmallLetterPi :
      return 1;
    case UCodePointScriptSmallE :
      return 2;
    default:
      assert(false);
      return -1;
  }
}

CodePoint ConstantNode::codePoint() const {
  UTF8Decoder decoder = UTF8Decoder(m_name);
  CodePoint result = decoder.nextCodePoint();
  assert(decoder.nextCodePoint() == UCodePointNull);
  return result;
}

int ConstantNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, canBeInterrupted, ignoreParentheses);
  }
  assert(type() == e->type());
  return rankOfConstant(codePoint()) - rankOfConstant(static_cast<const ConstantNode *>(e)->codePoint());
}

Layout ConstantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::String(m_name, strlen(m_name));
}

template<typename T>
Evaluation<T> ConstantNode::templatedApproximate() const {
  if (isIComplex()) {
    return Complex<T>::Builder(0.0, 1.0);
  }
  if (isPi()) {
    return Complex<T>::Builder(M_PI);
  }
  assert(isExponential());
  return Complex<T>::Builder(M_E);
}

Expression ConstantNode::shallowReduce(ReductionContext reductionContext) {
  return Constant(this).shallowReduce(reductionContext);
}

bool ConstantNode::isConstantCodePoint(CodePoint c) const {
  UTF8Decoder decoder(m_name);
  bool result = (decoder.nextCodePoint() == c);
  assert(decoder.nextCodePoint() == UCodePointNull);
  return result;
}

Constant Constant::Builder(CodePoint c) {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  size_t codePointSize = SerializationHelper::CodePoint(buffer, bufferSize, c);
  return SymbolAbstract::Builder<Constant, ConstantNode>(buffer, codePointSize);
}

Expression Constant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression result;
  bool isI = isIComplex();
  if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real && isI) {
    result = Unreal::Builder();
  } else if (reductionContext.target() == ExpressionNode::ReductionTarget::User && isI) {
    result = ComplexCartesian::Builder(Rational::Builder(0), Rational::Builder(1));
  } else {
    return *this;
  }
  replaceWithInPlace(result);
  return result;
}

}
