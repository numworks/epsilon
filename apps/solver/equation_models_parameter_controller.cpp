#include "equation_models_parameter_controller.h"
#include "list_controller.h"
#include <assert.h>
#include <poincare/layout_engine.h>
#include "../i18n.h"

using namespace Poincare;

namespace Solver {

constexpr const char * EquationModelsParameterController::k_models[k_numberOfModels];

EquationModelsParameterController::EquationModelsParameterController(Responder * parentResponder, EquationStore * equationStore, ListController * listController) :
  ViewController(parentResponder),
  m_emptyModelCell(I18n::Message::Empty, KDText::FontSize::Large),
  m_expressionLayouts{},
  m_selectableTableView(this),
  m_equationStore(equationStore),
  m_listController(listController)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setShowsIndicators(false);
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    Poincare::Expression * e = Expression::parse(k_models[i+1]);
    m_expressionLayouts[i] = e->createLayout(Poincare::PrintFloat::Mode::Decimal, Poincare::Expression::ComplexFormat::Cartesian);
    delete e;
    m_modelCells[i].setExpressionLayout(m_expressionLayouts[i]);
  }
}

EquationModelsParameterController::~EquationModelsParameterController() {
  for (int i = 0; i < k_numberOfExpressionCells; i++) {
    if (m_expressionLayouts[i]) {
      delete m_expressionLayouts[i];
      m_expressionLayouts[i] = nullptr;
    }
  }
}

const char * EquationModelsParameterController::title() {
  return I18n::translate(I18n::Message::UseEquationModel);
}

View * EquationModelsParameterController::view() {
  return &m_selectableTableView;
}

void EquationModelsParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectCellAtLocation(0, 0);
}

void EquationModelsParameterController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

bool EquationModelsParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Equation * newEquation = static_cast<Equation *>(m_equationStore->addEmptyModel());
    newEquation->setContent(k_models[selectedRow()]);
    app()->dismissModalViewController();
    m_listController->editExpression(newEquation, Ion::Events::OK);
    return true;
  }
  return false;
}

int EquationModelsParameterController::numberOfRows() {
  return k_numberOfExpressionCells+1;
};

KDCoordinate EquationModelsParameterController::rowHeight(int j) {
  return Metric::ToolboxRowHeight;
}

KDCoordinate EquationModelsParameterController::cumulatedHeightFromIndex(int j) {
  return rowHeight(0) * j;
}

int EquationModelsParameterController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = rowHeight(0);
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

HighlightCell * EquationModelsParameterController::reusableCell(int index, int type) {
  if (type == 0) {
    return &m_emptyModelCell;
  }
  return &m_modelCells[index];
}

int EquationModelsParameterController::reusableCellCount(int type) {
  if (type == 0) {
    return 1;
  }
  return k_numberOfExpressionCells;
}

int EquationModelsParameterController::typeAtLocation(int i, int j) {
  if (i == 0 && j == 0) {
    return 0;
  }
  return 1;
}

}
