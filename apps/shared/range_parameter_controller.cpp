#include "range_parameter_controller.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

RangeParameterController::RangeParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController<float>(parentResponder),
  m_interactiveRange(interactiveRange),
  m_tempInteractiveRange(*interactiveRange),
  m_rangeCells{},
  m_confirmPopUpController(Invocation([](void * context, void * sender) {
    Container::activeApp()->dismissModalViewController();
    ((RangeParameterController *)context)->stackController()->pop();
    return true;
  }, this))
{
  for (int i = 0; i < k_numberOfTextCell; i++) {
    m_rangeCells[i].setParentResponder(&m_selectableTableView);
    m_rangeCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

const char * RangeParameterController::title() {
  return I18n::translate(I18n::Message::Axis);
}

int RangeParameterController::numberOfRows() const {
  return k_numberOfTextCell+1;
}

void RangeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  I18n::Message labels[k_numberOfTextCell] = {I18n::Message::XMin, I18n::Message::XMax, I18n::Message::YMin, I18n::Message::YMax};
  myCell->setMessage(labels[index]);
  myCell->setTextColor(KDColorBlack);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

void RangeParameterController::setRange(InteractiveCurveViewRange * range){
  m_interactiveRange = range;
  m_tempInteractiveRange = *range;
}

bool RangeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_interactiveRange->rangeChecksum() != m_tempInteractiveRange.rangeChecksum()) {
    // Open pop-up to confirm discarding values
    Container::activeApp()->displayModalViewController(&m_confirmPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  return FloatParameterController::handleEvent(event);
}

float RangeParameterController::parameterAtIndex(int parameterIndex) {
  ParameterGetterPointer getters[k_numberOfTextCell] = {&InteractiveCurveViewRange::xMin,
    &InteractiveCurveViewRange::xMax, &InteractiveCurveViewRange::yMin, &InteractiveCurveViewRange::yMax};
  return (m_tempInteractiveRange.*getters[parameterIndex])();
}

bool RangeParameterController::setParameterAtIndex(int parameterIndex, float f) {
  ParameterSetterPointer setters[k_numberOfTextCell] = {&InteractiveCurveViewRange::setXMin,
    &InteractiveCurveViewRange::setXMax, &InteractiveCurveViewRange::setYMin, &InteractiveCurveViewRange::setYMax};
  (m_tempInteractiveRange.*setters[parameterIndex])(f);
  return true;
}

HighlightCell * RangeParameterController::reusableParameterCell(int index, int type) {
  assert(type == 1);
  assert(index >= 0 && index < k_numberOfTextCell);
  return m_rangeCells + index;
}

int RangeParameterController::reusableParameterCellCount(int type) {
  assert(type == 1);
  return k_numberOfTextCell;
}

void RangeParameterController::buttonAction() {
  *m_interactiveRange = m_tempInteractiveRange;
  StackViewController * stack = stackController();
  stack->pop();
}

}
