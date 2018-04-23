#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/matrix_data.h>

namespace Poincare {

class Matrix : public DynamicHierarchy {
friend class GlobalContext;
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

  Matrix * createTranspose() const;
  static Matrix * createIdentity(int dim);
private:
  /* Layout */
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedApproximate(Context& context, AngleUnit angleUnit) const;
  int m_numberOfRows;
};

}

#endif
