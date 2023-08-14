#ifndef CALCULATION_ADDITIONAL_RESULTS_CONTROLLER_H
#define CALCULATION_ADDITIONAL_RESULTS_CONTROLLER_H

#include <poincare/expression.h>

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

  struct AdditionalResultsType {
    bool integer : 1;
    bool rational : 1;
    bool directTrigonometry : 1;
    bool inverseTrigonometry : 1;
    bool unit : 1;
    bool matrix : 1;
    bool vector : 1;
    bool complex : 1;
    bool function : 1;
    bool scientificNotation : 1;
    bool isNotEmpty() const {
      return integer || rational || directTrigonometry || inverseTrigonometry ||
             unit || matrix || vector || complex || function ||
             scientificNotation;
    }
  };

  static AdditionalResultsType AdditionalResultsForCalculation(
      Calculation* calculation);

  static bool ForbidAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput,
      const Poincare::Expression approximateOutput);

  static bool HasComplexAdditionalResults(
      const Poincare::Expression approximateOutput);
  static bool HasDirectTrigoAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput);
  static bool HasInverseTrigoAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput);
  static bool HasUnitAdditionalResults(const Poincare::Expression exactOutput);
  static bool HasVectorAdditionalResults(
      const Poincare::Expression exactOutput);
  static bool HasMatrixAdditionalResults(
      const Poincare::Expression exactOutput);
  static bool HasFunctionAdditionalResults(
      const Poincare::Expression input,
      const Poincare::Expression approximateOutput);
  static bool HasScientificNotationAdditionalResults(
      const Poincare::Expression approximateOutput);
  static bool HasIntegerAdditionalResults(
      const Poincare::Expression exactOutput);
  static bool HasRationalAdditionalResults(
      const Poincare::Expression exactOutput);

  void openAdditionalResults(AdditionalResultsType type,
                             const Poincare::Expression input,
                             const Poincare::Expression exactOutput,
                             const Poincare::Expression approximateOutput);

 private:
  void computeResults(ExpressionsListController** expressionsListController,
                      const Poincare::Expression input,
                      const Poincare::Expression exactOutput,
                      const Poincare::Expression approximateOutput);

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
