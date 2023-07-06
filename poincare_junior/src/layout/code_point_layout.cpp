#include "code_point_layout.h"

#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {

CodePoint CodePointLayout::GetCodePoint(const Tree* node) {
  return CodePoint(
      *reinterpret_cast<const uint32_t*>(node->block()->nextNth(1)));
}

void CodePointLayout::GetName(const Tree* node, char* buffer,
                              size_t bufferSize) {
  CodePoint c = GetCodePoint(node);
  size_t size = UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  buffer[size] = 0;
}

KDSize CodePointLayout::Size(const Tree* node, const Tree* root,
                             KDFont::Size font) {
  KDSize glyph = KDFont::GlyphSize(font);
  KDCoordinate width = glyph.width();
  // Handle the case of the middle dot which is thinner than the other glyphs
  if (GetCodePoint(node) == UCodePointMiddleDot) {
    width = k_middleDotWidth;
  }
  return KDSize(width, glyph.height());
}

KDCoordinate CodePointLayout::Baseline(const Tree* node, const Tree* root,
                                       KDFont::Size font) {
  return KDFont::GlyphHeight(font) / 2;
}

void CodePointLayout::RenderNode(const Tree* node, const Tree* root,
                                 KDContext* ctx, KDPoint p, KDFont::Size font,
                                 KDColor expressionColor,
                                 KDColor backgroundColor) {
  CodePoint codePoint = GetCodePoint(node);
  // Handle the case of the middle dot which has to be drawn by hand since it is
  // thinner than the other glyphs.
  if (codePoint == UCodePointMiddleDot) {
    int width = k_middleDotWidth;
    int height = KDFont::GlyphHeight(font);
    ctx->fillRect(
        KDRect(p.translatedBy(KDPoint(width / 2, height / 2 - 1)), 1, 1),
        expressionColor);
    return;
  }
  // General case
  constexpr int bufferSize =
      sizeof(CodePoint) / sizeof(char) + 1;  // Null-terminating char
  char buffer[bufferSize];
  GetName(node, buffer, bufferSize);
  ctx->drawString(buffer, p,
                  KDGlyph::Style{.glyphColor = expressionColor,
                                 .backgroundColor = backgroundColor,
                                 .font = font});
}

}  // namespace PoincareJ
