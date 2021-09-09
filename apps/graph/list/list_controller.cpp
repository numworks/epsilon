#include "list_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>
#include <escher/metric.h>
#include <poincare/code_point_layout.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

ListController::ListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  Shared::FunctionListController(parentResponder, header, footer, I18n::Message::AddFunction),
  m_parameterController(this, this, I18n::Message::FunctionColor, I18n::Message::DeleteFunction, inputEventHandlerDelegate),
  m_modelsParameterController(this, nullptr, this),
  m_modelsStackController(nullptr, &m_modelsParameterController),
  m_parameterColumnSelected(false)
{}

const char * ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

bool layoutRepresentsAnEquality(Poincare::Layout l) {
  Poincare::Layout match = l.recursivelyMatches(
      [](Poincare::Layout layout) {
      constexpr size_t numberOfSymbols = 5;
      CodePoint symbols[numberOfSymbols] = { '=', '<', '>', UCodePointInferiorEqual, UCodePointSuperiorEqual};
      for (size_t i = 0; i < numberOfSymbols; i++) {
        if (layout.type() == Poincare::LayoutNode::Type::CodePointLayout && static_cast<Poincare::CodePointLayout &>(layout).codePoint() == symbols[i]) {
          return true;
        }
      }
      return false;
    });
  return !match.isUninitialized();
}

bool ListController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  m_parameterColumnSelected = false;
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutRepresentsAnEquality(layoutField->layout())) {
      layoutField->putCursorLeftOfLayout();
      layoutField->handleEventWithText("y=");
    }
  }
  return Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event);
}

bool ListController::handleEvent(Ion::Events::Event event) {
  // Here we handle an additional parameter column, within FunctionCell's button
  if (selectedRow() >= 0 && selectedRow() <= numberOfRows() && !isAddEmptyRow(selectedRow())) {
    // Selected row is a function cell
    if (m_parameterColumnSelected) {
      // Parameter column is selected
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        // Open function parameter menu
        configureFunction(modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
        return true;
      }
      if (event == Ion::Events::Left) {
        // Leave parameter column
        m_parameterColumnSelected = false;
        selectableTableView()->reloadData();
        return true;
      }
    } else if (event == Ion::Events::Right) {
      // Enter parameter column
      m_parameterColumnSelected = true;
      selectableTableView()->reloadData();
      return true;
    }
  }
  return FunctionListController::handleEvent(event);
}

Shared::ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfDisplayableRows;
}

HighlightCell * ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayCellForIndex(HighlightCell * cell, int j) {
  assert(cell != nullptr);
  FunctionListController::willDisplayCellForIndex(cell, j);
  if (isAddEmptyRow(j)) {
    return;
  }
  assert(j >= 0 && j < modelStore()->numberOfModels());
  FunctionCell * myCell = static_cast<FunctionCell *>(cell);
  ExpiringPointer<ContinuousFunction> f = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  myCell->setLayout(f->layout());
  myCell->setMessage(f->functionCategory());
  KDColor functionColor = f->isActive() ? f->color() : Palette::GrayDark;
  myCell->setColor(functionColor);
  KDColor textColor = f->isActive() ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
  myCell->setParameterSelected(m_parameterColumnSelected);
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(&m_modelsStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

ContinuousFunctionStore * ListController::modelStore() {
  return App::app()->functionStore();
}

}
