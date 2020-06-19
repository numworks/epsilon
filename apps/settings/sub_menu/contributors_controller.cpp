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

constexpr static int s_numberOfDevelopers = 11;
constexpr static I18n::Message s_developersUsernames[s_numberOfDevelopers] = {
  I18n::Message::PQuentinGuidee,
  I18n::Message::PSandraSimmons,
  I18n::Message::PJoachimLeFournis,
  I18n::Message::PJeanBaptisteBoric,
  I18n::Message::PMaximeFriess,
  I18n::Message::PDavid,
  I18n::Message::PDamienNicolet,
  I18n::Message::PEvannDreumont,
  I18n::Message::PSzaboLevente,
  I18n::Message::PVenceslasDuet,
  I18n::Message::PCharlotteThomas,
};

constexpr static int s_numberOfBetaTesters = 4;

constexpr static I18n::Message s_betaTestersUsernames[s_numberOfBetaTesters] = {
  I18n::Message::PCyprienMejat,
  I18n::Message::PTimeoArnouts,
  I18n::Message::PLouisC,
  I18n::Message::PLelahelHideux,
};

void ContributorsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithBuffer * myTextCell = (MessageTableCellWithBuffer *)cell;
  if (index == 0) {
    myTextCell->setAccessoryText("");
    myTextCell->setTextColor(KDColor::RGB24(0xC03535));
  } else if (index > 0 && index <= s_numberOfDevelopers) {
    myTextCell->setAccessoryText(I18n::translate(s_developersUsernames[index - 1]));
    myTextCell->setTextColor(Palette::PrimaryText);
  } else if (index == s_numberOfDevelopers + 1) {
    myTextCell->setAccessoryText("");
    myTextCell->setTextColor(KDColor::RGB24(0x1ABC9A));
  } else {
    myTextCell->setAccessoryText(I18n::translate(s_betaTestersUsernames[index - 2 - s_numberOfDevelopers]));
    myTextCell->setTextColor(Palette::PrimaryText);
  }
  myTextCell->setAccessoryTextColor(Palette::SecondaryText);
  GenericSubController::willDisplayCellForIndex(cell, index);
}

}
