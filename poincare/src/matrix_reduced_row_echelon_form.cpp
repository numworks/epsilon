#include <poincare/matrix_reduced_row_echelon_form.h>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixReducedRowEchelonForm::s_functionHelper;

const char * MatrixReducedRowEchelonFormNode::functionHelperName() const { return MatrixReducedRowEchelonForm::s_functionHelper.name(); }

}
