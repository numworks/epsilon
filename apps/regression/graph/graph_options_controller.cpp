#include "graph_options_controller.h"
#include "../app.h"
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

GraphOptionsController::GraphOptionsController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  ExplicitSelectableListViewController(parentResponder),
  m_changeRegressionCell(I18n::Message::RegressionModel),
  m_regressionEquationCell(&m_selectableTableView, I18n::Message::RegressionEquation),
  m_r2Cell(&m_selectableTableView, I18n::Message::Default, KDFont::Size::Large),
  m_residualPlotCell(I18n::Message::ResidualPlot),
  m_xParameterCell(I18n::Message::XPrediction),
  m_yParameterCell(I18n::Message::YPrediction),
  m_removeRegressionCell(&(this->m_selectableTableView), I18n::Message::RemoveRegression, Invocation::Builder<GraphOptionsController>([](GraphOptionsController * controller, void * sender) {
      controller->removeRegression();
      return true;
    }, this)),
  m_goToParameterController(this, inputEventHandlerDelegate, store, cursor, graphController),
  m_residualPlotCellController(parentResponder, store),
  m_store(store),
  m_graphController(graphController)
{
  m_r2Cell.setLayout(Poincare::HorizontalLayout::Builder(
      {Poincare::CodePointLayout::Builder('r'),
       Poincare::VerticalOffsetLayout::Builder(
           Poincare::CodePointLayout::Builder('2'),
           Poincare::VerticalOffsetLayoutNode::VerticalPosition::Superscript)}));
}

void GraphOptionsController::removeRegression() {
  int series = m_graphController->selectedSeriesIndex();
  m_store->setSeriesRegressionType(series, Model::Type::None);
  static_cast<StackViewController *>(parentResponder())->pop();
}

const char * GraphOptionsController::title() {
  return Store::SeriesTitle(m_graphController->selectedSeriesIndex());
}

void GraphOptionsController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void GraphOptionsController::viewWillAppear() {
  m_regressionEquationCell.setVisible(displayRegressionEquationCell());
  m_r2Cell.setVisible(displayR2Cell());
  m_residualPlotCell.setVisible(displayResidualPlotCell());
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

  HighlightCell * cell = selectedCell();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (cell == &m_changeRegressionCell) {
        RegressionController * controller = App::app()->regressionController();
        controller->setSeries(m_graphController->selectedSeriesIndex());
        controller->setDisplayedFromDataTab(false);
        stack->push(controller);
        return true;
    } else if (cell == &m_residualPlotCell) {
      m_residualPlotCellController.setSeries(m_graphController->selectedSeriesIndex());
      stack->push(&m_residualPlotCellController);
      return true;
    } else if (cell == &m_xParameterCell || cell == &m_yParameterCell) {
      m_goToParameterController.setXPrediction(cell == &m_xParameterCell);
      stack->push(&m_goToParameterController);
      return true;
    }
  } else if ((event == Ion::Events::Copy || event == Ion::Events::Cut) || event == Ion::Events::Sto || event == Ion::Events::Var) {
    if (cell == &m_r2Cell) {
      if (event == Ion::Events::Sto || event == Ion::Events::Var) {
        App::app()->storeValue(m_r2Cell.text());
      } else {
        Escher::Clipboard::SharedClipboard()->store(m_r2Cell.text());
      }
      return true;
    } else if (cell == &m_regressionEquationCell) {
      Poincare::Layout l = m_regressionEquationCell.layout();
      if (!l.isUninitialized()) {
        constexpr int bufferSize = TextField::MaxBufferSize();
        char buffer[bufferSize];
        l.serializeParsedExpression(buffer, bufferSize, nullptr);
        if (event == Ion::Events::Sto || event == Ion::Events::Var) {
          App::app()->storeValue(buffer);
        } else {
          Escher::Clipboard::SharedClipboard()->store(buffer);
        }
        return true;
      }
    }
  }
  return false;
}

HighlightCell * GraphOptionsController::cell(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  HighlightCell * reusableCells[k_maxNumberOfRows] = {
    &m_changeRegressionCell,
    &m_regressionEquationCell,
    &m_r2Cell,
    &m_residualPlotCell,
    &m_xParameterCell,
    &m_yParameterCell,
    &m_removeRegressionCell,
  };
  return reusableCells[index];
}

void GraphOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int series = m_graphController->selectedSeriesIndex();
  Regression::Model * model = m_store->modelForSeries(series);
  if (cell == &m_changeRegressionCell) {
    m_changeRegressionCell.setSubtitle(model->name());
    return;
  }
  const int significantDigits = Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();
  if (cell == &m_regressionEquationCell) {
    // Regression equation uses at most 5 coefficients and a few chars (Quartic)
    constexpr int bufferSize = (Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::PrintFloat::k_numberOfStoredSignificantDigits)-1)*5 + sizeof("y=·x^4+·x^3+·x^2+·x+");
    char buffer[bufferSize] = "y=";
    constexpr int bufferOffset = sizeof("y=") - 1;
    double * coefficients = m_store->coefficientsForSeries(series, m_graphController->globalContext());
    int length = model->buildEquationTemplate(buffer + bufferOffset, bufferSize - bufferOffset, coefficients, significantDigits, displayMode);
    assert(length < bufferSize - bufferOffset);
    (void) length;
    m_regressionEquationCell.setLayout(Poincare::LayoutHelper::StringToCodePointsLayout(buffer, strlen(buffer)));
  } else if (cell == &m_r2Cell) {
    constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    double value = m_store->determinationCoefficientForSeries(series, m_graphController->globalContext());
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, significantDigits, displayMode);
    m_r2Cell.setAccessoryText(buffer);
  }
}

bool GraphOptionsController::displayRegressionEquationCell() const {
  return m_store->coefficientsAreDefined(m_graphController->selectedSeriesIndex(), m_graphController->globalContext());
}

bool GraphOptionsController::displayR2Cell() const {
  return m_store->seriesRegressionType(m_graphController->selectedSeriesIndex()) != Regression::Model::Type::Median && displayRegressionEquationCell();
}

bool GraphOptionsController::displayResidualPlotCell() const {
  return displayRegressionEquationCell();
}

}
