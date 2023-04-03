#ifndef ESCHER_CELL_WIDGET_H
#define ESCHER_CELL_WIDGET_H

#include <escher/responder.h>
#include <escher/view.h>
#include <ion/events.h>
#include <poincare/layout.h>

namespace Escher {

// Widgets need a default constructor.
class CellWidget {
 public:
  enum class Type { Label, SubLabel, Accessory };
  virtual const View* view() const = 0;
  virtual Responder* responder() { return nullptr; }
  virtual const char* text() const { return nullptr; }
  virtual Poincare::Layout layout() const { return Poincare::Layout(); }
  virtual void defaultInitialization(Type type) {}
  virtual void setHighlighted(bool highlighted) {}
  /* This could be replaced by a type like Expressions or Layouts if needed.
   * For now, only editable text fields have a very specific behaviour. */
  virtual bool isAnEditableTextField() const { return false; }
  /* This returns true if this widget should be aligned with the label when it
   * is an accessory. */
  virtual bool alwaysAlignWithLabelAsAccessory() const { return false; }
  /* This returns true if this widget prevents the sublabel from being
   * right-aligned. */
  virtual bool preventRightAlignedSubLabel(Type type) const { return false; }
  virtual bool canBeActivatedByEvent(Ion::Events::Event event) const {
    return event == Ion::Events::EXE || event == Ion::Events::OK;
  }
};

class EmptyCellWidget : public CellWidget {
 public:
  const View* view() const override { return nullptr; }
};

}  // namespace Escher
#endif
