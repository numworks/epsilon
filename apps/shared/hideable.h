#ifndef APPS_SHARED_HIDEABLE_H
#define APPS_SHARED_HIDEABLE_H

#include <escher/palette.h>

namespace Shared {

class Hideable {
public:
  Hideable() :
    m_hide(false)
  {}
  static KDColor hideColor() { return Palette::WallScreenDark; }
  bool hidden() const { return m_hide; }
  virtual void setHide(bool hide) { m_hide = hide; }
  virtual void reinit() {}
private:
  bool m_hide;
};

}

#endif
