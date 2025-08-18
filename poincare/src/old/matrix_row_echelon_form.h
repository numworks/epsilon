#ifndef POINCARE_MATRIX_ROW_ECHELON_FORM_H
#define POINCARE_MATRIX_ROW_ECHELON_FORM_H

#include "matrix_echelon_form.h"

namespace Poincare {

class MatrixRowEchelonFormNode final : public MatrixEchelonFormNode {
 public:
  constexpr static AliasesList k_functionName = "ref";

  // PoolObject
  size_t size() const override { return sizeof(MatrixRowEchelonFormNode); }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "MatrixRowEchelonForm";
  }
#endif

  // Properties
  Type otype() const override { return Type::MatrixRowEchelonForm; }

 private:
  const char* functionHelperName() const override;
  bool isFormReduced() const override { return false; }
};

class MatrixRowEchelonForm final
    : public ExpressionOneChild<MatrixRowEchelonForm, MatrixRowEchelonFormNode,
                                MatrixEchelonForm> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
