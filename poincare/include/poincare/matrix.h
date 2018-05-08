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
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  int polynomialDegree(char symbolName) const override;

  /* Operation on matrix */
  /* createDeterminant, createTrace and createInverse can only be called on an
   * matrix of complex expressions. createDeterminant and createTrace return
   * a complex expression and createInverse returns a matrix of complex
   * expressions or nullptr if the inverse could not be computed. */
  template<typename T> Complex<T> * createTrace() const;
  template<typename T> Complex<T> * createDeterminant() const;
  template<typename T> Matrix * createInverse() const;
  Matrix * createTranspose() const;
  static Matrix * createIdentity(int dim);
  template<typename T> static Matrix * createApproximateIdentity(int dim);
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const;
  int m_numberOfRows;
};

}

#endif
