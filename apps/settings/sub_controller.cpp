#include "sub_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;

namespace Settings {

SubController::SubController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_editableCell(&m_selectableTableView, this, m_draftTextBuffer),
  m_selectableTableView(this, this, 0, 1, k_topBottomMargin, Metric::CommonRightMargin,
    k_topBottomMargin, Metric::CommonLeftMargin, this),
  m_messageTreeModel(nullptr)
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
  m_editableCell.setMessage(I18n::Message::SignificantFigures);
  m_editableCell.setMessageFontSize(KDText::FontSize::Large);
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
  if (m_messageTreeModel) {
    return I18n::translate(m_messageTreeModel->label());
  }
  return "";
}

View * SubController::view() {
  return &m_selectableTableView;
}

void SubController::didEnterResponderChain(Responder * previousFirstResponder) {
  if (m_messageTreeModel->label() == I18n::Message::ExamMode) {
    m_selectableTableView.reloadData();
  }
}

void SubController::didBecomeFirstResponder() {
  selectCellAtLocation(0, valueIndexForPreference(m_messageTreeModel->label()));
  app()->setFirstResponder(&m_selectableTableView);
}

bool SubController::handleEvent(Ion::Events::Event event) {
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && m_messageTreeModel->label() == I18n::Message::About && selectedRow() == numberOfRows()-1) {
    app()->displayModalViewController(&m_hardwareTestPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* Behavious of "Exam mode" menu*/
    if (m_messageTreeModel->label() == I18n::Message::ExamMode) {
      if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
        return false;
      }
      AppsContainer * container = (AppsContainer *)(app()->container());
      container->displayExamModePopUp(true);
      return true;
    }
    /* Behaviour of "About" menu */
    if (m_messageTreeModel->label() == I18n::Message::About) {
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
    assert(m_messageTreeModel->label() != I18n::Message::DisplayMode || selectedRow() != numberOfRows()-1); // In that case, events OK and EXE are handled by the cell
    setPreferenceWithValueIndex(m_messageTreeModel->label(), selectedRow());
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
  if (m_messageTreeModel) {
    return m_messageTreeModel->numberOfChildren();
  }
  return 0;
}

HighlightCell * SubController::reusableCell(int index, int type) {
  if (type == 2) {
    assert(index == 0);
    return &m_editableCell;
  } else if (type == 1) {
    assert(index >= 0 && index < 2);
    return &m_complexFormatCells[index];
  }
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int SubController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return k_totalNumberOfCell;
    case 1:
      return 2;
    case 2:
      return 1;
    default:
      assert(false);
      return 0;
  }
}

int SubController::typeAtLocation(int i, int j) {
  if (m_messageTreeModel->label() == I18n::Message::ComplexFormat) {
    return 1;
  }
  if (m_messageTreeModel->label() == I18n::Message::DisplayMode && j == numberOfRows()-1) {
    return 2;
  }
  return 0;
}

KDCoordinate SubController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate SubController::cumulatedHeightFromIndex(int j) {
  return rowHeight(0) * j;
}

int SubController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = rowHeight(0);
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

void SubController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_messageTreeModel->label() == I18n::Message::ComplexFormat) {
    return;
  }
  /* Number of significants figure row */
  if (m_messageTreeModel->label() == I18n::Message::DisplayMode && index == numberOfRows()-1) {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
    char buffer[3];
    Integer(Preferences::sharedPreferences()->numberOfSignificantDigits()).writeTextInBuffer(buffer, 3);
    myCell->setAccessoryText(buffer);
    return;
  }
  MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)cell;
  myCell->setMessage(m_messageTreeModel->children(index)->label());
  myCell->setMessageFontSize(KDText::FontSize::Large);
  myCell->setAccessoryText("");
  if (m_messageTreeModel->label() == I18n::Message::ExamMode && GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
  if (m_messageTreeModel->label() == I18n::Message::About) {
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

void SubController::setMessageTreeModel(const MessageTree * messageTreeModel) {
  m_messageTreeModel = (MessageTree *)messageTreeModel;
}

void SubController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

void SubController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
}

bool SubController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Up && selectedRow() > 0)
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool SubController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  Context * globalContext = textFieldDelegateApp()->localContext();
  float floatBody = Expression::approximateToScalar<float>(text, *globalContext);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    floatBody = PrintFloat::k_numberOfPrintedSignificantDigits;
  }
  if (floatBody < 1) {
   floatBody = 1;
  }
  if (floatBody > PrintFloat::k_numberOfStoredSignificantDigits) {
    floatBody = PrintFloat::k_numberOfStoredSignificantDigits;
  }
  Preferences::sharedPreferences()->setNumberOfSignificantDigits((char)std::round(floatBody));
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  if (event == Ion::Events::Up || event == Ion::Events::OK) {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}

StackViewController * SubController::stackController() const {
  return (StackViewController *)parentResponder();
}

void SubController::setPreferenceWithValueIndex(I18n::Message message, int valueIndex) {
  if (message == I18n::Message::AngleUnit) {
    Preferences::sharedPreferences()->setAngleUnit((Expression::AngleUnit)valueIndex);
  }
  if (message == I18n::Message::DisplayMode) {
    Preferences::sharedPreferences()->setDisplayMode((PrintFloat::Mode)valueIndex);
  }
  if (message == I18n::Message::ComplexFormat) {
    Preferences::sharedPreferences()->setComplexFormat((Expression::ComplexFormat)valueIndex);
  }
}

int SubController::valueIndexForPreference(I18n::Message message) {
  if (message == I18n::Message::AngleUnit) {
    return (int)Preferences::sharedPreferences()->angleUnit();
  }
  if (message == I18n::Message::DisplayMode) {
    return (int)Preferences::sharedPreferences()->displayMode();
  }
  if (message == I18n::Message::ComplexFormat) {
    return (int)Preferences::sharedPreferences()->complexFormat();
  }
  return 0;
}

Shared::TextFieldDelegateApp * SubController::textFieldDelegateApp() {
  return (Shared::TextFieldDelegateApp *)app();
}

}
