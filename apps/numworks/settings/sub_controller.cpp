#include "sub_controller.h"
#include "apps/global_preferences.h"
#include "apps/apps_container.h"
#include "poincare/src/layout/baseline_relative_layout.h"
#include "poincare/src/layout/string_layout.h"
#include <assert.h>

using namespace Poincare;

namespace Settings {

SubController::SubController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, k_topBottomMargin, Metric::CommonRightMargin,
    k_topBottomMargin, Metric::CommonLeftMargin, this),
  m_nodeModel(nullptr)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
    m_cells[i].setAccessoryFontSize(KDText::FontSize::Small);
    m_cells[i].setAccessoryTextColor(Palette::GreyDark);
  }
  const char text[] = {'a','+', Ion::Charset::IComplex, 'b', ' '};
  m_complexFormatLayout[0] = new StringLayout(text, sizeof(text));
  const char base[] = {'r', Ion::Charset::Exponential};
  const char superscript[] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' '};
  m_complexFormatLayout[1] = new BaselineRelativeLayout(new StringLayout(base, sizeof(base)), new StringLayout(superscript, sizeof(superscript)), BaselineRelativeLayout::Type::Superscript);
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

void SubController::didEnterResponderChain(Responder * previousResponder) {
  if (previousResponder->commonAncestorWith(this) == parentResponder()) {
    /* We want to select the prefered setting node only when the previous page
     * was the main setting page. We do not to change the selection when
     * dismissing a pop-up for instance. */
    selectCellAtLocation(0, valueIndexForPreference(m_nodeModel->label()));
  }
  if (m_nodeModel->label() == &I18n::Common::ExamMode) {
    m_selectableTableView.reloadData();
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool SubController::handleEvent(Ion::Events::Event event) {
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && m_nodeModel->label() == &I18n::Common::About && selectedRow() == numberOfRows()-1) {
    app()->displayModalViewController(&m_hardwareTestPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* Behavious of "Exam mode" menu*/
    if (m_nodeModel->label() == &I18n::Common::ExamMode) {
      if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
        return false;
      }
      AppsContainer * container = (AppsContainer *)(app()->container());
      container->displayExamModePopUp(true);
      return true;
    }
    /* Behaviour of "About" menu */
    if (m_nodeModel->label() == &I18n::Common::About) {
      if (selectedRow() == 0) {
        MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
        if (strcmp(myCell->accessoryText(), Ion::patchLevel()) == 0) {
          myCell->setAccessoryText(Ion::softwareVersion());
          return true;
        }
        myCell->setAccessoryText(Ion::patchLevel());
        return true;
      }
      return false;
    }
    /* Generic behaviour of preference menu*/
    setPreferenceWithValueIndex(m_nodeModel->label(), selectedRow());
    AppsContainer * myContainer = (AppsContainer * )app()->container();
    myContainer->refreshPreferences();
    StackViewController * stack = stackController();
    stack->pop();
    return true;
  }
  if (event == Ion::Events::Left) {
    stackController()->pop();
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
  if (m_nodeModel->label() == &I18n::Common::ComplexFormat) {
    return &m_complexFormatCells[index];
  }
  return &m_cells[index];
}

int SubController::reusableCellCount() {
  if (m_nodeModel->label() == &I18n::Common::ComplexFormat) {
    return 2;
  }
  return k_totalNumberOfCell;
}

KDCoordinate SubController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void SubController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_nodeModel->label() == &I18n::Common::ComplexFormat) {
    return;
  }
  MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)cell;
  myCell->setMessage(m_nodeModel->children(index)->label());
  myCell->setMessageFontSize(KDText::FontSize::Large);
  myCell->setAccessoryText("");
  if (m_nodeModel->label() == &I18n::Common::ExamMode && GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
    myCell->setMessage(&I18n::Common::ExamModeActive);
  }
  if (m_nodeModel->label() == &I18n::Common::About) {
    myCell->setMessageFontSize(KDText::FontSize::Small);
    const char * accessoryMessage = Ion::softwareVersion();
    switch (index) {
      case 0:
        accessoryMessage = Ion::softwareVersion();
        break;
      case 1:
        accessoryMessage = Ion::serialNumber();
        break;
      case 2:
        accessoryMessage = Ion::fccId();
        break;
      default:
        assert(false);
        break;
    }
    myCell->setAccessoryText(accessoryMessage);
  }
}

void SubController::setNodeModel(const Node * nodeModel) {
  m_nodeModel = (Node *)nodeModel;
}

void SubController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

void SubController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
}

StackViewController * SubController::stackController() const {
  return (StackViewController *)parentResponder();
}

void SubController::setPreferenceWithValueIndex(const I18n::Message *message, int valueIndex) {
  if (message == &I18n::Common::AngleUnit) {
    Preferences::sharedPreferences()->setAngleUnit((Expression::AngleUnit)valueIndex);
  }
  if (message == &I18n::Common::DisplayMode) {
    Preferences::sharedPreferences()->setDisplayMode((Expression::FloatDisplayMode)valueIndex);
  }
  if (message == &I18n::Common::ComplexFormat) {
    Preferences::sharedPreferences()->setComplexFormat((Expression::ComplexFormat)valueIndex);
  }
  if (message == &I18n::Common::Language) {
    GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)(valueIndex+1));
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    appsContainer->reloadTitleBar();
  }
}

int SubController::valueIndexForPreference(const I18n::Message *message) {
  if (message == &I18n::Common::AngleUnit) {
    return (int)Preferences::sharedPreferences()->angleUnit();
  }
  if (message == &I18n::Common::DisplayMode) {
    return (int)Preferences::sharedPreferences()->displayMode();
  }
  if (message == &I18n::Common::ComplexFormat) {
    return (int)Preferences::sharedPreferences()->complexFormat();
  }
  if (message == &I18n::Common::Language) {
    return (int)GlobalPreferences::sharedGlobalPreferences()->language()-1;
  }
  return 0;
}


}
