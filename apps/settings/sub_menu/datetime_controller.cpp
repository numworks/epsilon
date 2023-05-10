#include "../app.h"
#include "datetime_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>

using namespace Shared;

namespace Settings {

DateTimeController::DateTimeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_textCells{
    MessageTableCellWithEditableText{this, nullptr, this},
    MessageTableCellWithEditableText{this, nullptr, this},
  }
{
  for (int i = 0; i < k_totalNumberOfSwitchCells; i++) {
    m_switchCells[i].setMessageFont(KDFont::LargeFont);
  }
  for (int i = 0; i < k_totalNumberOfTextCells; i++) {
    m_textCells[i].setMessageFont(KDFont::LargeFont);
  }
}

bool DateTimeController::handleEvent(Ion::Events::Event event) {
  bool clockEnabled = Ion::RTC::mode() != Ion::RTC::Mode::Disabled;

  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (selectedRow() == 0) {
      clockEnabled = !clockEnabled;
      if (clockEnabled) {
        #ifndef _FXCG
        // This doesn't apply on Casio calculators
        Container::activeApp()->displayWarning(I18n::Message::RTCWarning1, I18n::Message::RTCWarning2);
        #endif
      }
      Ion::RTC::setMode(clockEnabled ? Ion::RTC::Mode::HSE : Ion::RTC::Mode::Disabled);
    }
  }
  else {
    return GenericSubController::handleEvent(event);
  }
  for (int i = 0; i < numberOfRows(); i++) {
    m_selectableTableView.reloadCellAtLocation(0, i);
  }
  AppsContainer::sharedAppsContainer()->redrawWindow();
  return true;
}

HighlightCell * DateTimeController::reusableCell(int index, int type) {
  assert(type == 1 || type == 2);
  if (type == 2) {
    assert(index >= 0 && index < k_totalNumberOfSwitchCells);
    return &m_switchCells[index];
  }
  else if (type == 1) {
    assert(index >= 0 && index < k_totalNumberOfTextCells);
    return &m_textCells[index];
  }
  return nullptr;
}

int DateTimeController::reusableCellCount(int type) {
  assert(type == 1 || type == 2);
  if (type == 2) {
    return k_totalNumberOfSwitchCells;
  }
  else if (type == 1) {
    return k_totalNumberOfTextCells;
  }
  return 0;
}

void DateTimeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);

  MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;

  if (index == 0) {
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(Ion::RTC::mode() != Ion::RTC::Mode::Disabled);
  }
  else {
    TextField * myTextField = (TextField *)mySwitchCell->accessoryView();
    Ion::RTC::DateTime dateTime = Ion::RTC::dateTime();
    if (index == 1) {
      Ion::RTC::toStringDate(dateTime, m_timeBuffer);
      myTextField->setText(m_timeBuffer);
    }
    else {
      Ion::RTC::toStringTime(dateTime, m_dateBuffer);
      myTextField->setText(m_dateBuffer);
    }
  }
}

int DateTimeController::typeAtLocation(int i, int j) {
  switch (j) {
    case 0:
      return 2;
    default:
      return 1;
  }
}

bool DateTimeController::textFieldShouldFinishEditing(TextField * view, Ion::Events::Event event) {
  return event == Ion::Events::Up || event == Ion::Events::Down || event == Ion::Events::EXE || event == Ion::Events::OK;
}

bool DateTimeController::textFieldDidReceiveEvent(TextField * view, Ion::Events::Event event) {
  if (view->isEditing() && view->shouldFinishEditing(event)) {
    Ion::RTC::DateTime dateTime = Ion::RTC::dateTime();

    if (((TextField*)m_textCells[0].accessoryView()) == view) {
      if (!Ion::RTC::parseDate(view->text(), dateTime)) {
        Container::activeApp()->displayWarning(I18n::Message::SyntaxError);
        return true;
      }
    }
    else {
      if (!Ion::RTC::parseTime(view->text(), dateTime)) {
        Container::activeApp()->displayWarning(I18n::Message::SyntaxError);
        return true;
      }
    }
    Ion::RTC::setDateTime(dateTime);
  }
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    m_selectableTableView.handleEvent(event);
    return true;
  }
  return false;
}

bool DateTimeController::textFieldDidFinishEditing(TextField * view, const char * text, Ion::Events::Event event)
{
  for (int i = 0; i < numberOfRows(); i++) {
    m_selectableTableView.reloadCellAtLocation(0, i);
  }
  return true;
}

}
