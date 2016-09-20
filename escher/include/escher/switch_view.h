#ifndef ESCHER_SWITCH_VIEW_H
#define ESCHER_SWITCH_VIEW_H

#include <escher/childless_view.h>
#include <escher/label_view.h>
#include <escher/palette.h>

class SwitchView : public ChildlessView {
public:
  SwitchView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

#endif
