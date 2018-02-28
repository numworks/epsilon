#include "script_node_cell.h"
#include <kandinsky/point.h>
#include <escher.h>

namespace Code {

constexpr char ScriptNodeCell::k_parentheses[];

ScriptNodeCell::ScriptNodeView::ScriptNodeView() :
  HighlightCell(),
  m_scriptNode(nullptr),
  m_scriptStore(nullptr)
{
}

void ScriptNodeCell::ScriptNodeView::setScriptNode(ScriptNode * scriptNode) {
  m_scriptNode = scriptNode;
}

void ScriptNodeCell::ScriptNodeView::setScriptStore(ScriptStore * scriptStore) {
  m_scriptStore = scriptStore;
}

void ScriptNodeCell::ScriptNodeView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->drawString(m_scriptNode->name(), KDPoint(0, Metric::TableCellLabelTopMargin), k_fontSize, KDColorBlack, isHighlighted()? Palette::Select : KDColorWhite);
  KDSize nameSize = KDText::stringSize(m_scriptNode->name(), k_fontSize);
  if (m_scriptNode->type() == ScriptNode::Type::Function) {
    ctx->drawString(ScriptNodeCell::k_parentheses, KDPoint(nameSize.width(), Metric::TableCellLabelTopMargin), k_fontSize, KDColorBlack, isHighlighted()? Palette::Select : KDColorWhite);
  }
  ctx->drawString(m_scriptStore->scriptAtIndex(m_scriptNode->scriptIndex()).name(), KDPoint(0, Metric::TableCellLabelTopMargin + nameSize.height() + k_verticalMargin), k_fontSize, Palette::GreyDark, isHighlighted()? Palette::Select : KDColorWhite);
}

KDSize ScriptNodeCell::ScriptNodeView::minimalSizeForOptimalDisplay() const {
  if (m_scriptNode->name() == nullptr) {
    return KDSizeZero;
  }
  KDSize size1 = KDText::stringSize(m_scriptNode->name(), k_fontSize);
  KDSize size2 = KDText::stringSize(m_scriptStore->scriptAtIndex(m_scriptNode->scriptIndex()).name(), k_fontSize);
  KDSize size3 = KDSizeZero;
  if (m_scriptNode->type() == ScriptNode::Type::Function) {
    size3 = KDText::stringSize(ScriptNodeCell::k_parentheses, k_fontSize);
  }
  return KDSize(size1.width() + size3.width() > size2.width() ? size1.width() + size3.width() : size2.width(), Metric::TableCellLabelTopMargin + size1.width() + k_verticalMargin + size2.width());
}

ScriptNodeCell::ScriptNodeCell() :
  TableCell(),
  m_scriptNodeView()
{
}

void ScriptNodeCell::setScriptNode(ScriptNode * scriptNode) {
  m_scriptNodeView.setScriptNode(scriptNode);
  reloadCell();
}

void ScriptNodeCell::setScriptStore(ScriptStore * scriptStore) {
  m_scriptNodeView.setScriptStore(scriptStore);
}

void ScriptNodeCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  m_scriptNodeView.setHighlighted(highlight);
}

void ScriptNodeCell::reloadCell() {
  layoutSubviews();
  HighlightCell::reloadCell();
}

}
