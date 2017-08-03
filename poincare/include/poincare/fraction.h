#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Fraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
  static Complex compute(const Complex c, const Complex d);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation * computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * n) const override;
  Evaluation * computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const override;
  Complex privateCompute(const Complex c, const Complex d) const override {
    return compute(c, d);
  }
};

}

#endif
