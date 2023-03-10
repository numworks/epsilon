#ifndef SHARED_CURSOR_VIEW_H
#define SHARED_CURSOR_VIEW_H

#include <escher/transparent_view.h>

namespace Shared {

class CursorView : public Escher::TransparentView {
 public:
  CursorView() : m_highlighted(false) {}
  virtual void setCursorFrame(View* parent, KDRect frame, bool force) {
    parent->setChildFrame(this, frame, force);
  }
  void setHighlighted(bool highlighted) { m_highlighted = highlighted; }
  virtual void setColor(KDColor color) {}

 protected:
  bool isHighlighted() const { return m_highlighted; }

 private:
  bool m_highlighted;
};

}  // namespace Shared

#endif
