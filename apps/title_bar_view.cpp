#include "title_bar_view.h"
extern "C" {
#include <assert.h>
}
#include <poincare.h>

using namespace Poincare;

TitleBarView::TitleBarView() :
  View(),
  m_titleView(KDText::FontSize::Small, I18n::Message::Default, 0.5f, 0.5f, KDColorWhite, Palette::YellowDark),
  m_preferenceView(KDText::FontSize::Small, 1.0f, 0.5, KDColorWhite, Palette::YellowDark)
{
}

void TitleBarView::setTitle(I18n::Message title) {
  m_titleView.setMessage(title);
}

bool TitleBarView::setChargeState(Ion::Battery::Charge chargeState) {
  return m_batteryView.setChargeState(chargeState);
}

bool TitleBarView::setIsCharging(bool isCharging) {
  return m_batteryView.setIsCharging(isCharging);
}

int TitleBarView::numberOfSubviews() const {
  return 3;
}

View * TitleBarView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_titleView;
  }
  if (index == 1) {
    return &m_preferenceView;
  }
  return &m_batteryView;
}

void TitleBarView::layoutSubviews() {
  m_titleView.setFrame(bounds());
  m_preferenceView.setFrame(KDRect(0, 0, m_preferenceView.minimalSizeForOptimalDisplay()));
  KDSize batterySize = m_batteryView.minimalSizeForOptimalDisplay();
  m_batteryView.setFrame(KDRect(bounds().width() - batterySize.width() - k_batteryLeftMargin, (bounds().height()- batterySize.height())/2, batterySize));
}

void TitleBarView::refreshPreferences() {
  char buffer[13];
  int numberOfChar = 0;
  if (Preferences::sharedPreferences()->displayMode() == Expression::FloatDisplayMode::Scientific) {
    strlcpy(buffer, I18n::translate(I18n::Message::Sci), strlen(I18n::translate(I18n::Message::Sci))+1);
    numberOfChar += strlen(I18n::translate(I18n::Message::Sci));
  }
  if (Preferences::sharedPreferences()->angleUnit() == Expression::AngleUnit::Radian) {
    strlcpy(buffer+numberOfChar, I18n::translate(I18n::Message::Rad), strlen(I18n::translate(I18n::Message::Rad))+1);
    numberOfChar += strlen(I18n::translate(I18n::Message::Rad));
  } else {
    strlcpy(buffer+numberOfChar, I18n::translate(I18n::Message::Deg), strlen(I18n::translate(I18n::Message::Sci))+1);
    numberOfChar += strlen(I18n::translate(I18n::Message::Deg));
  }
  buffer[numberOfChar] = 0;
  m_preferenceView.setText(buffer);
  layoutSubviews();
}
