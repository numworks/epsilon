#ifndef CODE_CONSOLE_LINE_CELL_H
#define CODE_CONSOLE_LINE_CELL_H

#include <escher/highlight_cell.h>
#include "console_line.h"

namespace Code {

class ConsoleLineCell : public HighlightCell {
public:
  ConsoleLineCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setLine(ConsoleLine line);
private:
  ConsoleLine m_line;
};

}

#endif
