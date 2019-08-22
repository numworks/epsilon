#include "contributors_controller.h"
#include <assert.h>

namespace Settings {

ContributorsController::ContributorsController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
    m_cells[i].setAccessoryFont(KDFont::SmallFont);
    m_cells[i].setAccessoryTextColor(Palette::GreyDark);
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

void ContributorsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
}

}
