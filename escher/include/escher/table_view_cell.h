#ifndef ESCHER_TABLE_VIEW_CELL_H
#define ESCHER_TABLE_VIEW_CELL_H

#include <escher/view.h>

class TableViewCell : public View {
public:
  TableViewCell();
  virtual void setHighlighted(bool highlight);
  bool isHighlighted() const;
  virtual void reloadCell();
private:
  bool m_highlighted;
};

#endif
