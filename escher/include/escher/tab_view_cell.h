#ifndef ESCHER_TAB_VIEW_CELL_H
#define ESCHER_TAB_VIEW_CELL_H

#include <escher/view.h>
#include <escher/text_view.h>

class TabViewCell : public View {
public:
  TabViewCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setName(const char * name);
  void setActive(bool active);
  void setSelected(bool selected);
  KDSize minimalSizeForOptimalDisplay() override;
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
private:
  constexpr static KDColor k_defaultTabColor = KDColor::RGB24(0x426DA7);
  constexpr static KDColor k_selectedTabColor = KDColor::RGB24(0xC0D3EB);
  constexpr static KDColor k_separatorTabColor = KDColor::RGB24(0x567AA7);
  bool m_active;
  bool m_selected;
  const char * m_name;
};

#endif
