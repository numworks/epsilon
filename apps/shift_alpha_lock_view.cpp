#include "shift_alpha_lock_view.h"

ShiftAlphaLockView::ShiftAlphaLockView() :
  View(),
  m_shiftAlphaView(KDFont::SmallFont, I18n::Message::Default, 1.0f, 0.5f, Palette::ToolbarText, Palette::Toolbar),
  m_status(Ion::Events::ShiftAlphaStatus::Default)
{
}

void ShiftAlphaLockView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::Toolbar);
}

bool ShiftAlphaLockView::setStatus(Ion::Events::ShiftAlphaStatus status) {
  if (status != m_status) {
    m_status = status;
    switch (m_status) {
      case Ion::Events::ShiftAlphaStatus::Alpha:
      case Ion::Events::ShiftAlphaStatus::AlphaLock:
        m_shiftAlphaView.setMessage(I18n::Message::Alpha);
        break;
      case Ion::Events::ShiftAlphaStatus::ShiftAlpha:
      case Ion::Events::ShiftAlphaStatus::ShiftAlphaLock:
        m_shiftAlphaView.setMessage(I18n::Message::CapitalAlpha);
        break;
      case Ion::Events::ShiftAlphaStatus::Shift:
        m_shiftAlphaView.setMessage(I18n::Message::Shift);
        break;
      case Ion::Events::ShiftAlphaStatus::Default:
        m_shiftAlphaView.setMessage(I18n::Message::Default);
        break;
    }
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

KDSize ShiftAlphaLockView::minimalSizeForOptimalDisplay() const {
  KDSize modifierSize = KDFont::SmallFont->stringSize(I18n::translate(I18n::Message::Alpha));
  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  KDCoordinate height = lockSize.height() > modifierSize.height() ? lockSize.height() : modifierSize.height();
  return KDSize(modifierSize.width() + lockSize.width() + k_lockRightMargin, height);
}

int ShiftAlphaLockView::numberOfSubviews() const {
  switch (m_status) {
    case Ion::Events::ShiftAlphaStatus::Alpha:
    case Ion::Events::ShiftAlphaStatus::Shift:
    case Ion::Events::ShiftAlphaStatus::ShiftAlpha:
      return 1;
    case Ion::Events::ShiftAlphaStatus::AlphaLock:
    case Ion::Events::ShiftAlphaStatus::ShiftAlphaLock:
      return 2;
    case Ion::Events::ShiftAlphaStatus::Default:
      return 0;
  }
  return 0;
}

View * ShiftAlphaLockView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_shiftAlphaView;
  }
  return &m_lockView;
}

void ShiftAlphaLockView::layoutSubviews(bool force) {
  KDSize modifierSize = KDFont::SmallFont->stringSize(I18n::translate(I18n::Message::Alpha));
  m_shiftAlphaView.setFrame(KDRect(bounds().width() - modifierSize.width(), (bounds().height()- modifierSize.height())/2, modifierSize), force);

  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  m_lockView.setFrame(KDRect(bounds().width() - modifierSize.width() - lockSize.width() - k_lockRightMargin, (bounds().height()- lockSize.height())/2, lockSize), force);
}
