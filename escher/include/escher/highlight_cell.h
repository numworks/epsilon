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

  bool isVisible() const override { return m_state != State::Hidden; }
  virtual void setVisible(bool visible);
  void show() { setVisible(true); }
  void hide() { setVisible(false); }

  bool isSelectable() { return isVisible() && protectedIsSelectable(); }

  virtual void setHighlighted(bool highlight);
  void setHighlightedWithoutReload(bool highlight) { m_state = highlight ? State::Highlighted : State::Visible; }
  virtual void reloadCell() { markRectAsDirty(bounds()); }
  virtual Responder * responder() { return nullptr; }
  virtual const char * text() const { return nullptr; }
  virtual Poincare::Layout layout() const { return Poincare::Layout(); }

protected:
  virtual bool protectedIsSelectable() { return true; }
  bool isHighlighted() const { return m_state == State::Highlighted; }
  KDColor defaultBackgroundColor() const { return isHighlighted() ? Palette::Select : KDColorWhite; }

  /* Not all cells keep m_state up to date, as they may not rely on it for
   * drawing. As such, controllers should not read this value to get the
   * definitive state of the cell. */
  enum class State : uint8_t {
    Hidden,
    Visible,
    Highlighted,
  };
  State m_state;
};

}

#endif
