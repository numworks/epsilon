#include "apps_window.h"
extern "C" {
#include <assert.h>
}

AppsWindow::AppsWindow() :
  Window(),
  m_titleBarView(TitleBarView())
{
}

void AppsWindow::setTitle(I18n::Message title) {
  m_titleBarView.setTitle(title);
}

void AppsWindow::updateBatteryLevel() {
  m_titleBarView.setChargeState(Ion::Battery::level());
}

void AppsWindow::updateIsChargingState() {
  m_titleBarView.setIsCharging(Ion::Battery::isCharging());
}

void AppsWindow::refreshPreferences() {
  m_titleBarView.refreshPreferences();
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
  m_titleBarView.setFrame(KDRect(0, 0, bounds().width(), k_titleBarHeight));
  if (m_contentView != nullptr) {
    m_contentView->setFrame(KDRect(0, k_titleBarHeight, bounds().width(), bounds().height()-k_titleBarHeight));
  }
}

#if ESCHER_VIEW_LOGGING
const char * AppsWindow::className() const {
  return "Window";
}
#endif
