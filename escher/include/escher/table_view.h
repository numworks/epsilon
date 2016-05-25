#ifndef ESCHER_TABLE_VIEW_H
#define ESCHER_TABLE_VIEW_H

#include <escher/view.h>

class TableView : public View {
public:
  TableView(
      -> DataSource,
      -> CellClass
      );
  TextView(KDPoint origin, const char * text);
  void drawRect(KDRect rect) override;
private:
  const char * m_text;
};

#endif
