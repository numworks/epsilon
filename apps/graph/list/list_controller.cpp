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
  m_expressionCells{},
  m_parameterController(this, this, I18n::Message::FunctionColor, I18n::Message::DeleteFunction, inputEventHandlerDelegate),
  m_modelsParameterController(this, nullptr, this),
  m_modelsStackController(nullptr, &m_modelsParameterController)
{}

const char * ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

bool layoutRepresentsAnEquality(Poincare::Layout l) {
  Poincare::Layout match = l.recursivelyMatches(
      [](Poincare::Layout layout) {
      CodePoint symbols[5] = { '=', '<', '>', UCodePointInferiorEqual, UCodePointSuperiorEqual};
      for (size_t i = 0; i < sizeof(symbols); i++) {
        if (layout.type() == Poincare::LayoutNode::Type::CodePointLayout && static_cast<Poincare::CodePointLayout &>(layout).codePoint() == symbols[i]) {
          return true;
        }
      }
      return false;
    });
  return !match.isUninitialized();
}

bool ListController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutRepresentsAnEquality(layoutField->layout())) {
      layoutField->putCursorLeftOfLayout();
      layoutField->handleEventWithText("y=");
    }
  }
  return Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event);
}

bool ListController::layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) {
  // TODO Hugo : Check there is a point with this function
  return true;
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
  if (isAddEmptyRow(j)) {
    return FunctionListController::willDisplayCellForIndex(cell, j);
  }
  assert(j >= 0 && j < modelStore()->numberOfModels());
  Shared::FunctionListController::willDisplayExpressionCellAtIndex(cell, j);
  FunctionExpressionCell * myCell = static_cast<FunctionExpressionCell *>(cell);
  ExpiringPointer<NewFunction> f = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  KDColor textColor = f->isActive() ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(&m_modelsStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

ContinuousFunctionStore * ListController::modelStore() {
  return App::app()->functionStore();
}

}
