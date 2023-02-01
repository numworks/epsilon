#include "apps_window.h"
#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

using namespace Escher;

AppsWindow::AppsWindow() :
  Window(),
  m_titleBarView(),
  m_hideTitleBarView(false)
{
}

void AppsWindow::setTitle(I18n::Message title) {
  m_titleBarView.setTitle(title);
}

bool AppsWindow::updateBatteryLevel() {
  return m_titleBarView.setChargeState(Ion::Battery::level());
}

void AppsWindow::restartLowBatteryAnimationIfNecessary() {
  return m_titleBarView.restartLowBatteryAnimationIfNecessary();
}

bool AppsWindow::updateIsChargingState() {
  return m_titleBarView.setIsCharging(Ion::Battery::isCharging());
}

bool AppsWindow::updatePluggedState() {
  return m_titleBarView.setIsPlugged(Ion::USB::isPlugged());
}

void AppsWindow::refreshPreferences() {
  m_titleBarView.refreshPreferences();
}

void AppsWindow::reloadTitleBarView() {
  m_titleBarView.reload();
}

bool AppsWindow::updateAlphaLock() {
  return m_titleBarView.setShiftAlphaLockStatus(Ion::Events::shiftAlphaStatus());
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

void AppsWindow::layoutSubviews(bool force) {
  KDCoordinate titleHeight = m_hideTitleBarView ? 0 : Metric::TitleBarHeight;
  m_titleBarView.setFrame(KDRect(0, 0, bounds().width(), titleHeight), force);
  if (m_contentView != nullptr) {
    m_contentView->setFrame(KDRect(0, titleHeight, bounds().width(), bounds().height()-titleHeight), force);
  }
}

#if ESCHER_VIEW_LOGGING
const char * AppsWindow::className() const {
  return "Window";
}
#endif
