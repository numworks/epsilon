#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/matrix_data.h>

namespace Poincare {

class Multiplication;

class Matrix : public DynamicHierarchy {
public:
  Matrix(MatrixData * matrixData); // pilfer the operands of matrixData
  Matrix(const Expression * const * operands, int numberOfRows, int numberOfColumns, bool cloneOperands = true);
  Expression * matrixOperand(int i, int j) { return editableOperand(i*numberOfColumns()+j); }
  int numberOfRows() const;
  int numberOfColumns() const;

  /* Expression */
  Type type() const override;
  Expression * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  int polynomialDegree(char symbolName) const override;

  /* Operation on matrix */
  int rank(Context & context, AngleUnit angleUnit, bool inPlace);
  /* createInverse can be called on any matrix reduce or not, approximate or not. */
  Expression * createInverse(Context & context, AngleUnit angleUnit) const;
  /* createDeterminant and createTrace can only be called on a matrix of complex
   * expressions. createDeterminant and createTrace return a complex
   * expression. */
  template<typename T> Complex<T> * createTrace() const;
  template<typename T> Complex<T> * createDeterminant(Context & context, AngleUnit angleUnit) const;
  /* createApproximateInverse has to be called on a matrix of complex and will
   * return a matrix of complex if possible and nullptr otherwise. */
  template<typename T> Matrix * createApproximateInverse() const;
  Matrix * createTranspose() const;
  static Matrix * createIdentity(int dim);
  template<typename T> static Matrix * createApproximateIdentity(int dim);
private:
  /* rowCanonize turns a matrix in its reduced row echelon form. */
  void rowCanonize(Context & context, AngleUnit angleUnit, Multiplication * m = nullptr);
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
