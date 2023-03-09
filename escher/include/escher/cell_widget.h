#ifndef ESCHER_CELL_WIDGET_H
#define ESCHER_CELL_WIDGET_H

#include <escher/view.h>
#include <ion/events.h>

namespace Escher {

class CellWidget {
 public:
  virtual const View* view() const = 0;
  virtual void setWidgetBackgroundColor(KDColor color) {}
  virtual bool enterOnEvent(Ion::Events::Event event) const { return false; }
};

class EmptyCellWidget : public CellWidget {
 public:
  const View* view() const override { return nullptr; }
};

}  // namespace Escher
#endif
