#ifndef POINCARE_STRING_LAYOUT_NODE_H
#define POINCARE_STRING_LAYOUT_NODE_H

#include <poincare/layout.h>

/* Following expressions: Numbers, symbols (like cos), units and some other
 * expressions like "undefined", will be layouted into StringLayout.
 * if they have more than 1 codepoint. This is to optimize the pool memory.
 * When a StringLayout is inserted into an input field (LayoutField in Escher),
 * it is made editable, which means that it is turned into CodePointsLayout
 * for the user to be able to edit it and move cursor.
 * /!\ CURSOR CANNOT BE MOVED INSIDE A STRINGLAYOUT /!\ */

namespace Poincare {

class StringLayoutNode : public LayoutNode {
public:
  StringLayoutNode(const char * string, int stringSize);

  Type type() const override { return Type::StringLayout; }

  int stringLength() const { return strlen(m_string); }
  const char * string() const { return m_string; }

  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Layout makeEditable() override;
  int numberOfThousandsSeparators();

  // TreeNode
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "StringLayout";
  }
  void logAttributes(std::ostream & stream) const override {
    stream <<" string=\"" << m_string << "\"";
  }
#endif

private:
  // We only display thousands separator if there is more than 4 digits (12 345 but 1234)
  constexpr static int k_minDigitsForThousandSeparator = 5;

  bool protectedIsIdenticalTo(Layout l) override;
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override {
    assert(false);
    return KDPointZero;
  }
  int firstNonDigitIndex();
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;

  char m_string[0];
};

class StringLayout : public Layout {
public:
  static void DistributeThousandDisplayType(Layout l, int start, int stop);

  StringLayout(const StringLayoutNode * n) : Layout(n) {}
  static StringLayout Builder(const char * string, int stringSize = -1);
  int stringLength() const { return const_cast<StringLayout *>(this)->node()->stringLength(); }
  const char * string() const { return const_cast<StringLayout *>(this)->node()->string(); }
  Layout makeEditable();

private:
  using Layout::node;
  StringLayoutNode * node() { return static_cast<StringLayoutNode *>(Layout::node()); }
};



}
#endif
