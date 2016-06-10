#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/view.h>

class TableViewDataSource {
  virtual int numberOfCells() = 0;
  View * cellAtIndex(int index) = 0;
  void tableWillDisplayCellAtIndex(int index) = 0;
};

class TableView : public View {
public:
  TableView();
  void drawRect(KDRect rect) override;

  void scrollToRow(int index);
private:
  const char * m_text;
};

#endif
