#include "language_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"

namespace Shared {

LanguageController::LanguageController(Responder * parentResponder, KDCoordinate topMargin) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, topMargin, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin, this)
{
  for (int i = 0; i < I18n::NumberOfLanguages; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
  }
}

void LanguageController::resetSelection() {
  m_selectableTableView.deselectTable();
  int index = (int)GlobalPreferences::sharedGlobalPreferences()->language()-1;
  selectCellAtLocation(0, index);
}

const char * LanguageController::title() {
  return I18n::translate(I18n::Message::Language);
}

View * LanguageController::view() {
  return &m_selectableTableView;
}

void LanguageController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

void LanguageController::viewWillAppear() {
  resetSelection();
}

bool LanguageController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)(selectedRow()+1));
    return true;
  }
  return false;
}

int LanguageController::numberOfRows() {
  return I18n::NumberOfLanguages;
}

KDCoordinate LanguageController::cellHeight() {
  return Metric::ParameterCellHeight;
}

HighlightCell * LanguageController::reusableCell(int index) {
  return &m_cells[index];
}

int LanguageController::reusableCellCount() {
  return I18n::NumberOfLanguages;
}

void LanguageController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *) cell;
  I18n::Message languages[I18n::NumberOfLanguages] = {I18n::Message::English, I18n::Message::French, I18n::Message::Spanish, I18n::Message::German, I18n::Message::Portuguese};
  myCell->setMessage(languages[index]);
}

}
