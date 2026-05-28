#pragma once

#include <escher/responder.h>
#include <escher/theme/components.h>
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
  virtual const Poincare::Layout& layout() const {
    return Poincare::Layout::EmptyLayout();
  }
  virtual void initSize(KDCoordinate width) {}

  enum class State : uint8_t {
    Hidden,
    Visible,
    Highlighted,
  };
  bool isHighlighted() const { return m_state == State::Highlighted; }

  constexpr static Escher::ThemeComponent k_defaultBackground =
      Escher::ThemeComponent::DefaultCellBackground;

 private:
  /* Not all cells keep m_state up to date, as they may not rely on it for
   * drawing. As such, controllers should not read this value to get the
   * definitive state of the cell. */
  State m_state;
};

}  // namespace Escher
