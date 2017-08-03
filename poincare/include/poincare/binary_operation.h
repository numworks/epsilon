#ifndef POINCARE_BINARY_OPERATION_H
#define POINCARE_BINARY_OPERATION_H

#include <poincare/expression.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/complex_matrix.h>

namespace Poincare {

class BinaryOperation : public Expression {
public:
  BinaryOperation();
  BinaryOperation(Expression ** operands, bool cloneOperands = true);
  ~BinaryOperation();
  BinaryOperation(const BinaryOperation& other) = delete;
  BinaryOperation(BinaryOperation&& other) = delete;
  BinaryOperation& operator=(const BinaryOperation& other) = delete;
  BinaryOperation& operator=(BinaryOperation&& other) = delete;
  bool hasValidNumberOfArguments() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
protected:
  Expression * m_operands[2];
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  virtual Evaluation * computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * n) const;
  virtual Evaluation * computeOnComplexMatrixAndComplex(Evaluation * m, const Complex * d) const;
  virtual Evaluation * computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const;
  Evaluation * computeOnComplexes(const Complex * c, const Complex * d) const {
    return new Complex(privateCompute(*c, *d));
  }
  virtual Complex privateCompute(const Complex c, const Complex d) const = 0;
};

}

#endif
