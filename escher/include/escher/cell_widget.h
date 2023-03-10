#ifndef ESCHER_CELL_WIDGET_H
#define ESCHER_CELL_WIDGET_H

#include <escher/view.h>
#include <ion/events.h>

namespace Escher {

class CellWidget {
 public:
  enum class Type { Label, SubLabel, Accessory };
  virtual const View* view() const = 0;

  virtual void defaultInitialization(Type type) {}

  virtual void setWidgetBackgroundColor(KDColor color) {}
  virtual bool enterOnEvent(Ion::Events::Event event) const { return false; }
};

class EmptyCellWidget : public CellWidget {
 public:
  const View* view() const override { return nullptr; }
};

}  // namespace Escher
#endif
