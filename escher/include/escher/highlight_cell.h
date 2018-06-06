#ifndef ESCHER_HIGHLIGHT_CELL_H
#define ESCHER_HIGHLIGHT_CELL_H

#include <escher/view.h>
#include <escher/responder.h>
#include <poincare.h>

class HighlightCell : public View {
public:
  HighlightCell();
  virtual void setHighlighted(bool highlight);
  bool isHighlighted() const { return m_highlighted; }
  virtual void reloadCell();
  virtual Responder * responder() {
    return nullptr;
  }
  virtual const char * text() const {
    return nullptr;
  }
  virtual Poincare::ExpressionLayout * expressionLayout() const {
    return nullptr;
  }
protected:
  bool m_highlighted;
};

#endif
