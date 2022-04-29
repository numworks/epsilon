#ifndef POINCARE_INV_STUDENT_H
#define POINCARE_INV_STUDENT_H

#include <poincare/approximation_helper.h>
#include <poincare/student_distribution_function.h>

namespace Poincare {

class InvStudentNode final : public StudentDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(InvStudentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "InvStudent";
  }
#endif

  // Properties
  Type type() const override { return Type::InvStudent; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class InvStudent final : public StudentDistributionFunction {
public:
  InvStudent(const InvStudentNode * n) : StudentDistributionFunction(n) {}
  static InvStudent Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<InvStudent, InvStudentNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("invt", 2, &UntypedBuilderTwoChildren<InvStudent>);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
