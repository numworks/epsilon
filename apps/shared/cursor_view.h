#ifndef SHARED_CURSOR_VIEW_H
#define SHARED_CURSOR_VIEW_H

#include <escher/transparent_view.h>

namespace Shared {

class CursorView : public Escher::TransparentView {
public:
  virtual void setCursorFrame(KDRect frame, bool force) { Escher::View::setFrame(frame, force); }
};

}

#endif
