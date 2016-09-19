#include <escher/table_view_cell.h>

constexpr KDColor TableViewCell::k_separatorColor;
constexpr KDColor TableViewCell::k_tableBackgroundColor;
constexpr KDColor TableViewCell::k_focusedCellBackgroundColor;
constexpr KDColor TableViewCell::k_cellBackgroundColor;
constexpr KDCoordinate TableViewCell::k_margin;
constexpr KDCoordinate TableViewCell::k_marginLabel;

TableViewCell::TableViewCell() :
  ChildlessView(),
  m_focused(false),
  m_message(nullptr)
{
}

bool TableViewCell::isFocused() const {
  return m_focused;
}

void TableViewCell::setFocused(bool focused) {
  m_focused = focused;
  markRectAsDirty(bounds());
}

void TableViewCell::setMessage(const char * message) {
  m_message = message;
  markRectAsDirty(bounds());
}

void TableViewCell::drawRect(KDContext * ctx, KDRect rect) const {

  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = (m_focused ? k_focusedCellBackgroundColor : k_cellBackgroundColor);
  KDColor textColor = (m_focused ? KDColorWhite : KDColorBlack);

  ctx->fillRect(KDRect(k_margin+1, 1, width-2*k_margin-1, height-1), backgroundColor);
  ctx->fillRect(KDRect(0,0,k_margin,height), k_tableBackgroundColor);
  ctx->fillRect(KDRect(k_margin,0,width-2*k_margin,1), k_separatorColor);
  ctx->fillRect(KDRect(k_margin,0,1,height), k_separatorColor);
  ctx->fillRect(KDRect(width-k_margin,0,1,height), k_separatorColor);
  ctx->fillRect(KDRect(width-k_margin+1,0,k_margin, height), k_tableBackgroundColor);

  ctx->drawString(m_message, KDPoint(k_margin+k_marginLabel, k_marginLabel), textColor, backgroundColor);
}
