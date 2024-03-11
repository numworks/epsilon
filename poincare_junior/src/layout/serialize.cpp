#include "serialize.h"

#include <algorithm>

#include "code_point_layout.h"

namespace PoincareJ {

/* TODO Serialization needs to go through expressions when possible to be able
 * to use operator priority to minimize parentheses and get the correct
 * multiplication symbol. */

char *Serialize(const Rack *rack, char *buffer, char *end) {
  for (const Tree *child : rack->children()) {
    buffer = Serialize(Layout::From(child), buffer, end);
    if (buffer == end) {
      return end;
    }
  }
  return buffer;
}

char *append(const char *text, char *buffer, char *end) {
  size_t len = std::min<size_t>(strlen(text), end - buffer);
  memcpy(buffer, text, len);
  return buffer + len;
}

char *Serialize(const Layout *layout, char *buffer, char *end) {
  switch (layout->layoutType()) {
    case LayoutType::CodePoint: {
      constexpr int bufferSize = sizeof(CodePoint) / sizeof(char) + 1;
      char codepointBuffer[bufferSize];
      CodePointLayout::GetName(layout, codepointBuffer, bufferSize);
      buffer = append(codepointBuffer, buffer, end);
      break;
    }
    case LayoutType::Parenthesis: {
      buffer = append("(", buffer, end);
      buffer = Serialize(layout->child(0), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::Fraction: {
      buffer = append("(", buffer, end);
      buffer = Serialize(layout->child(0), buffer, end);
      buffer = append(")/(", buffer, end);
      buffer = Serialize(layout->child(1), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::VerticalOffset: {
      buffer = append("^(", buffer, end);
      buffer = Serialize(layout->child(0), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    default:
      buffer = append("?", buffer, end);
  }
  return buffer;
}

}  // namespace PoincareJ
