#include "language_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"

namespace OnBoarding {

LanguageController::LanguageController(Responder * parentResponder, LogoController * logoController, UpdateController * updateController) :
  ViewController(parentResponder),
  m_logoController(logoController),
  m_updateController(updateController),
  m_selectableTableView(this, this, 0, 1, (Ion::Display::Height - I18n::NumberOfLanguages*Metric::ParameterCellHeight)/2, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin, this)
{
  for (int i = 0; i < I18n::NumberOfLanguages; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
  }
}

View * LanguageController::view() {
  return &m_selectableTableView;
}

void LanguageController::reinitOnBoarding() {
  m_selectableTableView.deselectTable();
  selectCellAtLocation(0, 0);
  app()->displayModalViewController(m_logoController, 0.5f, 0.5f);
}

void LanguageController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

bool LanguageController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)(selectedRow()+1));
    app()->displayModalViewController(m_updateController, 0.5f, 0.5f);
    return true;
  }
  if (event == Ion::Events::Back) {
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
  I18n::Message languages[I18n::NumberOfLanguages] = {I18n::Message::French, I18n::Message::English, I18n::Message::Spanish};
  myCell->setMessage(languages[index]);
}

}
