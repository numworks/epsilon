#include "pie_graph_controller.h"

#include <apps/i18n.h>

namespace Statistics::Categorical {

PieGraphController::PieGraphController(
    Escher::Responder* parentResponder, Escher::ButtonRowController* header,
    Escher::TabViewController* tabController,
    Escher::StackViewController* stackViewController,
    Escher::ViewController* typeViewController, Store* store)
    : Escher::ViewController(parentResponder),
      GraphButtonRowDelegate(header, stackViewController, this,
                             typeViewController),
      m_view(store),
      m_displayedDataButton(this, I18n::Message::DisplayedData,
                            Escher::Invocation::Builder<PieGraphController>(
                                [](PieGraphController* delegate, void* sender) {
                                  delegate->pushDataSelectionMenu();
                                  return true;
                                },
                                this),
                            KDFont::Size::Small),
      m_displayedDataController(this, tabController, stackViewController,
                                store),
      m_store(store),
      m_tabController(tabController),
      m_isPieGraphSelected(true) {}

void PieGraphController::initView() {
  if (!m_store->isGroupActive(m_store->getSelectedGroupForPieGraph())) {
    for (uint8_t group = 0; group < Store::k_maxNumberOfGroups; group++) {
      if (m_store->isGroupActive(group)) {
        m_store->setSelectedGroupForPieGraph(group);
        return;
      }
    }
    OMG::unreachable();
  }
}

void PieGraphController::viewWillAppear() {
  assert(m_store->isGroupActive(m_store->getSelectedGroupForPieGraph()));
  int group = m_store->getSelectedGroupForPieGraph();
  m_view.setGroup(group, m_store);
  m_isPieGraphSelected = true;
  m_view.toggleSelection(m_isPieGraphSelected);
}

int PieGraphController::numberOfButtons(
    Escher::ButtonRowController::Position position) const {
  assert(position == Escher::ButtonRowController::Position::Top);
  bool displayedDataButton = m_store->numberOfActiveGroups() >= 2;
  return displayedDataButton +
         GraphButtonRowDelegate::numberOfButtons(position);
}
Escher::ButtonCell* PieGraphController::buttonAtIndex(
    int index, Escher::ButtonRowController::Position position) const {
  assert(position == Escher::ButtonRowController::Position::Top);
  if (index == 1) {
    return const_cast<Escher::SimpleButtonCell*>(&m_displayedDataButton);
  }
  return GraphButtonRowDelegate::buttonAtIndex(index, position);
}

void PieGraphController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (!m_isPieGraphSelected) {
      header()->setSelectedButton(0);
    }
  }
}

bool PieGraphController::handleEvent(Ion::Events::Event event) {
  if (header()->selectedButton() >= 0) {
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::Down) {
      header()->setSelectedButton(-1);
      m_isPieGraphSelected = true;
      m_view.toggleSelection(m_isPieGraphSelected);
      Escher::App::app()->setFirstResponder(this);
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up || event == Ion::Events::Back) {
    m_isPieGraphSelected = false;
    m_view.toggleSelection(m_isPieGraphSelected);
    if (event == Ion::Events::Up) {
      header()->setSelectedButton(0);
    } else {
      m_tabController->selectTab();
    }
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    m_view.nextActiveCategory(event == Ion::Events::Right ? 1 : -1);
    return true;
  }
  return false;
}

PieGraphController::ContentView::ContentView(Store* store)
    : m_groupTitleView({.style = {.font = KDFont::Size::Small},
                        .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_pieGraphView(store),
      m_sideBannerView(store) {}

void PieGraphController::ContentView::setGroup(int group, Store* store) {
  char buffer[sizeof(Store::Label)];
  store->getGroupName(group, buffer, sizeof(buffer));
  m_groupTitleView.setText(buffer);
  int selectedCategory = m_pieGraphView.setGroup(group);
  m_sideBannerView.setGroup(group, selectedCategory);
  reload();
}

void PieGraphController::ContentView::toggleSelection(bool isSelected) {
  m_pieGraphView.toggleSelection(isSelected);
  m_sideBannerView.toggleSelection(isSelected);
  reload();
}

void PieGraphController::ContentView::nextActiveCategory(int direction) {
  int selectedCategory = m_pieGraphView.nextCategory(direction);
  m_sideBannerView.setCategory(selectedCategory);
  reload();
}

void PieGraphController::ContentView::layoutSubviews(bool force) {
  KDRect b = bounds();
  /* Almost like a square, but not quite to make it simple to draw the pie
   * graph. See implementation and comments in [PieGraphView] */
  const int leftSideWidth = b.height() + 5 - b.height() % 5;
  constexpr int k_titleHeight = 25;
  setChildFrame(&m_groupTitleView, KDRect(0, 0, leftSideWidth, k_titleHeight),
                force);
  setChildFrame(
      &m_pieGraphView,
      KDRect(0, k_titleHeight, leftSideWidth, b.height() - k_titleHeight),
      force);
  setChildFrame(&m_sideBannerView,
                KDRect(leftSideWidth, 0, b.width() - leftSideWidth, b.height()),
                force);
}

Escher::View* PieGraphController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_groupTitleView;
    case 1:
      return &m_pieGraphView;
    default:
      assert(index == 2);
      return &m_sideBannerView;
  }
}

}  // namespace Statistics::Categorical
