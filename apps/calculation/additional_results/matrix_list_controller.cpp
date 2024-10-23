#include "matrix_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/additional_results_helper.h>
#include <poincare/src/expression/projection.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void MatrixListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasMatrix(approximateOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Internal::ProjectionContext ctx = {
      .m_complexFormat = complexFormat(),
      .m_angleUnit = angleUnit(),
      .m_symbolic =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      .m_context = App::app()->localContext(),
      .m_advanceReduce = false};

  // Compute all layouts at once to reuse intermediate results
  Layout determinant, inverse, rowEchelonForm, reducedRowEchelonForm, trace;
  AdditionalResultsHelper::ComputeMatrixProperties(
      exactOutput, approximateOutput, ctx, determinant, inverse, rowEchelonForm,
      reducedRowEchelonForm, trace);

  size_t index = 0;
  // Determinant is only computed on squared matrices
  bool isSquared = !determinant.isUninitialized();
  if (isSquared) {
    m_message[index] = I18n::Message::AdditionalDeterminant;
    m_layouts[index++] = determinant;
    if (!inverse.isUninitialized()) {
      m_message[index] = I18n::Message::AdditionalInverse;
      m_layouts[index++] = inverse;
    }
  } else {
    assert(inverse.isUninitialized());
  }
  assert(!rowEchelonForm.isUninitialized() &&
         !reducedRowEchelonForm.isUninitialized());
  m_message[index] = I18n::Message::AdditionalRowEchelonForm;
  m_layouts[index++] = rowEchelonForm;
  m_message[index] = I18n::Message::AdditionalReducedRowEchelonForm;
  m_layouts[index++] = reducedRowEchelonForm;

  if (isSquared) {
    assert(!trace.isUninitialized());
    m_message[index] = I18n::Message::AdditionalTrace;
    m_layouts[index++] = trace;
  }
}

}  // namespace Calculation
