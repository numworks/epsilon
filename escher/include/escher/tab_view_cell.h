#ifndef ESCHER_TAB_VIEW_CELL_H
#define ESCHER_TAB_VIEW_CELL_H

#include <escher/view.h>
#include <escher/text_view.h>

class TabViewCell : public ChildlessView {
public:
  TabViewCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setName(const char * name);
  void setActive(bool active);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
private:
  bool m_active;
  const char * m_name;
};

#endif
