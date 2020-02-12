#ifndef ESCHER_STACK_VIEW_H
#define ESCHER_STACK_VIEW_H

#include <escher/bordered.h>
#include <escher/view.h>
#include <escher/view_controller.h>

class StackView : public View, public Bordered {
public:
  StackView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setNamedController(ViewController * controller);
  void setTextColor(KDColor textColor);
  void setBackgroundColor(KDColor backgroundColor);
  void setSeparatorColor(KDColor separatorColor);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
private:
  KDColor m_textColor;
  KDColor m_backgroundColor;
  KDColor m_separatorColor;
  ViewController * m_controller;
};

#endif
