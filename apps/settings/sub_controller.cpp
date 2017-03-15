#include "sub_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>

using namespace Poincare;

namespace Settings {

SubController::SubController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_cells{PointerTableCell(I18n::Message::Default, KDText::FontSize::Large), PointerTableCell(I18n::Message::Default, KDText::FontSize::Large),
    PointerTableCell(I18n::Message::Default, KDText::FontSize::Large)},
  m_selectableTableView(SelectableTableView(this, this, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_nodeModel(nullptr),
  m_preferenceIndex(0)
{
  const char text[6] = {'a','+', Ion::Charset::IComplex, 'b', ' ', 0};
  m_complexFormatLayout[0] = new StringLayout(text, 6);
  const char base[3] = {'r', Ion::Charset::Exponential, 0};
  const char superscript[4] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' ', 0};
  m_complexFormatLayout[1] = new BaselineRelativeLayout(new StringLayout(base, 4), new StringLayout(superscript, 3), BaselineRelativeLayout::Type::Superscript);
  for (int i = 0; i < 2; i++) {
    m_complexFormatCells[i].setExpression(m_complexFormatLayout[i]);
  }
}

SubController::~SubController() {
  for (int i = 0; i < 2; i++) {
    if (m_complexFormatLayout[i]) {
      delete m_complexFormatLayout[i];
      m_complexFormatLayout[i] = nullptr;
    }
  }
}

const char * SubController::title() {
  if (m_nodeModel) {
    return I18n::translate(m_nodeModel->label());
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
    return true;
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
  if (m_preferenceIndex == 2) {
    return &m_complexFormatCells[index];
  }
  return &m_cells[index];
}

int SubController::reusableCellCount() {
  if (m_preferenceIndex == 2) {
    return 2;
  }
  return k_totalNumberOfCell;
}

KDCoordinate SubController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void SubController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_preferenceIndex == 2) {
    return;
  }
  PointerTableCell * myCell = (PointerTableCell *)cell;
  myCell->setMessage(m_nodeModel->children(index)->label());
}

void SubController::setNodeModel(const Node * nodeModel, int preferenceIndex) {
  m_nodeModel = (Node *)nodeModel;
  m_preferenceIndex = preferenceIndex;
}

void SubController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

void SubController::viewWillDisappear() {
  m_selectableTableView.deselectTable();
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
      Preferences::sharedPreferences()->setComplexFormat((Expression::ComplexFormat)valueIndex);
      break;
    case 3:
      GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)(valueIndex+1));
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
      return (int)Preferences::sharedPreferences()->complexFormat();
    case 3:
      return (int)GlobalPreferences::sharedGlobalPreferences()->language()-1;
    default:
      assert(false);
      return 0;
    }
}


}
