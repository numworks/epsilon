#include "interest_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

#include "app.h"

using namespace Finance;

InterestMenuController::InterestMenuController(
    Escher::StackViewController* parentResponder,
    InterestController* interestController)
    : Escher::SelectableCellListPage<
          Escher::MessageTableCellWithChevronAndMessage,
          k_numberOfInterestCells, Escher::MemoizedListViewDataSource>(
          parentResponder, &m_contentView),
      m_messageView(KDFont::Size::Small, I18n::Message::ParameterChoose,
                    KDContext::k_alignCenter, KDContext::k_alignCenter,
                    Escher::Palette::GrayDark, Escher::Palette::WallScreen),
      m_contentView(&m_selectableListView, this, &m_messageView),
      m_interestController(interestController) {
  selectRow(0);
}

void InterestMenuController::didBecomeFirstResponder() {
  resetMemoization(true);
  int nRows = numberOfRows();
  for (int i = 0; i < nRows; i++) {
    cellAtIndex(i)->setMessage(
        App::GetInterestData()->labelForParameter(paramaterAtIndex(i)));
    cellAtIndex(i)->setSubtitle(
        App::GetInterestData()->sublabelForParameter(paramaterAtIndex(i)));
  }
  m_contentView.reload();
}

bool InterestMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    App::GetInterestData()->setUnknown(paramaterAtIndex(selectedRow()));
    stackOpenPage(m_interestController);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

const char* InterestMenuController::title() {
  return I18n::translate(App::GetInterestData()->menuTitle());
}

int InterestMenuController::numberOfRows() const {
  return App::GetInterestData()->numberOfUnknowns();
}

uint8_t InterestMenuController::paramaterAtIndex(int index) const {
  // Parameters are displayed in the same order as the enum order.
  assert(index >= 0 && index < App::GetInterestData()->numberOfUnknowns());
  return index;
}
