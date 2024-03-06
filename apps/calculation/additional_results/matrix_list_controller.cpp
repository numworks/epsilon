#include "matrix_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/determinant.h>
#include <poincare/matrix.h>
#include <poincare/matrix_inverse.h>
#include <poincare/matrix_reduced_row_echelon_form.h>
#include <poincare/matrix_row_echelon_form.h>
#include <poincare/matrix_trace.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void MatrixListController::computeAdditionalResults(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  assert(AdditionalResultsType::HasMatrix(approximateOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Context *context = App::app()->localContext();
  /* Change complex format to avoid all additional expressions to be
   * "nonreal" (with [i] for instance). As additional results are computed
   * from the output, which is built taking ComplexFormat into account, there
   * are no risks of displaying additional results on an nonreal output. */
  ComputationContext computationContext(
      context,
      complexFormat() == Preferences::ComplexFormat::Real
          ? Preferences::ComplexFormat::Cartesian
          : complexFormat(),
      angleUnit());

  // The expression must be reduced to call methods such as determinant or trace
  assert(approximateOutput.type() == ExpressionNode::Type::Matrix);
  Expression clone = exactOutput.type() == ExpressionNode::Type::Matrix
                         ? exactOutput.clone()
                         : approximateOutput.clone();
  Matrix matrix = static_cast<const Matrix &>(clone);

  bool mIsSquared = matrix.numberOfRows() == matrix.numberOfColumns();
  size_t index = 0;
  size_t messageIndex = 0;
  // 1. Matrix determinant if square matrix
  if (mIsSquared) {
    /* Determinant is reduced so that a null determinant can be detected.
     * However, some exceptions remain such as cos(x)^2+sin(x)^2-1 which will
     * not be reduced to a rational, but will be null in theory. */
    Expression determinant = Determinant::Builder(matrix);
    PoincareHelpers::CloneAndSimplify(
        &determinant, context,
        {.complexFormat = complexFormat(),
         .angleUnit = angleUnit(),
         .target = ReductionTarget::SystemForApproximation,
         .symbolicComputation =
             SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined});
    m_indexMessageMap[index] = messageIndex++;
    m_layouts[index++] =
        getExactLayoutFromExpression(determinant, computationContext);
    /* 2. Matrix inverse if invertible matrix
     * A squared matrix is invertible if and only if determinant is non null */
    if (!determinant.isUndefined() &&
        determinant.isNull(context) != TrinaryBoolean::True) {
      // TODO: Handle ExpressionNode::NullStatus::Unknown
      m_indexMessageMap[index] = messageIndex++;
      m_layouts[index++] = getExactLayoutFromExpression(
          MatrixInverse::Builder(matrix), computationContext);
    }
  }
  // 3. Matrix row echelon form
  messageIndex = 2;
  Expression rowEchelonForm = MatrixRowEchelonForm::Builder(matrix);
  m_indexMessageMap[index] = messageIndex++;
  m_layouts[index++] =
      getExactLayoutFromExpression(rowEchelonForm, computationContext);
  /* 4. Matrix reduced row echelon form
   *    it can be computed from row echelon form to save computation time.*/
  m_indexMessageMap[index] = messageIndex++;
  m_layouts[index++] = getExactLayoutFromExpression(
      MatrixReducedRowEchelonForm::Builder(rowEchelonForm), computationContext);
  // 5. Matrix trace if square matrix
  if (mIsSquared) {
    m_indexMessageMap[index] = messageIndex++;
    m_layouts[index++] = getExactLayoutFromExpression(
        MatrixTrace::Builder(matrix), computationContext);
  }
}

I18n::Message MatrixListController::messageAtIndex(int index) {
  // Message index is mapped in setExpression because it depends on the Matrix.
  assert(index < k_maxNumberOfOutputRows && index >= 0);
  I18n::Message messages[k_maxNumberOfOutputRows] = {
      I18n::Message::AdditionalDeterminant, I18n::Message::AdditionalInverse,
      I18n::Message::AdditionalRowEchelonForm,
      I18n::Message::AdditionalReducedRowEchelonForm,
      I18n::Message::AdditionalTrace};
  return messages[m_indexMessageMap[index]];
}

}  // namespace Calculation
