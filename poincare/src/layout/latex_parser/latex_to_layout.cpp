#include "latex_to_layout.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include <array>

#include "../code_point_layout.h"
#include "../k_tree.h"
#include "latex_tokens.h"

namespace Poincare::Internal {

namespace LatexParser {

Tree* LatexToLayout::Parse(const char* latexString) {
  Tree* result = KRackL()->clone();
  ParseOnRackUntilIdentifier(Rack::From(result), &latexString, "");
  return result;
}

void LatexToLayout::ParseOnRackUntilIdentifier(Rack* parent, const char** start,
                                               const char* endIdentifier) {
  size_t endLen = strlen(endIdentifier);
  while (**start != 0 &&
         (endLen == 0 || strncmp(*start, endIdentifier, endLen) != 0)) {
    Tree* child = NextToken(start);
    if (child) {
      NAry::AddChild(parent, child);
    }
  }

  if (**start == 0 && endLen > 0) {
    // endIdentifier couldn't be found
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
}

Tree* LatexToLayout::NextToken(const char** start) {
  for (const Tokens::LatexToken& token : Tokens::k_tokens) {
    const char* leftDelimiter = token.description[0];
    size_t leftDelimiterLength = strlen(leftDelimiter);
    if (strncmp(*start, leftDelimiter, leftDelimiterLength) != 0) {
      continue;
    }
    // Special token found
    *start += leftDelimiterLength;
    Tree* layoutToken = token.constructor();

    // Parse children
    for (int i = 1; i < token.descriptionLength; i += 2) {
      assert(strlen(token.description[i]) <= 1);
      int childIndexInLayout = token.description[i][0];
      const char* rightDelimiter = token.description[i + 1];
      ParseOnRackUntilIdentifier(
          Rack::From(layoutToken->child(childIndexInLayout)), start,
          rightDelimiter);
      *start += strlen(rightDelimiter);
    }

    return layoutToken;
  }

  if (**start == '\\' || **start == ' ' || **start == '{' || **start == '}') {
    /* Ignore \ and {} if it doesn't belong to a know token
     * Ignore spaces */
    *start += 1;
    return nullptr;
  }

  // Code points
  UTF8Decoder decoder(*start);
  Tree* codepoint = CodePointLayout::Push(decoder.nextCodePoint());
  *start = decoder.stringPosition();
  return codepoint;
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
