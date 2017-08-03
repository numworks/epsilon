#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Subtraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
  static Complex compute(const Complex c, const Complex d);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation * computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * m) const override;
  Complex privateCompute(const Complex c, const Complex d) const override {
    return compute(c, d);
  }
};

}

#endif
