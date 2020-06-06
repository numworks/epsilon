#ifndef POINCARE_MODULO_H
#define POINCARE_MODULO_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>
#include <poincare/evaluation.h>
#include <poincare/helpers.h>

namespace Poincare {

class Power;

class ModuloNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ModuloNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Modulo";
  }
#endif

  // Properties
  Type type() const override { return Type::Modulo; }

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;

  // Aproximation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

private:
  template<typename T> Complex<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;  
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
};

class Modulo final : public Expression {
  friend class ModuloNode;
public:
  Modulo(const ModuloNode * n) : Expression(n) {}
  static Modulo Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Modulo, ModuloNode>({child0, child1}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif

