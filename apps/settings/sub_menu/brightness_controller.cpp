#include "brightness_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include "../../shared/poincare_helpers.h"
#include <assert.h>
#include <cmath>
#include "../app.h"

#include <poincare/integer.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

BrightnessController::BrightnessController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  GenericSubController(parentResponder),
  m_brightnessCell(I18n::Message::Normal, KDFont::LargeFont),
  m_dimBrightnessCell(I18n::Message::Dim, KDFont::LargeFont),
  m_editableCellIdleBeforeDimmingSeconds(&m_selectableTableView, inputEventHandlerDelegate, this),
  m_editableCellIdleBeforeSuspendSeconds(&m_selectableTableView, inputEventHandlerDelegate, this)
{
  m_editableCellIdleBeforeDimmingSeconds.messageTableCellWithEditableText()->setMessage(I18n::Message::IdleTimeBeforeDimming);
  m_editableCellIdleBeforeDimmingSeconds.messageTableCellWithEditableText()->setMessageFont(KDFont::LargeFont);
  m_editableCellIdleBeforeSuspendSeconds.messageTableCellWithEditableText()->setMessage(I18n::Message::IdleTimeBeforeSuspend);
  m_editableCellIdleBeforeSuspendSeconds.messageTableCellWithEditableText()->setMessageFont(KDFont::LargeFont);
}

KDCoordinate BrightnessController::rowHeight(int j) {
  if (j == numberOfRows()-1) {
    return Metric::ParameterCellHeight+MessageTableCellWithEditableTextWithSeparator::k_margin;
  }
  return Metric::ParameterCellHeight;
}

KDCoordinate BrightnessController::cumulatedHeightFromIndex(int j) {
  return TableViewDataSource::cumulatedHeightFromIndex(j);
}

int BrightnessController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return TableViewDataSource::indexFromCumulatedHeight(offsetY);
}

HighlightCell * BrightnessController::reusableCell(int index, int type) {
  HighlightCell * editableCell[] = {
    &m_brightnessCell,
    &m_dimBrightnessCell,
    &m_editableCellIdleBeforeDimmingSeconds,
    &m_editableCellIdleBeforeSuspendSeconds
  };
  return editableCell[index];
}

int BrightnessController::reusableCellCount(int type) {
  switch(type) {
    case 0:
    case 1:
      return k_totalNumberOfCell ;
    default:
      assert(false);
      return 0;
  }
}

int BrightnessController::typeAtLocation(int i, int j) {
  return (j < 2 ? 1 : 1);
}

void BrightnessController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index < 2) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    switch (index) {
      case 0:
        myGauge->setLevel((float)GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
        break;
      default:
        myGauge->setLevel((float)GlobalPreferences::sharedGlobalPreferences()->dimBacklightBrightness()/(float)Ion::Backlight::MaxBrightness);
        break;
    }
    return;
  }
  MessageTableCellWithEditableTextWithSeparator * myCell = (MessageTableCellWithEditableTextWithSeparator *)cell;
  GenericSubController::willDisplayCellForIndex(myCell->messageTableCellWithEditableText(), index);
  int val[] = {
    GlobalPreferences::sharedGlobalPreferences()->idleBeforeDimmingSeconds(),
    GlobalPreferences::sharedGlobalPreferences()->idleBeforeSuspendSeconds()
  };
  char buffer[5];
  Poincare::Integer(val[index-2]).serialize(buffer, 5);
  myCell->messageTableCellWithEditableText()->setAccessoryText(buffer);
}

bool BrightnessController::handleEvent(Ion::Events::Event event) {
    if ((selectedRow() < 2) && (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus)) {
      int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
      int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
      switch (selectedRow()) {
        case 0:
          GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
          break;
        default:
          GlobalPreferences::sharedGlobalPreferences()->setDimBacklightBrightness(GlobalPreferences::sharedGlobalPreferences()->dimBacklightBrightness()+direction);
          break;
      }
      m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      return true;
    }
    return false;
}

bool BrightnessController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Up && selectedRow() > 0)
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool BrightnessController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  int val = std::round(floatBody);
  switch (selectedRow()) {
    case 2:
      GlobalPreferences::sharedGlobalPreferences()->setIdleBeforeDimmingSeconds(val);
      m_editableCellIdleBeforeDimmingSeconds.messageTableCellWithEditableText()->setAccessoryText(text);
      break;
    case 3:
      GlobalPreferences::sharedGlobalPreferences()->setIdleBeforeSuspendSeconds(val);
      m_editableCellIdleBeforeSuspendSeconds.messageTableCellWithEditableText()->setAccessoryText(text);
      break;
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


Shared::TextFieldDelegateApp * BrightnessController::textFieldDelegateApp() {
  return (Shared::TextFieldDelegateApp *) AppsContainer::sharedAppsContainer()->activeApp();
}

}