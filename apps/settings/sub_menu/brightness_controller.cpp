#include "brightness_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include "../../shared/poincare_helpers.h"
#include <assert.h>
#include <cmath>
#include "../app.h"
#include <apps/shared/text_field_delegate_app.h>
#include <poincare/integer.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

BrightnessController::BrightnessController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  GenericSubController(parentResponder),
  m_brightnessCell(I18n::Message::Brightness, KDFont::LargeFont),
  m_editableCellIdleBeforeDimmingSeconds(&m_selectableTableView, inputEventHandlerDelegate, this),
  m_editableCellIdleBeforeSuspendSeconds(&m_selectableTableView, inputEventHandlerDelegate, this),
  m_BrightnessShortcutCell(&m_selectableTableView, inputEventHandlerDelegate, this)
{
  m_editableCellIdleBeforeDimmingSeconds.setMessage(I18n::Message::IdleTimeBeforeDimming);
  m_editableCellIdleBeforeDimmingSeconds.setMessageFont(KDFont::LargeFont);
  m_editableCellIdleBeforeSuspendSeconds.setMessage(I18n::Message::IdleTimeBeforeSuspend);
  m_editableCellIdleBeforeSuspendSeconds.setMessageFont(KDFont::LargeFont);
  m_BrightnessShortcutCell.setMessage(I18n::Message::BrightnessShortcut);
  m_BrightnessShortcutCell.setMessageFont(KDFont::LargeFont);
}

HighlightCell * BrightnessController::reusableCell(int index, int type) {
  HighlightCell * editableCell[] = {
    &m_brightnessCell,
    &m_editableCellIdleBeforeDimmingSeconds,
    &m_editableCellIdleBeforeSuspendSeconds,
    &m_BrightnessShortcutCell
  };
  return editableCell[index];
}

int BrightnessController::reusableCellCount(int type) {
  switch(type) {
    case 0:
    case 1:
      return k_totalNumberOfCell;
    default:
      assert(false);
      return 0;
  }
}

void BrightnessController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if(index == 0){
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  } else {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
    GenericSubController::willDisplayCellForIndex(myCell, index);
    char buffer[5];
    int val;
    switch(index){
      case 1:{
        val = GlobalPreferences::sharedGlobalPreferences()->idleBeforeDimmingSeconds();
        break;
      }
      case 2:{
        val = GlobalPreferences::sharedGlobalPreferences()->idleBeforeSuspendSeconds();
        break;
      }
      case 3:{
        val = GlobalPreferences::sharedGlobalPreferences()->brightnessShortcut();
        break;
      }
      default:
        assert(false);
    }
    Poincare::Integer(val).serialize(buffer, 5);
    myCell->setAccessoryText(buffer);
  }
}

bool BrightnessController::handleEvent(Ion::Events::Event event) {
    if ((selectedRow() == 0) && (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus)) {
      int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
      int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
      GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
      m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      return true;
    }
    if (event == Ion::Events::BrightnessPlus || event == Ion::Events::BrightnessMinus){
      int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
      int NumberOfStepsPerShortcut = GlobalPreferences::sharedGlobalPreferences()->brightnessShortcut();
      int direction = (event == Ion::Events::BrightnessPlus) ? NumberOfStepsPerShortcut*delta : -delta*NumberOfStepsPerShortcut;
      GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
      m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), 0);
      return true;
    }
    return false;
}

bool BrightnessController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return ((event == Ion::Events::Up && selectedRow() > 0) || (event == Ion::Events::Down && selectedRow() < k_totalNumberOfCell - 1))
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool BrightnessController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  int val = std::round(floatBody);
  switch (selectedRow()) {
    case 1:
      GlobalPreferences::sharedGlobalPreferences()->setIdleBeforeDimmingSeconds(val);
      m_editableCellIdleBeforeDimmingSeconds.setAccessoryText(text);
      break;
    case 2:
      GlobalPreferences::sharedGlobalPreferences()->setIdleBeforeSuspendSeconds(val);
      m_editableCellIdleBeforeSuspendSeconds.setAccessoryText(text);
      break;
    case 3:{
      if(val > GlobalPreferences::NumberOfBrightnessStates){ val = GlobalPreferences::NumberOfBrightnessStates;
      } else if (val < 0){val = 0;}
      GlobalPreferences::sharedGlobalPreferences()->setBrightnessShortcut(val);
      m_BrightnessShortcutCell.setAccessoryText(text);
      break;
    }
    default:
      assert(false);
  }
  AppsContainer * myContainer = AppsContainer::sharedAppsContainer();
  myContainer->refreshPreferences();
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  if (event == Ion::Events::Up || event == Ion::Events::Down || event == Ion::Events::OK) {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}

}
