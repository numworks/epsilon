#ifndef POINCARE_STUDENT_CDF_RANGE_H
#define POINCARE_STUDENT_CDF_RANGE_H

#include <poincare/approximation_helper.h>
#include <poincare/student_distribution_function.h>

namespace Poincare {

class StudentCDFRangeNode final : public StudentDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(StudentCDFRangeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "StudentCDFRange";
  }
#endif

  // Properties
  Type type() const override { return Type::StudentCDFRange; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  int kIndex() const override { return 2; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class StudentCDFRange final : public StudentDistributionFunction {
public:
  StudentCDFRange(const StudentCDFRangeNode * n) : StudentDistributionFunction(n) {}
  static StudentCDFRange Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<StudentCDFRange, StudentCDFRangeNode>({child0, child1, child2}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("tcdfrange", 3, &UntypedBuilderThreeChildren<StudentCDFRange>);
};

}

#endif
