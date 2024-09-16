#include <escher/metric.h>
#include <omg/utf8_helper.h>
#include <poincare/layout.h>

#include <algorithm>

namespace Poincare {

StringLayoutNode::StringLayoutNode(const char *string, int stringSize)
    : LayoutNode() {
  strlcpy(m_string, string, stringSize);
}

OLayout StringLayoutNode::makeEditable() {
  return StringLayout(this).makeEditable();
}

size_t StringLayoutNode::serialize(char *buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return strlcpy(buffer, m_string, bufferSize);
}

size_t StringLayoutNode::size() const {
  return sizeof(StringLayoutNode) + sizeof(char) * (stringLength() + 1);
}

bool StringLayoutNode::protectedIsIdenticalTo(OLayout l) {
  assert(l.otype() == Type::StringLayout);
  StringLayout &sl = static_cast<StringLayout &>(l);
  return strncmp(m_string, sl.string(),
                 std::max(stringLength() + 1, sl.stringLength() + 1)) == 0;
}

int StringLayoutNode::numberOfThousandsSeparators() {
  int nonDigitIndex = firstNonDigitIndex();
  bool isNegative = m_string[0] == '-';
  if (nonDigitIndex - isNegative < k_minDigitsForThousandSeparator) {
    return 0;
  }
  return (nonDigitIndex - isNegative - 1) / 3;
}

int StringLayoutNode::firstNonDigitIndex() {
  int nonDigitIndex = m_string[0] == '-';
  while (nonDigitIndex < stringLength()) {
    if (!('0' <= m_string[nonDigitIndex] && '9' >= m_string[nonDigitIndex])) {
      break;
    }
    nonDigitIndex++;
  }
  return nonDigitIndex;
}

StringLayout StringLayout::Builder(const char *string, int stringSize) {
  if (stringSize < 1) {
    stringSize = strlen(string) + 1;
  }
  void *bufferNode = Pool::sharedPool->alloc(sizeof(StringLayoutNode) +
                                             sizeof(char) * stringSize);
  StringLayoutNode *node =
      new (bufferNode) StringLayoutNode(string, stringSize);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<StringLayout &>(h);
}

OLayout StringLayout::makeEditable() {
  OLayout editableLayout;
  assert(false);
  OLayout myParent = this->parent();
  /* editableLayout can be an HorizontalLayout, so it needs to be merged with
   * parent if it is also an HorizontalLayout. */
  if (!myParent.isUninitialized() && myParent.isHorizontal()) {
    int index = myParent.indexOfChild(*this);
    static_cast<HorizontalLayout &>(myParent).removeChildInPlace(
        *this, numberOfChildren());
    static_cast<HorizontalLayout &>(myParent).addOrMergeChildAtIndex(
        editableLayout, index);
    return myParent.childAtIndex(index);
  }
  replaceWithInPlace(editableLayout);
  return editableLayout;
}

}  // namespace Poincare
