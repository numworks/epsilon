#include "type_parameter_controller.h"
#include <poincare/layout_helper.h>
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>
#include "../../shared/poincare_helpers.h"

using namespace Escher;

namespace Graph {

TypeParameterController::TypeParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder),
  m_record()
{
}

void TypeParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  }
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

const char * TypeParameterController::title() {
  return I18n::translate(I18n::Message::Details);
}

void TypeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  assert(!m_record.isNull());
  selectCellAtLocation(0, 0);
  resetMemoization();
  m_selectableTableView.reloadData();
}

KDCoordinate TypeParameterController::nonMemoizedRowHeight(int j) {
  MessageTableCellWithMessageWithBuffer tempCell;
  return heightForCellAtIndex(&tempCell, j);
}

void TypeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < k_numberOfDataPoints);
  Shared::ContinuousFunction function = (Shared::ContinuousFunction)m_record;
  MessageTableCellWithMessageWithBuffer * myCell = static_cast<MessageTableCellWithMessageWithBuffer *>(cell);
  if (index == 0) {
    myCell->setMessage(I18n::Message::CurveType);
    myCell->setSubLabelMessage(I18n::Message::Default);
    myCell->setAccessoryText(I18n::translate(function.plotTypeMessage()));
  } else {
    myCell->setMessage(function.detailsTitle(index - 1));
    double value = function.detailsValue(index - 1);
    if (std::isnan(value)) {
      // There are no value to display, use the accessory for description
      myCell->setAccessoryText(I18n::translate(function.detailsDescription(index - 1)));
      myCell->setSubLabelMessage(I18n::Message::Default);
    } else {
      constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
      constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
      char buffer[bufferSize];
      Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
      myCell->setAccessoryText(buffer);
      myCell->setSubLabelMessage(function.detailsDescription(index - 1));
    }
  }
}

MessageTableCellWithMessageWithBuffer * TypeParameterController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  return &m_cells[index];
}

StackViewController * TypeParameterController::stackController() const {
  return static_cast<StackViewController *>(parentResponder());
}

}
