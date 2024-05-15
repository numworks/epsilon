#include "additional_results_controller.h"

#include <poincare/old/circuit_breaker_checkpoint.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void AdditionalResultsController::openAdditionalResults(
    AdditionalResultsType type, const UserExpression input,
    const UserExpression exactOutput, const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences) {
  // Head controller
  /* TODO: Refactor to avoid writing an if for each parent * child. */
  ExpressionsListController *mainController = nullptr;
  if (type.complex || type.unit || type.vector || type.matrix ||
      type.directTrigonometry || type.inverseTrigonometry || type.function) {
    m_unionController.~UnionController();
    mainController = m_unionController.listController();
    if (type.complex) {
      new (&m_unionController)
          ComplexListController(m_editExpressionController);
    } else if (type.directTrigonometry || type.inverseTrigonometry) {
      new (&m_unionController)
          TrigonometryListController(m_editExpressionController);
      m_unionController.m_trigonometryController.setTrigonometryType(
          type.directTrigonometry);
    } else if (type.unit) {
      new (&m_unionController) UnitListController(m_editExpressionController);
    } else if (type.vector) {
      new (&m_unionController) VectorListController(m_editExpressionController);
    } else if (type.matrix) {
      new (&m_unionController) MatrixListController(m_editExpressionController);
    } else {
      assert(type.function);
      new (&m_unionController)
          FunctionListController(m_editExpressionController);
    }
  } else if (type.scientificNotation) {
    mainController = &m_scientificNotationListController;
  }
  computeResults(&mainController, input, exactOutput, approximateOutput,
                 calculationPreferences);

  // Tail controller
  ExpressionsListController *tailController = nullptr;
  if (type.integer) {
    tailController = &m_integerController;
  } else if (type.rational) {
    tailController = &m_rationalController;
  }
  computeResults(&tailController, input, exactOutput, approximateOutput,
                 calculationPreferences);

  if (tailController) {
    if (mainController) {
      static_cast<ChainedExpressionsListController *>(mainController)
          ->setTail(static_cast<ChainableExpressionsListController *>(
              tailController));
    } else {
      mainController = tailController;
      tailController = nullptr;
    }
  }
  if (mainController) {
    assert(mainController->numberOfRows() > 0);
    App::app()->displayModalViewController(
        mainController, 0.f, 0.f, Escher::Metric::PopUpMarginsNoBottom);
  }
}

void AdditionalResultsController::computeResults(
    ExpressionsListController **expressionsListController,
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput,
    Preferences::CalculationPreferences calculationPreferences) {
  if (*expressionsListController == nullptr) {
    return;
  }
  assert(!AdditionalResultsType::ForbidAdditionalResults(input, exactOutput,
                                                         approximateOutput));
  CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    (*expressionsListController)->tidy();
    /* TODO: The complex format will be updated multiple times during the
     * computation of additionnal results if the expressions contain complex
     * number. It should maybe be updated once and for all now. */
    (*expressionsListController)
        ->setCalculationPreferences(calculationPreferences);
    (*expressionsListController)
        ->computeAdditionalResults(input, exactOutput, approximateOutput);
  } else {
    (*expressionsListController)->tidy();
    *expressionsListController = nullptr;
  }
}

}  // namespace Calculation
