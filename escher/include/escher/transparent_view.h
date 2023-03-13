#ifndef ESCHER_TRANSPARENT_VIEW_H
#define ESCHER_TRANSPARENT_VIEW_H

#include <escher/view.h>

namespace Escher {

class TransparentView : public View {
 public:
  TransparentView(View* superview) : m_superview(superview) {}

  // TODO remove this one after Cells templatization
  TransparentView() : m_superview(nullptr) {}

 protected:
  void markRectAsDirty(KDRect rect);

 private:
  View* m_superview;
};

}  // namespace Escher
#endif
