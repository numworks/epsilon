#include "contributors_controller.h"
#include <assert.h>

namespace Settings {

ContributorsController::ContributorsController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
    m_cells[i].setAccessoryFont(KDFont::SmallFont);
    m_cells[i].setAccessoryTextColor(Palette::SecondaryText);
  }
}

bool ContributorsController::handleEvent(Ion::Events::Event event) {
  return GenericSubController::handleEvent(event);
}

HighlightCell * ContributorsController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int ContributorsController::reusableCellCount(int type) {
  assert(type == 0);
  return k_totalNumberOfCell;
}

constexpr static int s_numberOfDevelopers = 18;
constexpr static int s_numberOfUpsilonDevelopers = 5;
constexpr static I18n::Message s_developersUsernames[s_numberOfDevelopers] = {
  I18n::Message::PLaurianFournier,
  I18n::Message::PYannCouturier,
  I18n::Message::PDavidLuca,
  I18n::Message::PLoicE,
  I18n::Message::PVictorKretz,
  I18n::Message::PQuentinGuidee,
  I18n::Message::PJoachimLeFournis,
  I18n::Message::PMaximeFriess,
  I18n::Message::PJeanBaptisteBoric,
  I18n::Message::PSandraSimmons,
  I18n::Message::PDavid,
  I18n::Message::PDamienNicolet,
  I18n::Message::PEvannDreumont,
  I18n::Message::PSzaboLevente,
  I18n::Message::PVenceslasDuet,
  I18n::Message::PCharlotteThomas,
  I18n::Message::PAntoninLoubiere,
  I18n::Message::PCyprienMejat,
};

void ContributorsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithBuffer * myTextCell = (MessageTableCellWithBuffer *)cell;
  myTextCell->setAccessoryText(I18n::translate(s_developersUsernames[index]));
  if (index < s_numberOfUpsilonDevelopers) {
    myTextCell->setTextColor(KDColor::RGB24(0x5e81ac));
  } else {
    myTextCell->setTextColor(KDColor::RGB24(0xc53431));
  }
  myTextCell->setAccessoryTextColor(Palette::SecondaryText);
  GenericSubController::willDisplayCellForIndex(cell, index);
}

}
