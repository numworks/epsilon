#ifndef APPS_ALPHA_LOCK_VIEW_H
#define APPS_ALPHA_LOCK_VIEW_H

#include <escher.h>
#include "lock_view.h"
#include "i18n.h"

class AlphaLockView : public View {
public:
  AlphaLockView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  bool setStatus(Ion::Events::ShiftAlphaStatus status);
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  constexpr static KDCoordinate k_lockRightMargin = 5;
  int numberOfSubviews() const override;
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  LockView m_lockView;
  MessageTextView m_modifierView;
  Ion::Events::ShiftAlphaStatus m_status;
};

#endif
