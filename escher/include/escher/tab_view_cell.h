#ifndef ESCHER_TAB_VIEW_CELL_H
#define ESCHER_TAB_VIEW_CELL_H

#include <escher/i18n.h>
#include <escher/text_view.h>
#include <escher/view.h>

namespace Escher {

class TabViewController;

class TabViewCell : public View {
 public:
  TabViewCell();
  void drawRect(KDContext *ctx, KDRect rect) const override;
  void setTabController(TabViewController *controller, uint8_t tabNumber);
  void setActive(bool active);
  void setSelected(bool selected);
  KDSize minimalSizeForOptimalDisplay() const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char *className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
 private:
  uint8_t m_tabNumber;
  bool m_active;
  bool m_selected;
  TabViewController *m_controller;
};

}  // namespace Escher
#endif
