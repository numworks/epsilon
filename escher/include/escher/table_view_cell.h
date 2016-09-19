#ifndef ESCHER_TABLE_VIEW_CELL_H
#define ESCHER_TABLE_VIEW_CELL_H

#include <escher.h>

class TableViewCell : public ChildlessView {
public:
  TableViewCell();
  void setMessage(const char * message);
  bool isFocused() const;
  void setFocused(bool focused);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDColor k_separatorColor = KDColor(0xB4B7B9);
  constexpr static KDColor k_tableBackgroundColor = KDColor(0xF0F3F5);
  constexpr static KDColor k_focusedCellBackgroundColor = KDColor(0xBFD3EB);
  constexpr static KDColor k_cellBackgroundColor = KDColor(0xFCFCFC);
  constexpr static KDCoordinate k_margin = 20;
  constexpr static KDCoordinate k_marginLabel = 5;
  const char * m_message;
  bool m_focused;
};

#endif
