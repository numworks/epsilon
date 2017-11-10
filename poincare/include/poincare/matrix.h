#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/matrix_data.h>

namespace Poincare {

class Matrix : public DynamicHierarchy {
public:
  Matrix(MatrixData * matrixData); // pilfer the operands of matrixData
  Matrix(const Expression * const * operands, int numberOfRows, int numberOfColumns, bool cloneOperands = true);
  int numberOfRows() const;
  int numberOfColumns() const;

  /* Expression */
  Type type() const override;
  Expression * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

  /* Operation on matrix */
  /* createDeterminant and createInverse can only be called on an approximate
   * matrix. Both methods assert that all their operands are complex.
   * createDeterminant returns a decimal expression (or an undefined expression)
   * and createInverse returns a matrix of decimal expression (or an undefined
   * expression). */
  Expression * createDeterminant() const;
  Expression * createInverse() const;

  Matrix * createTranspose() const;
  static Matrix * createIdentity(int dim);
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  /* Evaluation */
  Complex<float> * privateEvaluate(Expression::SinglePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { assert(false); return nullptr; }
  Complex<double> * privateEvaluate(Expression::DoublePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { assert(false); return nullptr; }
  int m_numberOfRows;
};

}

#endif
