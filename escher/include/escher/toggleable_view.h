#ifndef ESCHER_TOGGLEABLE_VIEW_H
#define ESCHER_TOGGLEABLE_VIEW_H

#include <escher/transparent_view.h>

namespace Escher {

class Toggleable {
 public:
  Toggleable() : m_state(true) {}
  bool state() const { return m_state; }
  void setState(bool state);

 protected:
  bool m_state;
};

class ToggleableView : public TransparentView, public Toggleable {
 public:
  ToggleableView(View* superview) : TransparentView(superview) {}
  void setState(bool state) {
    Toggleable::setState(state);
    markRectAsDirty(bounds());
  }
};

}  // namespace Escher
#endif
