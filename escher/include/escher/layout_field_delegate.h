#ifndef ESCHER_LAYOUT_FIELD_DELEGATE_H
#define ESCHER_LAYOUT_FIELD_DELEGATE_H

#include <escher/context_provider.h>
#include <ion/events.h>
#include <poincare/layout.h>

class LayoutField;

class LayoutFieldDelegate : public ContextProvider{
public:
  virtual void layoutFieldDidReceiveNoneXNTEvent() {};
  virtual bool layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) = 0;
  virtual bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) = 0;
  virtual bool layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) { return false; }
  virtual bool layoutFieldDidAbortEditing(LayoutField * layoutField) { return false; }
  virtual void layoutFieldDidChangeSize(LayoutField * layoutField) {}
};

#endif
