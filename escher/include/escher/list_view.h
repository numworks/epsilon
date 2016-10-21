 #ifndef ESCHER_LIST_VIEW_H
#define ESCHER_LIST_VIEW_H

#include <escher/table_view.h>
#include <escher/list_view_data_source.h>

class ListView : public TableView {
public:
  ListView(ListViewDataSource * dataSource, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void scrollToRow(int index);
  View * cellAtIndex(int index);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
};

#endif
