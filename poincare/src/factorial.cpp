#include <poincare/factorial.h>
#include <poincare/constant.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/symbol.h>
#include <poincare/simplification_helper.h>
#include <poincare/parenthesis.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

// Property

Expression FactorialNode::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  assert(s == Sign::Positive);
  return Factorial(this);
}

// Layout

bool FactorialNode::childNeedsParenthesis(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative) {
    return true;
  }
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->denominator().isOne()) {
    return true;
  }
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Addition, Type::Power};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 6);
}

// Simplification

Expression FactorialNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Factorial(this).shallowReduce();
}

Expression FactorialNode::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Factorial(this).shallowBeautify();
}

template<typename T>
Complex<T> FactorialNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  T n = c.real();
  if (c.imag() != 0 || std::isnan(n) || n != (int)n || n < 0) {
    return Complex<T>::Undefined();
  }
  T result = 1;
  for (int i = 1; i <= (int)n; i++) {
    result *= (T)i;
    if (std::isinf(result)) {
      return Complex<T>::Builder(result);
    }
  }
  return Complex<T>::Builder(std::round(result));
}

Layout FactorialNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  int childrenCount = result.numberOfChildren();
  result.addChildAtIndex(CharLayout::Builder('!'), childrenCount, childrenCount, nullptr);
  return result;
}

int FactorialNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (childNeedsParenthesis(childAtIndex(0))) {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  numberOfChar += childAtIndex(0)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (childNeedsParenthesis(childAtIndex(0))) {
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  buffer[numberOfChar++] = '!';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

Factorial Factorial::Builder(Expression child) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(FactorialNode));
  FactorialNode * node = new (bufferNode) FactorialNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, &child, 1);
  return static_cast<Factorial &>(h);
}

Expression Factorial::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    Rational r = childAtIndex(0).convert<Rational>();
    if (!r.integerDenominator().isOne() || r.sign() == ExpressionNode::Sign::Negative) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
    }
    if (Integer(k_maxOperandValue).isLowerThan(r.unsignedIntegerNumerator())) {
      return *this;
    }
    Rational fact = Rational::Builder(Integer::Factorial(r.unsignedIntegerNumerator()));
    assert(!fact.numeratorOrDenominatorIsInfinity()); // because fact < k_maxOperandValue!
    replaceWithInPlace(fact);
    return fact;
  }
  if (childAtIndex(0).type() == ExpressionNode::Type::Constant) {
    // e! = undef, i! = undef, pi! = undef
    Expression result = Undefined::Builder();
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

Expression Factorial::shallowBeautify() {
  // +(a,b)! ->(+(a,b))!
  if (childAtIndex(0).type() == ExpressionNode::Type::Addition
      || childAtIndex(0).type() == ExpressionNode::Type::Multiplication
      || childAtIndex(0).type() == ExpressionNode::Type::Power)
  {
    Expression result = Factorial::Builder(Parenthesis::Builder(childAtIndex(0)));
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

#if 0
int Factorial::simplificationOrderGreaterType(const Expression * e) const {
  if (SimplificationOrder(childAtIndex(0),e) == 0) {
    return 1;
  }
  return SimplificationOrder(childAtIndex(0), e);
}

int Factorial::simplificationOrderSameType(const Expression * e) const {
  return SimplificationOrder(childAtIndex(0), e->childAtIndex(0));
}
#endif

}
