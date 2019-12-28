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

constexpr static I18n::Message s_contributorsUsernames[7] = {
  I18n::Message::PQuentinGuidee,
  I18n::Message::PDannySimmons,
  I18n::Message::PJoachimLeFournis,
  I18n::Message::PJeanBaptisteBoric,
  I18n::Message::PMaximeFriess,
  I18n::Message::PDavid,
  I18n::Message::PDamienNicolet
};

void ContributorsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithBuffer * myTextCell = (MessageTableCellWithBuffer *)cell;
  myTextCell->setAccessoryText(I18n::translate(s_contributorsUsernames[index]));
  GenericSubController::willDisplayCellForIndex(cell, index);
}

}
