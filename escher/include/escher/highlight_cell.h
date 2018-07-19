#ifndef ESCHER_HIGHLIGHT_CELL_H
#define ESCHER_HIGHLIGHT_CELL_H

#include <escher/view.h>
#include <escher/responder.h>
#include <poincare.h>
#include <poincare/layout_reference.h>

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
  virtual Poincare::LayoutRef layoutRef() const {
    return Poincare::LayoutRef(nullptr);
  }
protected:
  bool m_highlighted;
};

#endif
