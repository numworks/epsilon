#include "interest_menu_controller.h"
#include <apps/i18n.h>
#include <escher/container.h>
#include <assert.h>

using namespace Solver;

InterestMenuController::InterestMenuController(Escher::StackViewController * parentResponder, InterestController * interestController, InterestData * data) :
      Shared::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfInterestCells>(parentResponder, &m_contentView),
      m_messageView(KDFont::SmallFont, I18n::Message::ParameterChoose, KDContext::k_alignCenter, KDContext::k_alignCenter, Escher::Palette::GrayDark, Escher::Palette::WallScreen),
      m_contentView(&m_selectableTableView, this, &m_messageView),
      m_interestController(interestController),
      m_data(data) {
  selectRow(0);
}

void InterestMenuController::didBecomeFirstResponder() {
  for (size_t i = 0; i < numberOfRows(); i++) {
    m_cells[i].setMessage(m_data->labelForParameter(paramaterAtIndex(i)));
    m_cells[i].setSubtitle(m_data->sublabelForParameter(paramaterAtIndex(i)));
  }
  resetMemoization();
  m_contentView.reload();
}

bool InterestMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_data->setUnknown(paramaterAtIndex(selectedRow()));
    m_interestController->setData(m_data);
    stackOpenPage(m_interestController, 1);
  } else if (event == Ion::Events::Left) {
    popStackViewControllerParentResponder();
  } else {
    return false;
  }
  return true;
}

uint8_t InterestMenuController::paramaterAtIndex(int index) const {
  // Parameters are displayed in the same order as the enum order.
  assert(index >= 0 && index < m_data->numberOfUnknowns());
  return index;
}
