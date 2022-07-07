#include "main_controller.h"

namespace Periodic {

// MainController::ContentView

Escher::View * MainController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_elementsView;
  }
  assert(index == 1);
  return &m_bannerView;
}

void MainController::ContentView::layoutSubviews(bool force) {
  KDSize bannerSize = m_bannerView.minimalSizeForOptimalDisplay();
  m_bannerView.setFrame(KDRect(0, bounds().height() - bannerSize.height(), bounds().width(), bannerSize.height()), force);
  m_elementsView.setFrame(KDRect(0, 0, bounds().width(), bounds().height() - bannerSize.height()), force);
}

// MainController

bool MainController::handleEvent(Ion::Events::Event e) {
  bool cursorMoved = false;
  if (e == Ion::Events::Up) {
    cursorMoved = m_view.elementsView()->moveCursorVertically(false);
  } else if (e == Ion::Events::Down) {
    cursorMoved = m_view.elementsView()->moveCursorVertically(true);
  } else if (e == Ion::Events::Left) {
    cursorMoved = m_view.elementsView()->moveCursorHorizontally(false);
  } else if (e == Ion::Events::Right) {
    cursorMoved = m_view.elementsView()->moveCursorHorizontally(true);
  }
  if (cursorMoved) {
    m_view.bannerView()->reload();
  }
  return cursorMoved;
}

}
