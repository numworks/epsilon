#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include "approximation_helper.h"
#include "junior_expression.h"
#include "number.h"

namespace Poincare {

/* Float represents an approximated number. This class is use to avoid turning
 * float/double into Decimal back and forth because performances are
 * dramatically affected when doing so. For instance, when plotting a graph, we
 * need to set a float/double value for a symbol and approximate an expression
 * containing the symbol for each dot displayed).
 * We thus use the Float class that hold a float/double.
 * Float can only be approximated ; Float is an INTERNAL node only. Indeed,
 * they are always turned back into Decimal when beautifying. Thus, the usual
 * methods of expression are not implemented to avoid code duplication with
 * Decimal. */

template <typename T>
class FloatNode final : public NumberNode {
 public:
  FloatNode(T value = static_cast<T>(0.0)) : m_value(value) {}

  T value() const { return m_value; }
  void setValue(T value) { m_value = value; }

  // PoolObject
  size_t size() const override { return sizeof(FloatNode<T>); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Float"; }
  void logAttributes(std::ostream& stream) const override {
    stream << " value=\"" << m_value << "\" type=\""
           << (sizeof(T) == sizeof(float) ? "float" : "double") << "\"";
  }
#endif

  // Properties
  Type otype() const override {
    return (sizeof(T) == sizeof(float)) ? Type::Float : Type::Double;
  }
  OMG::Troolean isPositive(Context* context) const override {
    return std::isnan(m_value)
               ? OMG::Troolean::Unknown
               : OMG::BoolToTroolean(m_value >= static_cast<T>(0.0));
  }

  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

  // NumberNode
  void setNegative(bool negative) override {
    if ((m_value < static_cast<T>(0.0)) != negative) {
      m_value = static_cast<T>(-1.0) * m_value;
    }
  }
  bool isZero() const override { return m_value == static_cast<T>(0.0); }
  bool isOne() const override { return m_value == static_cast<T>(1.0); }
  bool isMinusOne() const override { return m_value == static_cast<T>(-1.0); }
  bool isInteger() const override { return m_value == std::floor(m_value); }
  Integer integerValue() const override {
    assert(isInteger());
    return Integer(static_cast<double_native_int_t>(std::floor(m_value)));
  }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

 private:
  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::Decimal; }

  T m_value;
};

template <typename T>
class Float final : public Number {
 public:
  static Float Builder(T value);
  T value() const { return node()->value(); }
  void setValue(T value) { node()->setValue(value); }

 private:
  FloatNode<T>* node() const {
    return static_cast<FloatNode<T>*>(Number::node());
  }
};

}  // namespace Poincare

#endif
