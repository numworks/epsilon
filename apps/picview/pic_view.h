#ifndef PICVIEW_PIC_VIEW_H
#define PICVIEW_PIC_VIEW_H

#include <escher.h>

class PicView : public View {
public:
  PicView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

#endif
