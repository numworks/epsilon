#include <escher/nested_menu_controller.h>
#include <assert.h>
#include <string.h>

namespace Escher {

/* Breadcrumb Controller */

void NestedMenuController::BreadcrumbController::popTitle() {
  assert(m_titleCount > 0);
  m_titleCount--;
  updateTitle();
}

void NestedMenuController::BreadcrumbController::pushTitle(I18n::Message title) {
  assert(m_titleCount < k_maxModelTreeDepth);
  m_titles[m_titleCount] = title;
  m_titleCount++;
  updateTitle();
}

void NestedMenuController::BreadcrumbController::resetTitle() {
  m_titleCount = 0;
  updateTitle();
}

void NestedMenuController::BreadcrumbController::updateTitle() {
  // m_titleCount == 0 is handled and only sets m_titleBuffer[0] to 0
  constexpr int separatorLength = 3;
  constexpr char separator[] = " > ";
  // Define, from right to left, which subtitles will fit in the breadcrumb
  int titleLength = -separatorLength;
  int firstFittingSubtitleIndex = 0;
  for (int i = m_titleCount - 1; i >= 0; --i) {
    const int subtitleLength = strlen(I18n::translate(m_titles[i]));
    titleLength += separatorLength + subtitleLength;
    if (titleLength > k_maxTitleLength) {
      // This subtitle does not fit
      firstFittingSubtitleIndex = i + 1;
      break;
    }
  }
  // At least one subtitle should fit
  assert(m_titleCount == 0 || firstFittingSubtitleIndex < m_titleCount);
  // Build, from left to right, breadcrumb title from subtitles and separators
  int charIndex = 0;
  for (int i = firstFittingSubtitleIndex; i < m_titleCount; ++i) {
    // Separator ( only after first subtitle )
    if (i > firstFittingSubtitleIndex) {
      memcpy(m_titleBuffer + charIndex, separator, separatorLength);
      charIndex += separatorLength;
    }
    // Subtitle
    const char * subtitle = I18n::translate(m_titles[i]);
    const int subtitleLength = strlen(subtitle);
    memcpy(m_titleBuffer + charIndex, subtitle, subtitleLength);
    charIndex += subtitleLength;
  }
  assert(charIndex <= k_maxTitleLength);
  m_titleBuffer[charIndex] = 0;
}

/* List Controller */

void NestedMenuController::ListController::didBecomeFirstResponder() {
  m_selectableTableView->reloadData();
  Container::activeApp()->setFirstResponder(m_selectableTableView);
  m_selectableTableView->selectCellAtLocation(0, 0);
}

/* NestedMenuController */

NestedMenuController::NestedMenuController(Responder * parentResponder, I18n::Message title) :
  StackViewController(parentResponder, &m_listController, StackViewController::Style::PurpleWhite),
  m_selectableTableView(&m_listController, this, this, this),
  m_breadcrumbController(this, &m_selectableTableView),
  m_listController(this, &m_selectableTableView, title),
  m_lastState(0),
  m_savedChecksum(0)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
  /* Title and breadcrumb headers should not overlap. Breadcrumb should.
   * Using default tableCell's border color. */
  setupHeadersBorderOverlaping(false, true, Palette::GrayBright);
}

void NestedMenuController::viewWillAppear() {
  // Reset memoization first, so that the right cells are manipulated
  resetMemoization();
  StackViewController::viewWillAppear();
  m_selectableTableView.reloadData();

  // Load last state
  int checksum = controlChecksum();
  if (checksum != m_savedChecksum) {
    m_savedChecksum = checksum;
    returnToRootMenu();
  } else if (numberOfRows() == 0) {
    returnToRootMenu();
  } else {
    loadState(m_lastState);
  }
}

void NestedMenuController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
  m_selectableTableView.deselectTable();
}

HighlightCell * NestedMenuController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == k_leafCellType) {
    return leafCellAtIndex(index);
  }
  return nodeCellAtIndex(index);
}

bool NestedMenuController::handleEvent(Ion::Events::Event event) {
  const int rowIndex = selectedRow();
  if ((event == Ion::Events::Back || event == Ion::Events::Left) && stackDepth() > 0) {
    return returnToPreviousMenu();
  }
  if ((event == Ion::Events::Toolbox && isToolbox()) || (event == Ion::Events::Var && !isToolbox())) {
    if (stackDepth() == 0) {
      Container::activeApp()->modalViewController()->dismissModal();
      return true;
    }
    return returnToRootMenu();
  }
  if ((event == Ion::Events::Var && isToolbox()) || (event == Ion::Events::Toolbox && !isToolbox())) {
    Container::activeApp()->modalViewController()->dismissModal();
    assert(sender());
    return sender()->handleBoxEvent(event);
  }
  if (selectedRow() < 0) {
    return false;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && typeAtIndex(rowIndex) == k_nodeCellType) {
    return selectSubMenu(rowIndex);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && typeAtIndex(rowIndex) == k_leafCellType) {
    return selectLeaf(rowIndex);
  }

  return false;
}

bool NestedMenuController::selectSubMenu(int selectedRow) {
  resetMemoization();
  int previousDepth = stackDepth();
  m_stack.push(StackState(selectedRow, m_selectableTableView.contentOffset().y()));

  /* Unless breadcrumb wasn't visible (depth 0), we need to pop it first to push
   * it again, in order to force title refresh. */
  if (previousDepth != 0) {
    StackViewController::pop();
  }
  m_breadcrumbController.pushTitle(subTitle());
  StackViewController::push(&m_breadcrumbController);

  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}

bool NestedMenuController::returnToPreviousMenu() {
  resetMemoization();
  int previousDepth = stackDepth();
  NestedMenuController::StackState state = m_stack.stackPop();

  /* Unless breadcrumb is no longer visible (depth 1), we need to pop it first,
   * to push it again in order to force title refresh. */
  StackViewController::pop();
  m_breadcrumbController.popTitle();
  if (previousDepth != 1) {
    StackViewController::push(&m_breadcrumbController);
  }

  loadState(state);

  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}

bool NestedMenuController::returnToRootMenu() {
  resetMemoization();
  if (stackDepth() > 0) {
    // Reset breadcrumb and stack
    m_stack.reset();
    m_breadcrumbController.resetTitle();
    StackViewController::pop();
  }
  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}

void NestedMenuController::loadState(NestedMenuController::StackState state) {
  bool isStateValid = state.selectedRow() < numberOfRows();
  m_selectableTableView.selectCellAtLocation(0, isStateValid ? state.selectedRow() : 0);
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), isStateValid ? state.verticalScroll() : 0));
}

void NestedMenuController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (selectedRow() >= 0) {
    m_lastState = currentState(); // Persist current state
  }
}

void NestedMenuController::open() {
  Container::activeApp()->displayModalViewController(this, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
}

}
