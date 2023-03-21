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
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::ChevronView>,
          k_numberOfInterestCells, Escher::MemoizedListViewDataSource>(
          parentResponder),
      m_messageView(I18n::Message::ParameterChoose,
                    {{.glyphColor = Escher::Palette::GrayDark,
                      .backgroundColor = Escher::Palette::WallScreen,
                      .font = KDFont::Size::Small},
                     .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_contentView(&m_selectableListView, this, &m_messageView),
      m_interestController(interestController) {
  selectRow(0);
}

void InterestMenuController::didBecomeFirstResponder() {
  resetMemoization(true);
  int nRows = numberOfRows();
  for (int i = 0; i < nRows; i++) {
    cellAtIndex(i)->label()->setMessage(
        App::GetInterestData()->labelForParameter(paramaterAtIndex(i)));
    cellAtIndex(i)->subLabel()->setMessage(
        App::GetInterestData()->sublabelForParameter(paramaterAtIndex(i)));
  }
  m_contentView.reload();
}

bool InterestMenuController::handleEvent(Ion::Events::Event event) {
  // enterOnEvent can be called on any cell with chevron
  if (cellAtIndex(0)->enterOnEvent(event)) {
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
