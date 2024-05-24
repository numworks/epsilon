#include "layout_to_latex.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "latex_tokens.h"

namespace Poincare::Internal {

namespace LatexParser {

char* LayoutToLatex::Parse(const Rack* rack, char* buffer, char* end) {
  for (const Tree* child : rack->children()) {
    if (child->isSeparatorLayout()) {
      // Invisible layouts in latex
      continue;
    }

    if (buffer >= end) {
      // Buffer is too short
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      return end;
    }

    if (child->isCodePointLayout()) {
      buffer = CodePointLayout::CopyName(child, buffer, end - buffer);
      continue;
    }

    bool tokenFound = false;
    for (const Tokens::LatexToken& token : Tokens::k_tokens) {
      if (!token.detector(child)) {
        continue;
      }

      int i = 0;
      while (true) {
        const char* delimiter = token.description[i];
        size_t delimiterLength = strlen(delimiter);
        if (buffer + delimiterLength >= end) {
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
          return end;
        }
        memcpy(buffer, delimiter, delimiterLength);
        buffer += delimiterLength;
        if (i == token.descriptionLength - 1) {
          *buffer = 0;
          break;
        }
        assert(strlen(token.description[i + 1]) <= 1);
        int indexOfChildInLayout = token.description[i + 1][0];
        buffer =
            Parse(Rack::From(child->child(indexOfChildInLayout)), buffer, end);
        i += 2;
      }
      tokenFound = true;
    }

    if (!tokenFound) {
      // Layout can't be represented in Latex (for now)
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
  }

  return buffer;
}

}  // namespace LatexParser
}  // namespace Poincare::Internal
