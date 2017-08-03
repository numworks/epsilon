#ifndef POINCARE_COMPLEX_MATRIX_H
#define POINCARE_COMPLEX_MATRIX_H

#include <poincare/evaluation.h>

namespace Poincare {

class ComplexMatrix : public Evaluation {
public:
  ComplexMatrix(const Complex * complexes, int numberOfRows, int numberOfColumns);
  ~ComplexMatrix();
  ComplexMatrix(const ComplexMatrix& other) = delete;
  ComplexMatrix(ComplexMatrix&& other) = delete;
  ComplexMatrix& operator=(const ComplexMatrix& other) = delete;
  ComplexMatrix& operator=(ComplexMatrix&& other) = delete;
  float toFloat() const override;
  const Complex * complexOperand(int i) const override;
  int numberOfRows() const override;
  int numberOfColumns() const override;
  ComplexMatrix * clone() const override;
  ComplexMatrix * cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  static Evaluation * createIdentity(int dim);
private:
  Complex * m_values;
  int m_numberOfRows;
  int m_numberOfColumns;
};

}

#endif

