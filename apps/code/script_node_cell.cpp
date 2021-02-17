#include "script_node_cell.h"
#include <algorithm>

using namespace Escher;

namespace Code {

constexpr char ScriptNodeCell::k_parentheses[];
constexpr char ScriptNodeCell::k_parenthesesWithEmpty[];

void ScriptNodeCell::setScriptNode(ScriptNode * node) {
  // Use a temporary buffer to crop label name.
  const int maxNumberOfCharsInBuffer = BufferTextView::maxNumberOfCharsInBuffer();
  const int labelLength = std::min(maxNumberOfCharsInBuffer - 1, node->nameLength());

  char temp_buffer[maxNumberOfCharsInBuffer];
  assert(strlen(node->name()) >= labelLength);
  memcpy(temp_buffer, node->name(), labelLength);
  temp_buffer[labelLength] = 0;

  m_labelView.setText(temp_buffer);

  if (node->type() == ScriptNode::Type::WithParentheses) {
    m_labelView.appendText(ScriptNodeCell::k_parentheses);
  }

  m_subLabelView.setText(node->description() != nullptr ? node->description() : "");
  m_accessoryView.setText(node->nodeSourceName() != nullptr ? node->nodeSourceName() : "");
  reloadCell();
}

void ScriptNodeCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  m_subLabelView.setBackgroundColor(backgroundColor);
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void ScriptNodeCell::reloadCell() {
  layoutSubviews();
  HighlightCell::reloadCell();
}

}
