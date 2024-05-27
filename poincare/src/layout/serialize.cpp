#include "serialize.h"

#include <poincare/src/expression/builtin.h>

#include <algorithm>

#include "code_point_layout.h"
#include "grid.h"
#include "vertical_offset.h"

namespace Poincare::Internal {

/* TODO Serialization needs to go through expressions when possible to be able
 * to use operator priority to minimize parentheses and get the correct
 * multiplication symbol. */

char* SerializeRack(const Rack* rack, char* buffer, char* end) {
  for (const Tree* child : rack->children()) {
    buffer = SerializeLayout(Layout::From(child), buffer, end);
    if (buffer == end) {
      return end;
    }
  }
  return buffer;
}

char* append(const char* text, char* buffer, char* end) {
  size_t len = std::min<size_t>(strlen(text), end - 1 - buffer);
  memcpy(buffer, text, len);
  return buffer + len;
}

char* SerializeLayout(const Layout* layout, char* buffer, char* end,
                      RackSerializer serializer) {
  switch (layout->layoutType()) {
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint: {
      constexpr int bufferSize = sizeof(CodePoint) / sizeof(char) + 1;
      char codepointBuffer[bufferSize];
      CodePointLayout::CopyName(layout, codepointBuffer, bufferSize);
      buffer = append(codepointBuffer, buffer, end);
      break;
    }
    case LayoutType::Parenthesis: {
      buffer = append("(", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::CurlyBrace: {
      buffer = append("{", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append("}", buffer, end);
      break;
    }
    case LayoutType::Fraction: {
      buffer = append("((", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append(")/(", buffer, end);
      buffer = serializer(layout->child(1), buffer, end);
      buffer = append("))", buffer, end);
      break;
    }
    case LayoutType::VerticalOffset: {
      if (VerticalOffset::IsSuffixSuperscript(layout)) {
        buffer = append("^(", buffer, end);
        buffer = serializer(layout->child(0), buffer, end);
        buffer = append(")", buffer, end);
      } else {
        buffer = append("(", buffer, end);
        buffer = serializer(layout->child(0), buffer, end);
        buffer = append(")", buffer, end);
      }
      break;
    }
    case LayoutType::OperatorSeparator:
    case LayoutType::ThousandSeparator:
      break;
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(layout);
      buffer = append("[", buffer, end);
      for (int j = 0; j < grid->numberOfRows() - 1; j++) {
        buffer = append("[", buffer, end);
        for (int i = 0; i < grid->numberOfColumns() - 1; i++) {
          if (i > 0) {
            buffer = append(",", buffer, end);
          }
          buffer = serializer(grid->childAt(i, j), buffer, end);
        }
        buffer = append("]", buffer, end);
      }
      buffer = append("]", buffer, end);
      break;
    }
    case LayoutType::Diff:
    case LayoutType::NthDiff: {
      buffer = append("diff(", buffer, end);
      buffer = serializer(layout->child(2), buffer, end);
      buffer = append(",", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append(",", buffer, end);
      buffer = serializer(layout->child(1), buffer, end);
      if (layout->isNthDiffLayout()) {
        buffer = append(",", buffer, end);
        buffer = serializer(layout->child(3), buffer, end);
      }
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::Point2D: {
      buffer = append("(", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append(",", buffer, end);
      buffer = serializer(layout->child(1), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    default: {
      if (layout->isPiecewiseLayout()) {
        buffer = append("piecewise", buffer, end);
      } else {
        const BuiltinWithLayout* builtin =
            BuiltinWithLayout::GetReservedFunction(layout->layoutType());
        if (!builtin) {
          assert(false);
          buffer = append("?", buffer, end);
        }
        buffer = append(builtin->aliases()->mainAlias(), buffer, end);
      }
      buffer = append("(", buffer, end);
      bool firstChild = true;
      for (int i = 0; const Tree* child : layout->children()) {
        if (layout->isParametricLayout() &&
            i == layout->numberOfChildren() - 1) {
          break;
        }
        if (layout->isPiecewiseLayout() &&
            i == layout->numberOfChildren() - 2) {
          break;
        }
        if (layout->isPiecewiseLayout() &&
            i == layout->numberOfChildren() - 3 &&
            child->numberOfChildren() == 0) {
          break;
        }
        if (!firstChild) {
          buffer = append(",", buffer, end);
        }
        if (layout->isParametricLayout() && i == 0) {
          buffer = serializer(Rack::From(layout->lastChild()), buffer, end);
          buffer = append(",", buffer, end);
        }
        buffer = serializer(Rack::From(child), buffer, end);
        firstChild = false;
        i++;
      }
      buffer = append(")", buffer, end);
    }
  }
  return buffer;
}

}  // namespace Poincare::Internal
