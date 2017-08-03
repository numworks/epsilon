#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Power : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
  static Complex compute(const Complex c, const Complex d);
private:
  constexpr static float k_maxNumberOfSteps = 10000.0f;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Complex privateCompute(const Complex c, const Complex d) const override {
    return compute(c, d);
  }
  Evaluation * computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * n) const override;
  Evaluation * computeOnComplexMatrixAndComplex(Evaluation * m, const Complex * d) const override;
  Evaluation * computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const override;
};

}

#endif
