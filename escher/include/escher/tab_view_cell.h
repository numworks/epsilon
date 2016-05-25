#ifndef ESCHER_TAB_VIEW_CELL_H
#define ESCHER_TAB_VIEW_CELL_H

#include <escher/view.h>
#include <escher/text_view.h>

class TabViewCell : public View {
public:
  TabViewCell();
  int numberOfSubviews() override;
  View * subview(int index) override;

  void setName(const char * name);
private:
  TextView m_textView;
};

#endif
