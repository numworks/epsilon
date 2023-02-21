#ifndef APPS_SHIFT_ALPHA_LOCK_VIEW_H
#define APPS_SHIFT_ALPHA_LOCK_VIEW_H

#include <apps/i18n.h>
#include <escher/message_text_view.h>
#include <ion.h>

#include "lock_view.h"

class ShiftAlphaLockView : public Escher::View {
 public:
  ShiftAlphaLockView();
  void drawRect(KDContext* ctx, KDRect rect) const override;
  bool setStatus(Ion::Events::ShiftAlphaStatus status);
  KDSize minimalSizeForOptimalDisplay() const override;

 private:
  constexpr static KDCoordinate k_lockRightMargin = 5;
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  Escher::View* subviewAtIndex(int index) override;
  LockView m_lockView;
  Escher::MessageTextView m_shiftAlphaView;
  Ion::Events::ShiftAlphaStatus m_status;
};

#endif
