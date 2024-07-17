#include "matrix_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/memory/tree.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Poincare::Internal;
using namespace Shared;

namespace Calculation {

JuniorLayout CreateBeautifiedLayout(Tree* expression, ProjectionContext* ctx) {
  Simplification::BeautifyReduced(expression, ctx);
  return JuniorLayout::Builder(Layouter::LayoutExpression(
      expression, false,
      Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits(),
      Poincare::Preferences::SharedPreferences()->displayMode()));
}

// TODO_PCJ: Move part of this in Poincare
void MatrixListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasMatrix(approximateOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  ProjectionContext ctx = {
#if 0  // TODO_PCJ: Should this be applied ?
    /* Change complex format to avoid all additional expressions to be
     * "nonreal" (with [i] for instance). As additional results are computed
     * from the output, which is built taking ComplexFormat into account, there
     * are no risks of displaying additional results on an nonreal output. */
    .m_complexFormat = complexFormat() == Preferences::ComplexFormat::Real
                           ? Preferences::ComplexFormat::Cartesian
                           : complexFormat(),
#else
    .m_complexFormat = complexFormat(),
#endif
    .m_angleUnit = angleUnit(),
    .m_symbolic =
        SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
    .m_context = App::app()->localContext()
  };

  // The expression must be reduced to call methods such as determinant or trace
  assert(approximateOutput.tree()->isMatrix());
  Tree* matrix =
      (exactOutput.tree()->isMatrix() ? exactOutput : approximateOutput)
          .tree()
          ->cloneTree();
  Simplification::ProjectAndReduce(matrix, &ctx, false);
  bool mIsSquared = Internal::Matrix::NumberOfRows(matrix) ==
                    Internal::Matrix::NumberOfColumns(matrix);
  size_t index = 0;
  size_t messageIndex = 0;

  // 1. Matrix determinant if square matrix
  if (mIsSquared) {
    /* Determinant is reduced so that a null determinant can be detected.
     * However, some exceptions remain such as cos(x)^2+sin(x)^2-1 which will
     * not be reduced to a rational, but will be null in theory. */
    Tree* determinant;
    Tree* matrixClone = matrix->cloneTree();
    Internal::Matrix::RowCanonize(matrixClone, true, &determinant, false);
    // determinant has
    bool determinantIsUndefinedOrNull =
        determinant->isUndefined() || determinant->isZero();

    m_indexMessageMap[index] = messageIndex++;
    m_layouts[index++] = CreateBeautifiedLayout(determinant, &ctx);
    matrixClone->removeTree();

    /* 2. Matrix inverse if invertible matrix
     * A squared matrix is invertible if and only if determinant is non null */
    if (!determinantIsUndefinedOrNull) {
      // TODO: Handle a determinant that can be null.
      Tree* inverse = Internal::Matrix::Inverse(matrix, false);
      m_indexMessageMap[index] = messageIndex++;
      m_layouts[index++] = CreateBeautifiedLayout(inverse, &ctx);
    }
  }

  // 3. Matrix row echelon form
  messageIndex = 2;
  Tree* reducedRowEchelonForm = matrix->cloneTree();
  Internal::Matrix::RowCanonize(reducedRowEchelonForm, false, nullptr, false);
  // preserve reducedRowEchelonForm for next step.
  Tree* rowEchelonForm = reducedRowEchelonForm->cloneTree();
  m_indexMessageMap[index] = messageIndex++;
  m_layouts[index++] = CreateBeautifiedLayout(rowEchelonForm, &ctx);

  /* 4. Matrix reduced row echelon form
   *    it can be computed from row echelon form to save computation time.*/
  Internal::Matrix::RowCanonize(reducedRowEchelonForm, true, nullptr, false);
  m_indexMessageMap[index] = messageIndex++;
  m_layouts[index++] = CreateBeautifiedLayout(reducedRowEchelonForm, &ctx);
  // 5. Matrix trace if square matrix
  if (mIsSquared) {
    m_indexMessageMap[index] = messageIndex++;
    m_layouts[index++] =
        CreateBeautifiedLayout(Internal::Matrix::Trace(matrix), &ctx);
  }
  matrix->removeTree();
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
