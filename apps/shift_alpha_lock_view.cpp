#include "shift_alpha_lock_view.h"

AlphaLockView::AlphaLockView() :
  View(),
  m_modifierView(KDText::FontSize::Small, I18n::Message::Default, 1.0f, 0.5f, KDColorWhite, Palette::YellowDark),
  m_status(Ion::Events::ShiftAlphaStatus::Default)
{
}

void AlphaLockView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::YellowDark);
}

bool AlphaLockView::setStatus(Ion::Events::ShiftAlphaStatus status) {
  if (status != m_status) {
    m_status = status;
    switch (m_status) {
      case Ion::Events::ShiftAlphaStatus::Alpha:
      case Ion::Events::ShiftAlphaStatus::AlphaLock:
      case Ion::Events::ShiftAlphaStatus::AlphaLockShift:
        m_modifierView.setMessage(I18n::Message::Alpha);
        break;
      case Ion::Events::ShiftAlphaStatus::ShiftAlpha:
      case Ion::Events::ShiftAlphaStatus::ShiftAlphaLock:
        m_modifierView.setMessage(I18n::Message::CapitalAlpha);
        break;
      case Ion::Events::ShiftAlphaStatus::Shift:
        m_modifierView.setMessage(I18n::Message::Shift);
        break;
      case Ion::Events::ShiftAlphaStatus::Default:
        m_modifierView.setMessage(I18n::Message::Default);
        break;
    }
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

KDSize AlphaLockView::minimalSizeForOptimalDisplay() const {
  KDSize modifierSize = KDText::stringSize(I18n::translate(I18n::Message::Alpha), KDText::FontSize::Small);
  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  KDCoordinate height = lockSize.height() > modifierSize.height() ? lockSize.height() : modifierSize.height();
  return KDSize(modifierSize.width() + lockSize.width() + k_lockRightMargin, height);
}

int AlphaLockView::numberOfSubviews() const {
  switch (m_status) {
    case Ion::Events::ShiftAlphaStatus::Alpha:
    case Ion::Events::ShiftAlphaStatus::Shift:
    case Ion::Events::ShiftAlphaStatus::ShiftAlpha:
      return 1;
    case Ion::Events::ShiftAlphaStatus::AlphaLock:
    case Ion::Events::ShiftAlphaStatus::AlphaLockShift:
    case Ion::Events::ShiftAlphaStatus::ShiftAlphaLock:
      return 2;
    case Ion::Events::ShiftAlphaStatus::Default:
      return 0;
  }
}

View * AlphaLockView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_modifierView;
  }
  return &m_lockView;
}

void AlphaLockView::layoutSubviews() {
  KDSize modifierSize = KDText::stringSize(I18n::translate(I18n::Message::Alpha), KDText::FontSize::Small);
  m_modifierView.setFrame(KDRect(bounds().width() - modifierSize.width(), (bounds().height()- modifierSize.height())/2, modifierSize));

  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  m_lockView.setFrame(KDRect(bounds().width() - modifierSize.width() - lockSize.width() - k_lockRightMargin, (bounds().height()- lockSize.height())/2, lockSize));
}
