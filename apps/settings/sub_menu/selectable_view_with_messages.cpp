#include "selectable_view_with_messages.h"
#include <apps/i18n.h>
#include <assert.h>
#include <algorithm>

using namespace Shared;
using namespace Escher;

namespace Settings {

SelectableViewWithMessages::SelectableViewWithMessages(SelectableTableView * selectableTableView) :
  m_selectableTableView(selectableTableView),
  m_numberOfMessages(0)
{
  for (int i = 0; i < k_maxNumberOfLines; i++) {
    m_messageLines[i].setFont(KDFont::Size::Small);
    m_messageLines[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_messageLines[i].setBackgroundColor(Palette::WallScreen);
  }
}

void SelectableViewWithMessages::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::WallScreen);
}

void SelectableViewWithMessages::setMessages(I18n::Message * m, int numberOfMessages) {
  assert(numberOfMessages <= k_maxNumberOfLines);
  m_numberOfMessages = numberOfMessages;
  for (int i = 0; i < m_numberOfMessages; i++) {
    m_messageLines[i].setMessage(m[i]);
  }
  layoutSubviews();
}

void SelectableViewWithMessages::reload() {
  m_selectableTableView->reloadData(false);
  layoutSubviews();
}

View * SelectableViewWithMessages::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_selectableTableView;
  }
  return &m_messageLines[index-1];
}

void SelectableViewWithMessages::layoutSubviews(bool force) {
  // Prioritize text position
  KDCoordinate textHeight = KDFont::GlyphHeight(KDFont::Size::Small);
  KDCoordinate defOrigin = std::max<KDCoordinate>(
    bounds().height() - Metric::CommonBottomMargin - m_numberOfMessages*textHeight,
    Metric::CommonBottomMargin + k_minSelectableTableViewHeight + Metric::CommonBottomMargin);

  // Layout the table view
  m_selectableTableView->setFrame(KDRect(0, 0, bounds().width(), defOrigin), force);

  // Layout the text
  for (int i = 0; i < m_numberOfMessages; i++) {
    m_messageLines[i].setFrame(KDRect(0, defOrigin, bounds().width(), textHeight), force);
    defOrigin += textHeight;
  }
  for (int i = m_numberOfMessages; i < k_maxNumberOfLines; i++) {
    m_messageLines[i].setFrame(KDRectZero, force);
  }
}

}
