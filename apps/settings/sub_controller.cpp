#include "sub_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Poincare;

namespace Settings {

SubController::SubController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_cells{PointerTableCell(nullptr, KDText::FontSize::Large), PointerTableCell(nullptr, KDText::FontSize::Large),
    PointerTableCell(nullptr, KDText::FontSize::Large)},
  m_selectableTableView(SelectableTableView(this, this, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_nodeModel(nullptr),
  m_preferenceIndex(0)
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
    AppsContainer * myContainer = (AppsContainer * )app()->container();
    myContainer->refreshPreferences();
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

HighlightCell * SubController::reusableCell(int index) {
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

void SubController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  PointerTableCell * myCell = (PointerTableCell *)cell;
  myCell->setText(m_nodeModel->children(index)->label());
}

void SubController::setNodeModel(const Node * nodeModel, int preferenceIndex) {
  m_nodeModel = (Node *)nodeModel;
  m_preferenceIndex = preferenceIndex;
}

void SubController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

StackViewController * SubController::stackController() const {
  return (StackViewController *)parentResponder();
}

void SubController::setPreferenceAtIndexWithValueIndex(int preferenceIndex, int valueIndex) {
  switch (preferenceIndex) {
    case 0:
      Preferences::sharedPreferences()->setAngleUnit((Expression::AngleUnit)valueIndex);
      break;
    case 1:
      Preferences::sharedPreferences()->setDisplayMode((Expression::FloatDisplayMode)valueIndex);
      break;
    case 2:
      Preferences::sharedPreferences()->setNumberType((Preferences::NumberType)valueIndex);
      break;
    case 3:
      Preferences::sharedPreferences()->setComplexFormat((Preferences::ComplexFormat)valueIndex);
      break;
    case 4:
      GlobalPreferences::sharedGlobalPreferences()->setLanguage((GlobalPreferences::Language)valueIndex);
      break;
    }
}

int SubController::valueIndexAtPreferenceIndex(int preferenceIndex) {
  switch (preferenceIndex) {
    case 0:
      return (int)Preferences::sharedPreferences()->angleUnit();
    case 1:
      return (int)Preferences::sharedPreferences()->displayMode();
    case 2:
      return (int)Preferences::sharedPreferences()->numberType();
    case 3:
      return (int)Preferences::sharedPreferences()->complexFormat();
    case 4:
      return (int)GlobalPreferences::sharedGlobalPreferences()->language();
    default:
      assert(false);
      return 0;
    }
}


}
