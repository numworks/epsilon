#include "title_bar_view.h"
extern "C" {
#include <assert.h>
}

using namespace Poincare;

TitleBarView::TitleBarView() :
  View(),
  m_titleView(KDText::FontSize::Small, nullptr, 0.5f, 0.5f, KDColorWhite, Palette::YellowDark),
  m_preferenceView(KDText::FontSize::Small, 1.0f, 0.5, KDColorWhite, Palette::YellowDark)
{
}

void TitleBarView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::YellowDark);

}

void TitleBarView::setTitle(const char * title) {
  m_titleView.setText(title);
}

void TitleBarView::setChargeState(Ion::Battery::Charge chargeState) {
  m_batteryView.setChargeState(chargeState);
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

void TitleBarView::setPreferences(Preferences * preferences) {
  char buffer[13];
  int numberOfChar = 0;
  if (preferences->displayMode() == Expression::DisplayMode::Scientific) {
    strlcpy(buffer, "sci/", 5);
    numberOfChar += 4;
  }
  if (preferences->numberType() == Preferences::NumberType::Complex) {
    strlcpy(buffer+numberOfChar, "cplx/", 6);
    numberOfChar += 5;
  }
  if (preferences->angleUnit() == Expression::AngleUnit::Radian) {
    strlcpy(buffer+numberOfChar, "rad", 4);
  } else {
    strlcpy(buffer+numberOfChar, "deg", 4);
  }
  numberOfChar += 3;
  buffer[numberOfChar] = 0;
  m_preferenceView.setText(buffer);
  layoutSubviews();
}
