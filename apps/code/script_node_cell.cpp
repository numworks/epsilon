#include "script_node_cell.h"
#include <kandinsky/point.h>
#include <escher.h>

namespace Code {

constexpr char ScriptNodeCell::k_parentheses[];
constexpr char ScriptNodeCell::k_parenthesesWithEmpty[];

// TODO LEA remove static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

void ScriptNodeCell::ScriptNodeView::drawRect(KDContext * ctx, KDRect rect) const {
  const KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;

  /* If it exists, draw the description name. If it did not fit, we would have
   * put  nullptr at the node creation. */
  const char * descriptionName = m_scriptNode->description();
  bool drawDescription = descriptionName != nullptr;
  KDCoordinate rectHeightPerLine = rect.height();
  if (drawDescription) {
    rectHeightPerLine = rect.height() / 2;
    ctx->drawString(descriptionName, KDPoint(0, rectHeightPerLine + k_verticalMargin / 2), k_font, Palette::GreyDark, backgroundColor);
  }

  // Draw the node name
  const char * nodeName = m_scriptNode->name();
  const int nodeNameLength = m_scriptNode->nameLength();
  KDSize nameSize = k_font->stringSize(nodeName, nodeNameLength);
  const KDCoordinate nodeNameY = rectHeightPerLine - k_verticalMargin / 2 - nameSize.height() ;
  ctx->drawString(nodeName, KDPoint(0, nodeNameY), k_font, KDColorBlack, backgroundColor, nodeNameLength);
  // If it is a function, draw the parentheses
  if (m_scriptNode->type() == ScriptNode::Type::Function) {
    ctx->drawString(ScriptNodeCell::k_parentheses, KDPoint(nameSize.width(), nodeNameY), k_font, KDColorBlack, backgroundColor);
  }

  /* If it exists, draw the source name. If it did not fit, we would have put
   * nullptr at the node creation. */
  const char * sourceName = m_scriptNode->nodeSourceName();
  if (sourceName != nullptr) {
    KDSize sourceNameSize = k_font->stringSize(sourceName);
    ctx->drawString(sourceName, KDPoint(rect.width() - sourceNameSize.width(), nodeNameY), k_font, Palette::GreyDark, backgroundColor);
  }
}

KDSize ScriptNodeCell::ScriptNodeView::minimalSizeForOptimalDisplay() const {
  if (m_scriptNode->name() == nullptr) {
    return KDSizeZero;
  }
  return KDSize(300 /*TODO LEA*/, 2*k_topBottomMargin + m_scriptNode->description() == nullptr ? k_font->glyphSize().height() : k_font->glyphSize().height() * 2 + k_verticalMargin);
#if 0
  KDSize nameLineSize = k_font->stringSize(m_scriptNode->name(), m_scriptNode->nameLength());
  if (m_scriptNode->type() == ScriptNode::Type::Function) {
    nameLineSize = KDSize(nameLineSize.width() + k_font->stringSize(k_parentheses).width(), nameLineSize.height());
  }
  const char * sourceName = m_scriptNode->nodeSourceName();
  if (sourceName != nullptr) {
    KDSize sourceNameSize = k_font->stringSize(sourceName, nodeNameLength);
    nameLineSize = KDSize(nameLineSize.width() + sourceNameSize.width() + k_font->glyphSize().width(), nameLineSize.height());
  }

  const char * descriptionName = m_scriptNode->description();
  if (descriptionName == nullptr) {
    return nameLineSize;
  }
  const KDSize descriptionNameSize = k_font->stringSize(descriptionName);
  return KDSize(maxCoordinate(nameLineSize.width(), descriptionNameSize.width()),2*Metric::TableCellVerticalMargin + nameLineSize.width() + k_verticalMargin + descriptionNameSize.height());
#endif
}

void ScriptNodeCell::setScriptNode(ScriptNode * scriptNode) {
  m_scriptNodeView.setScriptNode(scriptNode);
  reloadCell();
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
