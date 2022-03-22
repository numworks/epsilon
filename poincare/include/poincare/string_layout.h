#ifndef POINCARE_STRING_LAYOUT_NODE_H
#define POINCARE_STRING_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/string_format.h>

namespace Poincare {

class StringLayoutNode : public LayoutNode, public StringFormat {
public:
  StringLayoutNode(const char * string, int stringSize, const KDFont * font = StringFormat::k_defaultFont);

  Type type() const override { return Type::StringLayout; }

  int stringLength() const { return m_stringLength; }
  const char * string() const { return m_string; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override { assert(false); }
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override  { assert(false); }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Layout makeEditable() override;
  int numberOfThousandsSeparators();
  void setDecimalOrInteger(bool b) { m_decimalOrInteger = b; }

  // TreeNode
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "StringLayout";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " stringLength=\"" << m_stringLength <<"\" string=\"" << m_string << "\"";
  }
#endif

private:
  // We only display thousands separator if there is more than 4 digits (12 345 but 1234)
  constexpr static int k_minDigitsForThousandSeparator = 5;

  bool protectedIsIdenticalTo(Layout l) override;
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }
  int firstNonDigitIndex();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;

  int m_stringLength;
  // We use this to know if we need to compute thousands separator margins.
  bool m_decimalOrInteger;
  char m_string[0];
};

class StringLayout : public Layout {
public:
  static void DistributeThousandDisplayType(Layout l, int start, int stop);

  StringLayout(const StringLayoutNode * n) : Layout(n) {}
  static StringLayout Builder(const char * string , int stringSize, const KDFont * font = StringLayoutNode::k_defaultFont);
  int stringLength() const { return const_cast<StringLayout *>(this)->node()->stringLength(); }
  const char * string() const { return const_cast<StringLayout *>(this)->node()->string(); }
  const KDFont * font() const { return const_cast<StringLayout *>(this)->node()->font(); }
  Layout makeEditable();

private:
  using Layout::node;
  StringLayoutNode * node() { return static_cast<StringLayoutNode *>(Layout::node()); }
};



}
#endif

