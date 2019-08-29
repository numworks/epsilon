#include "domain_parameter_controller.h"
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

DomainParameterController::DomainParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  FloatParameterController(parentResponder),
  m_domainCells{},
  m_record()
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_domainCells[i].setParentResponder(&m_selectableTableView);
    m_domainCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

const char * DomainParameterController::title() {
  return I18n::translate(I18n::Message::FunctionDomain);
}

int DomainParameterController::numberOfRows() {
  return k_totalNumberOfCell+1;
}

void DomainParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  Shared::CartesianFunction::PlotType plotType = function()->plotType();
  I18n::Message * labels;
  switch (plotType) {
    case Shared::CartesianFunction::PlotType::Cartesian:
    {
      I18n::Message l[k_totalNumberOfCell] = {I18n::Message::XMin, I18n::Message::XMax};
      labels = l;
      break;
    }
    case Shared::CartesianFunction::PlotType::Parametric:
    {
      I18n::Message l[k_totalNumberOfCell] = {I18n::Message::TMin, I18n::Message::TMax};
      labels = l;
      break;
    }
    default:
    {
      assert(plotType == Shared::CartesianFunction::PlotType::Polar);
      I18n::Message l[k_totalNumberOfCell] = {I18n::Message::ThetaMin, I18n::Message::ThetaMax};
      labels = l;
      break;
    }
  }
  myCell->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

int DomainParameterController::reusableParameterCellCount(int type) {
  return k_totalNumberOfCell;
}

HighlightCell * DomainParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_domainCells[index];
}

bool DomainParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && stackController()->depth() > 2) {
    stackController()->pop();
    return true;
  }
  return false;
}

double DomainParameterController::parameterAtIndex(int index) {
  return index == 0 ? function()->tMin() : function()->tMax();
}

bool DomainParameterController::setParameterAtIndex(int parameterIndex, double f) {
  // TODO: what to do if the xmin > xmax?
  parameterIndex == 0 ? function()->setTMin(f) : function()->setTMax(f);
  return true;
}

void DomainParameterController::buttonAction() {
  StackViewController * stack = stackController();
  stack->pop();
  stack->pop();
}

Shared::ExpiringPointer<Shared::CartesianFunction> DomainParameterController::function() {
  assert(!m_record.isNull());
  App * myApp = App::app();
  return myApp->functionStore()->modelForRecord(m_record);
}

}
