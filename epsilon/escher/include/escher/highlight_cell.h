#ifndef ESCHER_HIGHLIGHT_CELL_H
#define ESCHER_HIGHLIGHT_CELL_H

#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/view.h>
#include <poincare/layout.h>

namespace Escher {

// TODO: make an AbstractHighlightCell without any member variable

class HighlightCell : public View {
 public:
  HighlightCell();

  bool isVisible() const { return m_state != State::Hidden; }
  virtual void setVisible(bool visible);

  virtual void setHighlighted(bool highlight);
  void setHighlightedWithoutReload(bool highlight) {
    m_state = highlight ? State::Highlighted : State::Visible;
  }
  virtual void reloadCell() { markWholeFrameAsDirty(); }
  virtual Responder* responder() { return nullptr; }
  virtual const char* text() const { return nullptr; }
  virtual Poincare::Layout layout() const { return Poincare::Layout(); }
  virtual void initSize(KDCoordinate width) {}

  enum class State : uint8_t {
    Hidden,
    Visible,
    Highlighted,
  };
  bool isHighlighted() const { return m_state == State::Highlighted; }

 protected:
  KDColor defaultBackgroundColor() const {
    return isHighlighted() ? Palette::Select : KDColorWhite;
  }

 private:
  /* Not all cells keep m_state up to date, as they may not rely on it for
   * drawing. As such, controllers should not read this value to get the
   * definitive state of the cell. */
  State m_state;
};

}  // namespace Escher

#endif
