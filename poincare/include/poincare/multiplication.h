#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Multiplication : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
      int numnerOfOperands, bool cloneOperands = true) const override;
  static Evaluation * computeOnMatrices(Evaluation * m, Evaluation * n);
  static Evaluation * computeOnComplexAndMatrix(const Complex * c, Evaluation * m);
  static Complex compute(const Complex c, const Complex d);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation * computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const override {
    return computeOnMatrices(m, n);
  }
  Complex privateCompute(const Complex c, const Complex d) const override {
    return compute(c, d);
  }
};

}

#endif
