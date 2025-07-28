#ifndef CALCULATION_ADDITIONAL_RESULTS_CONTROLLER_H
#define CALCULATION_ADDITIONAL_RESULTS_CONTROLLER_H

#include "additional_results_type.h"
#include "complex_list_controller.h"
#include "function_list_controller.h"
#include "integer_list_controller.h"
#include "matrix_list_controller.h"
#include "rational_list_controller.h"
#include "scientific_notation_list_controller.h"
#include "trigonometry_list_controller.h"
#include "unit_list_controller.h"
#include "vector_list_controller.h"

namespace Calculation {

class Calculation;

class AdditionalResultsController {
 public:
  AdditionalResultsController(
      EditExpressionController* editExpressionController)
      : m_editExpressionController(editExpressionController),
        m_unionController(editExpressionController),
        m_integerController(editExpressionController),
        m_rationalController(editExpressionController),
        m_scientificNotationListController(editExpressionController) {}

  void openAdditionalResults(AdditionalResultsType type,
                             const Poincare::UserExpression input,
                             const Poincare::UserExpression exactOutput,
                             const Poincare::UserExpression approximateOutput,
                             const Poincare::Preferences::CalculationPreferences
                                 calculationPreferences);

 private:
  void computeResults(ExpressionsListController** expressionsListController,
                      const Poincare::UserExpression input,
                      const Poincare::UserExpression exactOutput,
                      const Poincare::UserExpression approximateOutput,
                      const Poincare::Preferences::CalculationPreferences
                          calculationPreferences);

  EditExpressionController* m_editExpressionController;

  union UnionController {
    UnionController(EditExpressionController* editExpressionController)
        : m_complexController(editExpressionController) {}
    ~UnionController() { listController()->~ExpressionsListController(); }
    ExpressionsListController* listController() {
      return static_cast<ExpressionsListController*>(&m_complexController);
    }
    // These results cannot exist at the same time
    ComplexListController m_complexController;
    TrigonometryListController m_trigonometryController;
    UnitListController m_unitController;
    VectorListController m_vectorController;
    MatrixListController m_matrixController;
    FunctionListController m_functionController;
  };
  UnionController m_unionController;

  // These results may be appended to another "main" result
  IntegerListController m_integerController;
  RationalListController m_rationalController;
  ScientificNotationListController m_scientificNotationListController;
};

}  // namespace Calculation

#endif
