#include "store_menu_controller.h"

#include <escher/clipboard.h>
#include <escher/invocation.h>
#include <poincare/store.h>

#include "app_with_store_menu.h"
#include "expression_display_permissions.h"
#include "poincare_helpers.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

StoreMenuController::InnerListController::InnerListController(
    StoreMenuController* dataSource)
    : ViewController(dataSource),
      m_selectableListView(this, dataSource, dataSource) {
  m_selectableListView.resetMargins();
  m_selectableListView.hideScrollBars();
}

void StoreMenuController::InnerListController::didBecomeFirstResponder() {
  m_selectableListView.selectCell(0);
  m_selectableListView.reloadData();
}

StoreMenuController::StoreMenuController()
    : ModalViewController(this, &m_stackViewController),
      m_stackViewController(nullptr, &m_listController,
                            StackViewController::Style::PurpleWhite, false),
      m_listController(this),
      m_cell(this, this),
      m_abortController(Invocation::Builder<StoreMenuController>(
                            [](StoreMenuController* storeMenu, void* sender) {
                              /* Close the warning and then the store menu which
                               * are both modals */
                              storeMenu->dismissModal();
                              storeMenu->close();
                              return true;
                            },
                            this),
                        Invocation::Builder<StoreMenuController>(
                            [](StoreMenuController* storeMenu, void* sender) {
                              storeMenu->dismissModal();
                              return true;
                            },
                            this),
                        I18n::Message::Warning, I18n::Message::Ok,
                        I18n::Message::Cancel),
      m_preventReload(false) {
  m_abortController.setContentMessage(I18n::Message::InvalidInputWarning);
  /* We need to set the width early since minimalSizeForOptimalDisplay will be
   * called before willDisplayCell. */
  view()->setChildFrame(
      &m_cell,
      KDRect(0, 0, Ion::Display::Width - Metric::PopUpMargins.width(), 0),
      false);
  m_cell.layoutField()->setTextEditionBuffer(
      m_savedDraftTextBuffer, AbstractTextField::MaxBufferSize());
}

void StoreMenuController::didBecomeFirstResponder() {
  App::app()->setFirstResponder(&m_listController);
  m_cell.layoutField()->reload();
}

void StoreMenuController::setText(const char* text) {
  m_preventReload = true;
  m_cell.layoutField()->clearAndSetEditing(true);
  m_cell.layoutField()->handleEventWithText(text, false, true);
  m_cell.layoutField()->handleEventWithText("→");
  if (text[0] == 0) {
    m_cell.layoutField()->putCursorOnOneSide(OMG::Direction::Left());
  }
  m_stackViewController.setupActiveView();
  m_preventReload = false;
}

void StoreMenuController::open() {
  App::app()->displayModalViewController(
      this, KDGlyph::k_alignCenter, KDGlyph::k_alignCenter,
      {Metric::PopUpMargins.horizontal(), {}}, true);
}

void StoreMenuController::close() {
  App::app()->modalViewController()->dismissModal();
  m_cell.layoutField()->setEditing(false);
}

void StoreMenuController::fillCellForRow(HighlightCell* cell, int row) {
  m_cell.reloadCell();
}

void StoreMenuController::layoutFieldDidChangeSize(LayoutField* layoutField) {
  assert(layoutField == m_cell.layoutField());
  if (m_preventReload) {
    return;
  }
  /* Reloading the store menu will update its frame to match the size of the
   * layout but it will also call layoutFieldDidChangeSize. We set this
   * boolean to break the cycle. */
  m_preventReload = true;
  App::app()->modalViewController()->reloadModal();
  m_preventReload = false;
}

void StoreMenuController::openAbortWarning() {
  /* We want to open the warning but the current store menu is likely too small
   * to display it entirely. We open the pop-up and then reload which will cause
   * the store menu to be relayouted with the minimalsizeForOptimalDisplay of
   * the warning. We could save a reload by moving the centering logic after the
   * embedded pop-up. */
  displayModalViewController(&m_abortController, KDGlyph::k_alignCenter,
                             KDGlyph::k_alignCenter);
  App::app()->modalViewController()->reloadModal();
}

bool StoreMenuController::parseAndStore(const char* text) {
  AppWithStoreMenu* app = static_cast<AppWithStoreMenu*>(App::app());
  Context* context = app->localContext();
  Expression input = Expression::Parse(text, context);
#if 1  // TODO_PCJ
  assert(false);
  return false;
#else
  if (input.isUninitialized()) {
    openAbortWarning();
    return false;
  }
  Expression reducedExp = input;
  PoincareHelpers::CloneAndSimplify(&reducedExp, context);
  if (reducedExp.type() != ExpressionNode::Type::Store) {
    openAbortWarning();
    return false;
  }
  bool isVariable =
      reducedExp.childAtIndex(1).type() == ExpressionNode::Type::Symbol;
  Expression leftHandSideApproximation =
      PoincareHelpers::ApproximateKeepingUnits<double>(
          reducedExp.childAtIndex(0), context);
  if (isVariable &&
      ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          input, reducedExp.childAtIndex(0), leftHandSideApproximation,
          context)) {
    reducedExp.replaceChildAtIndexInPlace(0, leftHandSideApproximation);
  }
  Store store = static_cast<Store&>(reducedExp);
  close();
  app->prepareForIntrusiveStorageChange();
  bool storeImpossible = !store.storeValueForSymbol(context);
  app->concludeIntrusiveStorageChange();
  if (storeImpossible) {
    /* TODO: we could detect this before the close and open the warning over the
     * store menu */
    app->displayWarning(I18n::Message::VariableCantBeEdited);
  }
  return true;
#endif
}

bool StoreMenuController::layoutFieldDidFinishEditing(
    Escher::LayoutField* layoutField, Ion::Events::Event event) {
  assert(layoutField == m_cell.layoutField());
  assert(!layoutField->isEditing());
  constexpr size_t bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];
  Layout layout = layoutField->layout();
  layout.serializeForParsing(buffer, bufferSize);
  if (parseAndStore(buffer)) {
    layoutField->clearLayout();
    return true;
  }
  return false;
}

void StoreMenuController::layoutFieldDidAbortEditing(
    Escher::LayoutField* layoutField) {
  assert(layoutField == m_cell.layoutField());
  /* Since dismissing the controller will call layoutFieldDidChangeSize, we need
   * to set the flag to avoid reloadData from happening which would otherwise
   * setFirstResponder on the store menu while it is hidden. */
  m_preventReload = true;
  close();
}

}  // namespace Shared
