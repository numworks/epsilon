#include <poincare/based_integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

#include <assert.h>
#include <math.h>
#include <utility>
#include <stdlib.h>
#include <string.h>

namespace Poincare {

/* Based integer Node */

BasedIntegerNode::BasedIntegerNode(const native_uint_t * digits, uint8_t size, OMG::Base base) :
  NumberNode(),
  m_base(base),
  m_numberOfDigits(size)
{
  if (digits) {
    memcpy(m_digits, digits, size*sizeof(native_uint_t));
  }
}

Integer BasedIntegerNode::integer() const {
  return Integer::BuildInteger((native_uint_t *)m_digits, m_numberOfDigits, false);
}

// Tree Node

static size_t BasedIntegerSize(uint8_t numberOfDigits) {
  uint8_t realDigitsSize = numberOfDigits > Integer::k_maxNumberOfDigits ? 0 : numberOfDigits;
  return sizeof(BasedIntegerNode) + sizeof(native_uint_t)*realDigitsSize;
}

size_t BasedIntegerNode::size() const {
  return BasedIntegerSize(m_numberOfDigits);
}

// Serialization Node

#if POINCARE_TREE_LOG
void BasedIntegerNode::logAttributes(std::ostream & stream) const {
  stream << " base=\"" << (int)m_base << "\"";
  stream << " integer=\"";
  this->integer().logInteger(stream);
  stream << "\"";
}
#endif

int BasedIntegerNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return integer().serialize(buffer, bufferSize, m_base);
}

// Layout

Layout BasedIntegerNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return integer().createLayout(m_base);
}

// Approximation

template<typename T> T BasedIntegerNode::templatedApproximate() const {
  return integer().approximate<T>();
}

// Comparison

int BasedIntegerNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(e->type() == ExpressionNode::Type::BasedInteger);
  const BasedIntegerNode * other = static_cast<const BasedIntegerNode *>(e);
  return Integer::NaturalOrder(integer(), other->integer());
}

Expression BasedIntegerNode::shallowReduce(const ReductionContext& reductionContext) {
  return BasedInteger(this).shallowReduce();
}

/* BasedInteger  */

// Constructors

BasedInteger BasedInteger::Builder(const char * digits, size_t length, OMG::Base base) {
  Integer m(digits, length, false, base);
  return Builder(m, base);
}

BasedInteger BasedInteger::Builder(const Integer & m, OMG::Base base) {
  void * bufferNode = TreePool::sharedPool->alloc(BasedIntegerSize(m.numberOfDigits()));
  BasedIntegerNode * node = new (bufferNode) BasedIntegerNode(m.digits(), m.numberOfDigits(), base);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<BasedInteger &>(h);
}

Expression BasedInteger::shallowReduce() {
  // Turn BasedInteger into Rational
  Integer numerator = node()->integer();
  Integer denominator(1);
  Expression result;
  if (numerator.isOverflow() || denominator.isOverflow()) {
    result = Number::FloatNumber(node()->integer().template approximate<double>());
  } else {
    result = Rational::Builder(numerator, denominator);
  }
  replaceWithInPlace(result);
  return result;
}

template float BasedIntegerNode::templatedApproximate() const;
template double BasedIntegerNode::templatedApproximate() const;

}
