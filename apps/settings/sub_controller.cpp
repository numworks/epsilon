#include "sub_controller.h"
#include <assert.h>

namespace Settings {

SubController::SubController(Responder * parentResponder, Preference * preference) :
  ViewController(parentResponder),
  m_cells{MenuListCell(nullptr, KDText::FontSize::Large), MenuListCell(nullptr, KDText::FontSize::Large)},
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_nodeModel(nullptr),
  m_preferenceIndex(0),
  m_preference(preference)
{
}

const char * SubController::title() const {
  if (m_nodeModel) {
    return m_nodeModel->label();
  }
  return "";
}

View * SubController::view() {
  return &m_selectableTableView;
}

void SubController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, valueIndexAtPreferenceIndex(m_preferenceIndex));
  app()->setFirstResponder(&m_selectableTableView);
}

bool SubController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    setPreferenceAtIndexWithValueIndex(m_preferenceIndex, m_selectableTableView.selectedRow());
    StackViewController * stack = stackController();
    stack->pop();
  }
  return false;
}

int SubController::numberOfRows() {
  if (m_nodeModel) {
    return m_nodeModel->numberOfChildren();
  }
  return 0;
}

TableViewCell * SubController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_cells[index];
}

int SubController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate SubController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void SubController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(m_nodeModel->children(index)->label());
}

void SubController::setNodeModel(const Node * nodeModel, int preferenceIndex) {
  m_nodeModel = (Node *)nodeModel;
  m_preferenceIndex = preferenceIndex;
}

StackViewController * SubController::stackController() const {
  return (StackViewController *)parentResponder();
}

void SubController::setPreferenceAtIndexWithValueIndex(int preferenceIndex, int valueIndex) {
  switch (preferenceIndex) {
    case 0:
      m_preference->setAngleUnit((Preference::AngleUnit)valueIndex);
      break;
    case 1:
      m_preference->setDisplayMode((Preference::DisplayMode)valueIndex);
      break;
    case 2:
      m_preference->setNumberType((Preference::NumberType)valueIndex);
      break;
    case 3:
      m_preference->setComplexFormat((Preference::ComplexFormat)valueIndex);
      break;
    case 4:
      m_preference->setLanguage((Preference::Language)valueIndex);
      break;
    }
}

int SubController::valueIndexAtPreferenceIndex(int preferenceIndex) {
  switch (preferenceIndex) {
    case 0:
      return (int)m_preference->angleUnit();
    case 1:
      return (int)m_preference->displayMode();
    case 2:
      return (int)m_preference->numberType();
    case 3:
      return (int)m_preference->complexFormat();
    case 4:
      return (int)m_preference->language();
    default:
      assert(false);
      return 0;
    }
}


}
