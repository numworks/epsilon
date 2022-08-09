#ifndef POINCARE_BOOLEAN_H
#define POINCARE_BOOLEAN_H

#include <poincare/expression.h>
#include <poincare/aliases_list.h>

namespace Poincare {

// === Evaluation ===

template<typename T>
class BooleanEvaluationNode final : public EvaluationNode<T> {
public:
  BooleanEvaluationNode(bool value) : m_value(value) {}
  std::complex<T> complexAtIndex(int index) const override { return std::complex<T>(NAN, NAN); }
  int numberOfChildren() const override { return 0; }
  bool value() const { return m_value; }

  // TreeNode
  size_t size() const override { return sizeof(BooleanEvaluationNode<T>); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BooleanEvaluation";
  }
  void logAttributes(std::ostream & stream) const override {
    stream << " value=\"" << (m_value ? "True" : "False");
  }
#endif

  // EvaluationNode
  typename EvaluationNode<T>::Type type() const override { return EvaluationNode<T>::Type::BooleanEvaluation; }
  bool isUndefined() const override { return false; }
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;

private:
  bool m_value;
};

template<typename T>
class BooleanEvaluation final : public Evaluation<T> {
public:
  BooleanEvaluation(BooleanEvaluationNode<T> * node) : Evaluation<T>(node) {}
  static BooleanEvaluation Builder(bool value);

  bool value() const { return static_cast<BooleanEvaluationNode<T> *>(TreeHandle::node())->value(); }
};

// === Expression ===

class BooleanNode final : public ExpressionNode  {
public:
  constexpr static AliasesList k_trueAliases = AliasesLists::k_trueAliases;
  constexpr static AliasesList k_falseAliases = AliasesLists::k_falseAliases;

  BooleanNode(bool value) : m_value(value) {}
  // TreeNode
  size_t size() const override { return sizeof(BooleanNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Boolean";
  }
  void logAttributes(std::ostream & stream) const override {
    stream << " value=" << (m_value ? "True" : "False");
  }
#endif

  // Expression Node Properties
  Type type() const override { return Type::Boolean; }

  // Properties
  bool value() const { return m_value; }
  AliasesList aliasesList() const { return m_value ? k_trueAliases : k_falseAliases; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return BooleanEvaluation<float>::Builder(m_value);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return BooleanEvaluation<double>::Builder(m_value);
  }
  bool m_value;
};

class Boolean final : public Expression {
public:
  static Boolean Builder(bool value);
  bool value() const { return static_cast<BooleanNode *>(node())->value(); }
};

}
#endif