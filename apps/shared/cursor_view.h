#ifndef SHARED_CURSOR_VIEW_H
#define SHARED_CURSOR_VIEW_H

#include <escher.h>

namespace Shared {

class CursorView : public View {
public:
  virtual void setCursorFrame(KDRect frame) { View::setFrame(frame); }
};

}

#endif
