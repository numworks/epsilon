#ifndef ESCHER_HIGHLIGHT_CELL_H
#define ESCHER_HIGHLIGHT_CELL_H

#include <escher/view.h>

class HighlightCell : public View {
public:
  HighlightCell();
  virtual void setHighlighted(bool highlight);
  bool isHighlighted() const { return m_highlighted; }
  virtual void reloadCell();
protected:
  bool m_highlighted;
};

#endif
