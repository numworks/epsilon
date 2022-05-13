#include "graph_options_controller.h"
#include "app.h"
#include "graph_controller.h"
#include "regression_controller.h"
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/horizontal_layout.h>
#include <escher/clipboard.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Regression {

GraphOptionsController::GraphOptionsController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  SelectableListViewController(parentResponder),
  m_changeRegressionCell(I18n::Message::Regression),
  // TODO Hugo : Update LineEquationTitle, Plot and Regression Messages
  m_regressionEquationCell(&m_selectableTableView, I18n::Message::LineEquationTitle),
  m_r2Cell(&m_selectableTableView),
  m_residualPlot(I18n::Message::Plot),
  m_xParameterCell(I18n::Message::XPrediction),
  m_yParameterCell(I18n::Message::YPrediction),
  m_removeRegressionCell(&(this->m_selectableTableView), I18n::Message::Regression, Invocation([](void * context, void * sender) {
      GraphOptionsController * controller = (GraphOptionsController *) context;
      controller->removeRegression();
      return true;
    }, this)),
  m_goToParameterController(this, inputEventHandlerDelegate, store, cursor, graphController),
  m_store(store),
  m_graphController(graphController)
{
  m_r2Cell.setLayout(Poincare::HorizontalLayout::Builder(
      {Poincare::CodePointLayout::Builder('r', KDFont::LargeFont),
       Poincare::VerticalOffsetLayout::Builder(
           Poincare::CodePointLayout::Builder('2', KDFont::LargeFont),
           Poincare::VerticalOffsetLayoutNode::Position::Superscript)}));
}

void GraphOptionsController::removeRegression() {
  int series = m_graphController->selectedSeriesIndex();
  m_store->setSeriesRegressionType(series, Model::Type::None);
  static_cast<StackViewController *>(parentResponder())->pop();
}

void GraphOptionsController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void GraphOptionsController::viewWillAppear() {
  // m_regressionEquationCell may have changed size
  m_regressionEquationCell.reloadScroll();
  resetMemoization();
  m_selectableTableView.reloadData();
}

bool GraphOptionsController::handleEvent(Ion::Events::Event event) {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::Left && stack->depth() > Shared::InteractiveCurveViewController::k_graphControllerStackDepth + 1) {
    /* We only allow popping with Left if there is another menu beneath this
     * one. */
    stack->pop();
    return true;
  }

  int type = typeAtIndex(selectedRow());
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    switch (type) {
    case k_changeRegressionCellType: {
        RegressionController * controller = App::app()->regressionController();
        controller->setSeries(m_graphController->selectedSeriesIndex());
        stack->push(controller);
        return true;
      }
    case k_residualPlotCellType:
      // TODO Hugo : Implement residual plot
      return true;
    case k_xParameterCellType:
    case k_yParameterCellType:
      m_goToParameterController.setXPrediction(type == k_xParameterCellType);
      stack->push(&m_goToParameterController);
      return true;
    }
  } else if ((event == Ion::Events::Copy || event == Ion::Events::Cut)) {
    if (type == k_r2CellType) {
      Escher::Clipboard::sharedClipboard()->store(m_r2Cell.text(), strlen(m_r2Cell.text()));
      return true;
    } else if (type == k_regressionEquationCellType) {
      Poincare::Layout l = m_regressionEquationCell.layout();
      if (!l.isUninitialized()) {
        constexpr int bufferSize = TextField::maxBufferSize();
        char buffer[bufferSize];
        l.serializeParsedExpression(buffer, bufferSize, nullptr);
        Escher::Clipboard::sharedClipboard()->store(buffer, strlen(buffer));
        return true;
      }
    }
  }
  return false;
}

HighlightCell * GraphOptionsController::reusableCell(int index, int type) {
  assert(type >= 0 && type < k_maxNumberOfRows);
  HighlightCell * reusableCells[k_maxNumberOfRows] = {
    &m_changeRegressionCell,
    &m_regressionEquationCell,
    &m_r2Cell,
    &m_residualPlot,
    &m_xParameterCell,
    &m_yParameterCell,
    &m_removeRegressionCell,
  };
  return reusableCells[type];
}

int GraphOptionsController::typeAtIndex(int index) {
  if (index >= k_r2CellType && !displayR2Cell()) {
    index++;
  }
  return index;
}

void GraphOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int series = m_graphController->selectedSeriesIndex();
  Regression::Model * model = m_store->modelForSeries(series);
  int type = typeAtIndex(index);
  if (type == k_changeRegressionCellType) {
    m_changeRegressionCell.setSubtitle(model->name());
    return;
  }
  const int significantDigits = Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();
  if (type == k_regressionEquationCellType) {
    // TODO Hugo : Improve bufferSize
    // Regression equation uses at most 5 coefficients and a few chars
    constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits)*5 - 5 + 18 + 1;
    char buffer[bufferSize];
    double * coefficients = m_store->coefficientsForSeries(series, m_graphController->globalContext());
    int length = model->buildEquationTemplate(buffer, bufferSize, coefficients, significantDigits, displayMode);
    assert(length < bufferSize);
    (void) length;
    m_regressionEquationCell.setLayout(Poincare::LayoutHelper::StringToCodePointsLayout(buffer, strlen(buffer)));
  } else if (type == k_r2CellType) {
    constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    double value = m_store->determinationCoefficientForSeries(series, m_graphController->globalContext());
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, significantDigits, displayMode);
    m_r2Cell.setAccessoryText(buffer);
  }
}

bool GraphOptionsController::displayR2Cell() const {
  return m_store->seriesRegressionType(m_graphController->selectedSeriesIndex()) == Regression::Model::Type::Linear;
}

}
