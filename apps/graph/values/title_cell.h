#ifndef GRAPH_TITLE_CELL_H
#define GRAPH_TITLE_CELL_H

#include <escher.h>
#include "../even_odd_cell.h"

class TitleCell : public EvenOddCell {
public:
  TitleCell();
  void setText(const char * title);
  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  char * m_text;
};

#endif
