#ifndef ESCHER_TRANSPARENT_VIEW_H
#define ESCHER_TRANSPARENT_VIEW_H

#include <escher/view.h>

namespace Escher {

class TransparentView : public View {
 protected:
  void markRectAsDirty(KDRect rect) override;
};

}  // namespace Escher
#endif
