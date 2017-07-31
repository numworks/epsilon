#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Addition : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
      int numnerOfOperands, bool cloneOperands = true) const override;
  bool isCommutative() const override;
  static Complex compute(const Complex c, const Complex d);
  static Evaluation * computeOnMatrices(Evaluation * m, Evaluation * n);
  static Evaluation * computeOnComplexAndMatrix(const Complex * c, Evaluation * m);
private:
  Complex privateCompute(const Complex c, const Complex d) const override {
    return compute(c, d);
  }
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif
