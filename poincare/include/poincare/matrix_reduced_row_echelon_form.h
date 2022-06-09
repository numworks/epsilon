#ifndef POINCARE_MATRIX_REDUCED_ROW_ECHELON_FORM_H
#define POINCARE_MATRIX_REDUCED_ROW_ECHELON_FORM_H

#include <poincare/matrix_echelon_form.h>

namespace Poincare {

class MatrixReducedRowEchelonFormNode final : public MatrixEchelonFormNode {
public:
  static constexpr char functionName[] = "rref";

  // TreeNode
  size_t size() const override { return sizeof(MatrixReducedRowEchelonFormNode); }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixReducedRowEchelonForm";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixReducedRowEchelonForm; }
private:
  const char * functionHelperName() const override;
  bool isFormReduced() const override { return true; }
};

class MatrixReducedRowEchelonForm final : public HandleOneChild<MatrixReducedRowEchelonForm, MatrixReducedRowEchelonFormNode, MatrixEchelonForm> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
};

}

#endif
