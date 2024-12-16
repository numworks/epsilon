#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include "array.h"
#include "old_expression.h"

namespace Poincare {

class MatrixNode final : public Array, public ExpressionNode {
 public:
  MatrixNode() : Array() {}

  // PoolObject
  size_t size() const override { return sizeof(MatrixNode); }
  int numberOfChildren() const override {
    return m_numberOfRows * m_numberOfColumns;
  }
  void didChangeArity(int newNumberOfChildren) override {
    return Array::didChangeNumberOfChildren(newNumberOfChildren);
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "OMatrix"; }
  void logAttributes(std::ostream& stream) const override {
    stream << " rows=\"" << m_numberOfRows << "\"";
    stream << " columns=\"" << m_numberOfColumns << "\"";
  }
#endif

  // Properties
  Type otype() const override { return Type::OMatrix; }

  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override;
};

class OMatrix final : public OExpression {
  template <typename T>
  friend class MatrixComplexNode;
  friend class GlobalContext;

 public:
  /* Cap the matrix's size for inverse and determinant computation.
   * TODO: Find another solution */
  constexpr static int k_maxNumberOfChildren = 100;

  OMatrix(const MatrixNode* node) : OExpression(node) {}
  static OMatrix Builder() {
    return PoolHandle::NAryBuilder<OMatrix, MatrixNode>();
  }

  void setDimensions(int rows, int columns);
  Array::VectorType vectorType() const { return node()->vectorType(); }
  bool isVector() const { return node()->isVector(); }
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  using PoolHandle::addChildAtIndexInPlace;
  using PoolHandle::removeChildAtIndexInPlace;
  void addChildrenAsRowInPlace(PoolHandle t, int i);
  OExpression matrixChild(int i, int j) {
    return childAtIndex(i * numberOfColumns() + j);
  }

  /* Operation on matrix */

  // rank returns -1 if the rank cannot be computed
  int rank(Context* context, bool forceCanonization = false);
  OExpression createTrace();
  static OMatrix CreateIdentity(int dim);
  OMatrix createTranspose() const;
  OExpression createRef(const ReductionContext& reductionContext,
                        bool* couldComputeRef, bool reduced) const;
  /* createInverse can be called on any matrix, reduced or not, approximated or
   * not. */
  OExpression createInverse(const ReductionContext& reductionContext,
                            bool* couldComputeInverse) const;
  OExpression determinant(const ReductionContext& reductionContext,
                          bool* couldComputeDeterminant, bool inPlace);
  OExpression norm(const ReductionContext& reductionContext) const;
  OExpression dot(OMatrix* b, const ReductionContext& reductionContext) const;
  OMatrix cross(OMatrix* b, const ReductionContext& reductionContext) const;

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);

 private:
  MatrixNode* node() const {
    return static_cast<MatrixNode*>(OExpression::node());
  }
  void setNumberOfRows(int rows) { node()->setNumberOfRows(rows); }
  void setNumberOfColumns(int columns) { node()->setNumberOfColumns(columns); }
  OExpression computeInverseOrDeterminant(
      bool computeDeterminant, const ReductionContext& reductionContext,
      bool* couldCompute) const;
  /* rowCanonize turns a matrix in its row echelon form, reduced or not.
   *
   * If the matrix contains unresolved symbols, the canonization cannot
   * be properly done. The method will interrupt and set canonizationSuccess to
   * false.
   *
   *  WARNING: If forceCanonization is set to true though, the canonization will
   * be done anyway. This is very risky since the canonization behaviour is
   * undef for unknown value. For example, the matrix [[3, 0][x, 5]] does not
   * canonize the same if x=2 or x=6.
   * This is used only by the solver when calling rank method to be able to
   * solve equation systems. It works in this case because reduced is true and
   * we know the matrix that is canonized is of the form:
   * [ ... ...  0   0   0  ]
   * [ ... ...  0   0   0  ]
   * [ ... ...  0   0   0  ]
   * [  1   0  ...  0   t1 ]
   * [  0 ... 1 ... 0   tk ]
   * [  0   0  ...  1   tN ]
   * so the 1's of the last rows will always be taken as pivots, and not the tk.
   * */
  bool isCanonizable(const ReductionContext& reductionContext);
  OMatrix rowCanonize(const ReductionContext& reductionContext,
                      bool* canonizationSuccess, OExpression* determinant,
                      bool reduced = true, bool forceCanonization = false);
};

}  // namespace Poincare

#endif
