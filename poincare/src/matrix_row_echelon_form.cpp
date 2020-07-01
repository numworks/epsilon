#include <poincare/matrix_row_echelon_form.h>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixRowEchelonForm::s_functionHelper;

const char * MatrixRowEchelonFormNode::functionHelperName() const { return MatrixRowEchelonForm::s_functionHelper.name(); }

}
