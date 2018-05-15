#ifndef ESCHER_TRANSPARENT_VIEW_H
#define ESCHER_TRANSPARENT_VIEW_H

#include <escher/view.h>

class TransparentView : public View {
public:
  void markRectAsDirty(KDRect rect) override;
};

#endif
