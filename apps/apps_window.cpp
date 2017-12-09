#include "apps_window.h"
#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

void AppsWindow::hideTitleBarView(bool hide) {
  if (m_hideTitleBarView != hide) {
    m_hideTitleBarView = hide;
    layoutSubviews();
  }
}

int AppsWindow::numberOfSubviews() const {
  return (m_contentView == nullptr ? 1 : 2);
}

View * AppsWindow::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_titleBarView;
  }
  assert(m_contentView != nullptr && index == 1);
  return m_contentView;
}

void AppsWindow::layoutSubviews() {
  KDCoordinate titleHeight = m_hideTitleBarView ? 0 : Metric::TitleBarHeight;
  m_titleBarView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  if (m_contentView != nullptr) {
    m_contentView->setFrame(KDRect(0, titleHeight, bounds().width(), bounds().height()-titleHeight));
  }
}

#if ESCHER_VIEW_LOGGING
const char * AppsWindow::className() const {
  return "Window";
}
#endif
