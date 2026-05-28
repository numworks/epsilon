#include "apps_window.h"

#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

using namespace Escher;

AppsWindow::AppsWindow(FocusManager* focusManager)
    : Window(),
      m_titleBarView(),
      m_focusManager(focusManager),
      m_hideTitleBarView(false) {
  assert(focusManager);
}

void AppsWindow::setTitle(I18n::Message title) {
  m_titleBarView.setTitle(title);
}

bool AppsWindow::updateBatteryLevel() {
  return m_titleBarView.setChargeState(Ion::Battery::level());
}

void AppsWindow::updateBatteryAnimation() {
  return m_titleBarView.updateBatteryAnimation();
}

bool AppsWindow::updateIsChargingState() {
  return m_titleBarView.setIsCharging(Ion::Battery::isCharging());
}

bool AppsWindow::updatePluggedState() {
  return m_titleBarView.setIsPlugged(Ion::USB::isPlugged());
}

void AppsWindow::refreshPreferences() { m_titleBarView.refreshPreferences(); }

void AppsWindow::reloadTitleBarView() { m_titleBarView.reload(); }

bool AppsWindow::updateAlphaLock() {
  return m_titleBarView.setShiftAlphaLockStatus(
      Ion::Events::shiftAlphaStatus());
}

void AppsWindow::hideTitleBarView(bool hide) {
  if (m_hideTitleBarView != hide) {
    m_hideTitleBarView = hide;
    layoutSubviews();
  }
}

int AppsWindow::numberOfSubviews() const {
  return 2 + (m_contentView != nullptr);
}

View* AppsWindow::subviewAtIndex(int index) {
  assert(index < numberOfSubviews());
  if (index == 0) {
    return &m_titleBarView;
  } else if (index == 1) {
    return m_contentView ? m_contentView : m_focusManager;
  }
  assert(index == 2);
  return m_focusManager;
}

void AppsWindow::layoutSubviews(bool force) {
  KDCoordinate titleHeight = m_hideTitleBarView ? 0 : Metric::TitleBarHeight;
  setChildFrame(&m_titleBarView, KDRect(0, 0, bounds().width(), titleHeight),
                force);
  if (m_contentView != nullptr) {
    setChildFrame(m_contentView,
                  KDRect(0, titleHeight, bounds().width(),
                         bounds().height() - titleHeight),
                  force);
  }
  setChildFrame(m_focusManager, Ion::Display::Rect, force);
}

#if ESCHER_VIEW_LOGGING
const char* AppsWindow::className() const { return "Window"; }
#endif
