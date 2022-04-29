#ifndef POINCARE_STUDENT_CDF_H
#define POINCARE_STUDENT_CDF_H

#include <poincare/approximation_helper.h>
#include <poincare/student_distribution_function.h>

namespace Poincare {

class StudentCDFNode final : public StudentDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(StudentCDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "StudentCDF";
  }
#endif

  // Properties
  Type type() const override { return Type::StudentCDF; }
  Sign sign(Context * context) const override { return Sign::Positive; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class StudentCDF final : public StudentDistributionFunction {
public:
  StudentCDF(const StudentCDFNode * n) : StudentDistributionFunction(n) {}
  static StudentCDF Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<StudentCDF, StudentCDFNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("tcdf", 2, &UntypedBuilderTwoChildren<StudentCDF>);
};

}

#endif
