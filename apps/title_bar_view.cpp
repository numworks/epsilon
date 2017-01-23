#include "title_bar_view.h"
extern "C" {
#include <assert.h>
}

TitleBarView::TitleBarView() :
  View(),
  m_titleView(KDText::FontSize::Small, nullptr, 0.5f, 0.5f, KDColorWhite, Palette::YellowOne)
{
}

void TitleBarView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::YellowOne);

}

void TitleBarView::setTitle(const char * title) {
  m_titleView.setText(title);
}

void TitleBarView::setChargeState(Ion::Battery::Charge chargeState) {
  m_batteryView.setChargeState(chargeState);
}

int TitleBarView::numberOfSubviews() const {
  return 2;
}

View * TitleBarView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_titleView;
  }
  return &m_batteryView;
}

void TitleBarView::layoutSubviews() {
  m_titleView.setFrame(bounds());
  KDSize batterySize = m_batteryView.minimalSizeForOptimalDisplay();
  m_batteryView.setFrame(KDRect(bounds().width() - batterySize.width() - k_batteryLeftMargin, (bounds().height()- batterySize.height())/2, batterySize));
}
