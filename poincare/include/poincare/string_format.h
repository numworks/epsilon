#ifndef POINCARE_STRING_FORMAT_H
#define POINCARE_STRING_FORMAT_H

#include <kandinsky/font.h>
#include <escher/metric.h>

namespace Poincare {

/* TODO Hugo : Remove this font and update all const KDFont * k_defaultFont into
 * the optimized enum StringFont. */

class StringFormat {
public:
  constexpr static const KDFont * k_defaultFont = KDFont::LargeFont;

  StringFormat(const KDFont * font) {
    setFont(font);
  }

  const KDFont * font() const { return m_font == StringFont::LargeFont ? KDFont::LargeFont : KDFont::SmallFont; }
  void setFont(const KDFont * font) { font == KDFont::LargeFont ? m_font = StringFont::LargeFont : m_font = StringFont::SmallFont; }

private:
  /* We use an enum to optimize space.
   * KDFont are pointers which takes up more space than uint8_t. */
  enum class StringFont : uint8_t {
    SmallFont,
    LargeFont
  };
  StringFont m_font;
};

}

#endif
