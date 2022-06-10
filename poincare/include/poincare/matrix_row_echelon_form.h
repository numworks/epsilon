#ifndef POINCARE_MATRIX_ROW_ECHELON_FORM_H
#define POINCARE_MATRIX_ROW_ECHELON_FORM_H

#include <poincare/matrix_echelon_form.h>

namespace Poincare {

class MatrixRowEchelonFormNode final : public MatrixEchelonFormNode {
public:
  static constexpr char k_functionName[] = "ref";

  // TreeNode
  size_t size() const override { return sizeof(MatrixRowEchelonFormNode); }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixRowEchelonForm";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixRowEchelonForm; }
private:
  const char * functionHelperName() const override;
  bool isFormReduced() const override { return false; }
};

class MatrixRowEchelonForm final : public ExpressionOneChild<MatrixRowEchelonForm, MatrixRowEchelonFormNode, MatrixEchelonForm> {
public:
  using ExpressionBuilder::ExpressionBuilder;
};

}

#endif
