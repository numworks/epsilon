#ifndef POINCARE_COMPLEX_MATRIX_H
#define POINCARE_COMPLEX_MATRIX_H

#include <poincare/evaluation.h>
#include <assert.h>

namespace Poincare {

template<typename T>
class ComplexMatrix : public Evaluation<T>  {
public:
  ComplexMatrix(const Complex<T> * complexes, int numberOfRows, int numberOfColumns);
  ~ComplexMatrix();
  ComplexMatrix(const ComplexMatrix& other) = delete;
  ComplexMatrix(ComplexMatrix&& other) = delete;
  ComplexMatrix& operator=(const ComplexMatrix& other) = delete;
  ComplexMatrix& operator=(ComplexMatrix&& other) = delete;

  /* Expression */
  Expression::Type type() const override;
  ComplexMatrix<T> * clone() const override;

  // TODO: Remove these 2 functions
  void replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand) override {
    assert(false);
  }
  void swapOperands(int i, int j) override { assert(false); }

  /* Evaluation */
  T toScalar() const override;
  int numberOfRows() const override;
  int numberOfColumns() const override;
  const Complex<T> * complexOperand(int i) const override;
  /* If the buffer is too small, the function fills the buffer until reaching
   * buffer size */
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

  static Evaluation<T> * createIdentity(int dim);
private:
  Evaluation<float> * privateEvaluate(Expression::SinglePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(Expression::DoublePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename U> Evaluation<U> * templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const;
  Complex<T> * m_values;
  int m_numberOfRows;
  int m_numberOfColumns;
};

}

#endif

