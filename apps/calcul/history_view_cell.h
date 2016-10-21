#ifndef CALCUL_HISTORY_VIEW_CELL_H
#define CALCUL_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calcul.h"

namespace Calcul {

class HistoryViewCell : public View {
public:
  HistoryViewCell();
  BufferTextView * result();
  void setCalcul(Calcul * calcul);
  void setHighlighted(bool highlight);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  Calcul * m_calcul;
  bool m_highlighted;
  BufferTextView m_result;
};

}

#endif
