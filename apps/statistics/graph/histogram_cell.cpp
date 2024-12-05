#include "histogram_cell.h"

namespace Statistics {

void HistogramCell::setHighlighted(bool highlight) {
  Escher::HighlightCell::setHighlighted(highlight);
  m_view.setFocus(highlight);
  m_view.setDisplayLabels(highlight);
}

void HistogramCell::layoutSubviews(bool force) {
  setChildFrame(&m_view, KDRect(0, 0, bounds().width(), bounds().height()),
                force);
  m_view.reload();
}

}  // namespace Statistics
