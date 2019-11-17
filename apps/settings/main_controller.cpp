#include "main_controller.h"
#include "../global_preferences.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Poincare;

namespace Settings {

MainController::MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  ViewController(parentResponder),
  m_brightnessCell(I18n::Message::Default, KDFont::LargeFont),
  m_popUpCell(I18n::Message::Default, KDFont::LargeFont),
  m_selectableTableView(this),
  m_mathOptionsController(this, inputEventHandlerDelegate),
  m_languageController(this, 13),
  m_accessibilityController(this),
  m_examModeController(this),
  m_aboutController(this)
{
  for (int i = 0; i < k_numberOfSimpleChevronCells; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  if (model()->children(selectedRow())->numberOfChildren() == 0) {
    if (model()->children(selectedRow())->label() == promptMessage()) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        globalPreferences->setShowPopUp(!globalPreferences->showPopUp());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (model()->children(selectedRow())->label() == I18n::Message::Brightness) {
      if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
        int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
        int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
        globalPreferences->setBrightnessLevel(globalPreferences->brightnessLevel()+direction);
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (model()->children(selectedRow())->label() == I18n::Message::Language) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
        stackController()->push(&m_languageController);
        return true;
      }
      return false;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    GenericSubController * subController = nullptr;
    if (model()->children(selectedRow())->label() == I18n::Message::Brightness || model()->children(selectedRow())->label() == I18n::Message::Language) {
      assert(false);
    } else if (model()->children(selectedRow())->label() == I18n::Message::ExamMode) {
      subController = &m_examModeController;
    } else if (model()->children(selectedRow())->label() == I18n::Message::About) {
      subController = &m_aboutController;
    } else if (model()->children(selectedRow())->label() == I18n::Message::Accessibility) {
      subController = &m_accessibilityController;
    } else if (model()->children(selectedRow())->label() == I18n::Message::MathOptions) {
      subController = &m_mathOptionsController;
    }
    subController->setMessageTreeModel(model()->children(selectedRow()));
    StackViewController * stack = stackController();
    stack->push(subController);
    return true;
  }
  return false;
}

int MainController::numberOfRows() const {
  return model()->numberOfChildren();
};

KDCoordinate MainController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int MainController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return offsetY/rowHeight(0);
}

HighlightCell * MainController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_numberOfSimpleChevronCells);
    return &m_cells[index];
  }
  assert(index == 0);
  if (type == 2) {
    return &m_popUpCell;
  }
  assert(type == 1);
  return &m_brightnessCell;
}

int MainController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfSimpleChevronCells;
  }
  return 1;
}

int MainController::typeAtLocation(int i, int j) {
  if (model()->children(j)->label() == I18n::Message::Brightness) {
    return 1;
  }
  if (model()->children(j)->label() == I18n::Message::UpdatePopUp || model()->children(j)->label() == I18n::Message::BetaPopUp) {
    return 2;
  }
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message thisLabel = model()->children(index)->label();
  myCell->setMessage(thisLabel);

  //switch to irregular cell types
  if (thisLabel == I18n::Message::Brightness) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)globalPreferences->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
  if (thisLabel == I18n::Message::Language) {
    int index = (int)globalPreferences->language()-1;
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
  if (hasPrompt() && (thisLabel == I18n::Message::UpdatePopUp || thisLabel == I18n::Message::BetaPopUp)) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->showPopUp());
    return;
  }
}

void MainController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
