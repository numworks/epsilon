#ifndef POINCARE_STUDENT_DISTRIBUTION_FUNCTION_H
#define POINCARE_STUDENT_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>

namespace Poincare {

/* StudentDistributionFunctions are StudentCDF, StudentCDFRange, InvStudent and
 * StudentPDF */

class StudentDistributionFunctionNode : public ExpressionNode {
public:
  virtual int kIndex() const { return 1; }
private:
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class StudentDistributionFunction : public Expression {
public:
  StudentDistributionFunction(const StudentDistributionFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(Context * context, bool * stopReduction = nullptr);
private:
  int kIndex() const { return node()->kIndex(); }
  StudentDistributionFunctionNode * node() const { return static_cast<StudentDistributionFunctionNode *>(Expression::node()); }
};

}

#endif
