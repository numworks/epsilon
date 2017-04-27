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

bool AppsWindow::updateBatteryLevel() {
  return m_titleBarView.setChargeState(Ion::Battery::level());
}

bool AppsWindow::updateIsChargingState() {
  return m_titleBarView.setIsCharging(Ion::Battery::isCharging());
}

void AppsWindow::refreshPreferences() {
  m_titleBarView.refreshPreferences();
}

bool AppsWindow::updateAlphaLock() {
  AlphaLockView::Status alphaLockStatus = AlphaLockView::Status::Default;
  if (Ion::Events::isAlphaLocked()) {
    alphaLockStatus = AlphaLockView::Status::AlphaLock;
  } else if (Ion::Events::isShiftAlphaLocked()) {
    alphaLockStatus = AlphaLockView::Status::CapitalAlphaLock;
  } else if (Ion::Events::isAlphaActive()) {
    if (Ion::Events::isShiftActive()) {
      alphaLockStatus = AlphaLockView::Status::CapitalAlpha;
    } else {
      alphaLockStatus = AlphaLockView::Status::Alpha;
    }
  }
  return m_titleBarView.setAlphaLockStatus(alphaLockStatus);
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
