#ifndef ESCHER_TAB_VIEW_CELL_H
#define ESCHER_TAB_VIEW_CELL_H

#include <escher/view.h>
#include <escher/text_view.h>

class TabViewCell : public ChildlessView {
public:
  TabViewCell();
  void drawRect(KDRect rect) const override;
  void setName(const char * name);
  void setActive(bool active);
private:
  bool m_active;
  const char * m_name;
};

#endif
