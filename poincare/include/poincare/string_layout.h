#ifndef POINCARE_STRING_LAYOUT_NODE_H
#define POINCARE_STRING_LAYOUT_NODE_H

#include <poincare/layout.h>

namespace Poincare {

class StringLayoutNode : public LayoutNode {
public:
  static constexpr const KDFont * k_defaultFont = KDFont::LargeFont;

  StringLayoutNode(const char * string, int stringSize);

  Type type() const override { return Type::StringLayout; }

  int stringLength() const { return m_stringLength; }
  const char * string() const { return m_string; }
  const KDFont * font() const { return KDFont::LargeFont; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override { assert(false); }
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override { assert(false); }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "StringLayout";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " String length=\"" << m_stringLength <<"\" String=\"" << m_string << "\"";
  }
#endif

private:
  bool protectedIsIdenticalTo(Layout l) override;
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;

  int m_stringLength;
  char m_string[0];
};

class StringLayout : public Layout {
public:
  static void DistributeThousandDisplayType(Layout l, int start, int stop);

  StringLayout(const StringLayoutNode * n) : Layout(n) {}
  static StringLayout Builder(const char * string , int stringSize);
  int stringLength() const { return const_cast<StringLayout *>(this)->node()->stringLength(); }
  const char * string() const { return const_cast<StringLayout *>(this)->node()->string(); }
  const KDFont * font() const { return const_cast<StringLayout *>(this)->node()->font(); }

private:
  using Layout::node;
  StringLayoutNode * node() { return static_cast<StringLayoutNode *>(Layout::node()); }
};



}
#endif

