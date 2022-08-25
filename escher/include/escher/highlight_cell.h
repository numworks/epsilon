#ifndef ESCHER_HIGHLIGHT_CELL_H
#define ESCHER_HIGHLIGHT_CELL_H

#include <escher/view.h>
#include <escher/responder.h>
#include <poincare/layout.h>

namespace Escher {

// TODO: make an AbstractHighlightCell without any member variable

class HighlightCell : public View {
public:
  HighlightCell();

  virtual bool isSelectable() { return true; }
  virtual void setHighlighted(bool highlight);
  virtual void reloadCell();
  virtual Responder * responder() { return nullptr; }
  virtual const char * text() const { return nullptr; }
  virtual Poincare::Layout layout() const { return Poincare::Layout(); }

protected:
  bool isHighlighted() const { return m_highlighted; }
  KDColor defaultBackgroundColor() const { return m_highlighted ? Palette::Select : KDColorWhite; }

  /* Not all cells keep m_highlighted up to date, as they may not rely on it
   * for drawing. As such, controllers should not read this value to get the
   * definitive state of the cell. */
  bool m_highlighted;
};

}

#endif
