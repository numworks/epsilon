#include "store_menu_controller.h"
#include <escher/clipboard.h>
#include <escher/invocation.h>
#include "shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;
using namespace Ion;
using namespace Escher;

StoreMenuController::InnerListController::InnerListController(StoreMenuController * dataSource, SelectableTableViewDelegate * delegate) :
  ViewController(dataSource),
  m_selectableTableView(this, dataSource, dataSource, delegate)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void StoreMenuController::InnerListController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  m_selectableTableView.reloadData();
}

StoreMenuController::StoreMenuController() :
  ModalViewController(this, &m_stackViewController),
  m_stackViewController(nullptr, &m_listController, StackViewController::Style::PurpleWhite),
  m_listController(this),
  m_cell(this, nullptr, this, this),
  m_abortController(
    Invocation([](void * context, void * sender) {
      StoreMenuController * storeMenu = static_cast<StoreMenuController*>(context);
      storeMenu->dismissModalViewController();
      Container::activeApp()->dismissModalViewController();
      return true;
    }, this),
    Invocation([](void * context, void * sender) {
      StoreMenuController * storeMenu = static_cast<StoreMenuController*>(context);
      storeMenu->dismissModalViewController();
      return true;
    }, this),
    I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel
    ),
  m_isUpdating(false)
{
  m_abortController.setContentMessage(I18n::Message::InvalidInputWarning);
}

void StoreMenuController::viewDidDisappear() {
  m_cell.expressionField()->tidy();
}

bool StoreMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back || event == Ion::Events::EXE || event == Ion::Events::OK) {
    Container::activeApp()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Sto) {
    m_cell.expressionField()->handleEventWithText("→");
    return true;
  }
  return false;
}

void StoreMenuController::setup() {
  const char * text = Escher::Clipboard::sharedStoreBuffer()->storedText();
  m_isUpdating = true;
  m_cell.expressionField()->setEditing(true);
  m_cell.expressionField()->setText(text);
  m_cell.expressionField()->handleEventWithText("→");
  m_isUpdating = false;
}

void StoreMenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  m_cell.reloadCell();
}

void StoreMenuController::layoutFieldDidChangeSize(LayoutField * layoutField) {
  if (!m_isUpdating) {
    m_isUpdating = true;
    m_listController.selectableTableView()->reloadData();
    // m_listController.selectableTableView()->setFrame(m_listController.selectableTableView()->bounds(), true);
  }
  m_isUpdating = false;
}

bool StoreMenuController::layoutFieldShouldFinishEditing(Escher::LayoutField * layoutField, Ion::Events::Event event) {
  if (event != Ion::Events::OK && event != Ion::Events::EXE) {
    return false;
  }
  PoincareHelpers::ParseAndSimplify(m_cell.expressionField()->text(), Container::activeApp()->localContext());
  // TODO move warning here
  /* We need to dismiss the controller but it can't be done here so we set a
   * flag a catch the event later. */
  // We are already in a popup, we only need the bottom margin
  // displayModalViewController(&m_abortController, 0.f, 0.f, 0, 0, Escher::Metric::PopUpBottomMargin, 0);
  return false;
}

bool StoreMenuController::layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) {
  // Container::activeApp()->dismissModalViewController();
  return true;
}

void StoreMenuController::didBecomeFirstResponder() {
  resetMemoization();
  Container::activeApp()->setFirstResponder(&m_listController);
  // Additional outputs should have at least one row to display
  assert(numberOfRows() > 0);
}

