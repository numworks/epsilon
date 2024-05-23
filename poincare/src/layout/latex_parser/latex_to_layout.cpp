#include "latex_to_layout.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "../code_point_layout.h"
#include "../k_tree.h"
#include "../rack_layout.h"

namespace Poincare::Internal {

namespace LatexParser {

Tree* LatexToLayout::Parse(const char* latexString) {
  Tree* result = KRackL()->clone();
  ParseOnRackUntil(Rack::From(result), latexString,
                   latexString + strlen(latexString));
  return result;
}

void LatexToLayout::ParseOnRackUntil(Rack* parent, const char* latexString,
                                     const char* stringEnd) {
  while (latexString < stringEnd) {
    if (*latexString == '\\') {
      ParseSpecialIdentifier(parent, &latexString, stringEnd);
      continue;
    }
    Tree* codepoint = CodePointLayout::Push(*latexString);
    NAry::AddChild(parent, codepoint);
    latexString += 1;
  }
}

void LatexToLayout::ParseSpecialIdentifier(Rack* parent,
                                           const char** latexString,
                                           const char* stringEnd) {
  assert(**latexString == '\\');
  for (int i = 0; i < k_nPairs; i++) {
    const char* leftIdentifier = k_identifiersPairs[i].left;
    size_t identifierLen = strlen(leftIdentifier);
    if (strncmp(*latexString, leftIdentifier, identifierLen) != 0) {
      continue;
    }

    *latexString += identifierLen;
    const char* rightIdentifierPosition =
        FindIdentifier(k_identifiersPairs[i].right, *latexString, stringEnd);
    if (rightIdentifierPosition == stringEnd) {
      // Couldn't find matching right identifier.
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      return;
    }
    Type t = k_identifiersPairs[i].type;
    Tree* child;
    if (t == Type::ParenthesisLayout) {
      child = KParenthesisL(KRackL())->clone();
    } else {
      child = KAbsL(KRackL())->clone();
    }
    ParseOnRackUntil(Rack::From(child->firstChild()), *latexString,
                     rightIdentifierPosition);
    *latexString =
        rightIdentifierPosition + strlen(k_identifiersPairs[i].right);
    NAry::AddChild(parent, child);
    return;
  }
  // No identifier detected. Skip '\'
  *latexString += 1;
}

const char* LatexToLayout::FindIdentifier(const char* identifier,
                                          const char* string,
                                          const char* stringEnd) {
  size_t identifierLen = strlen(identifier);
  while (string + identifierLen < stringEnd) {
    if (strncmp(string, identifier, identifierLen) == 0) {
      return string;
    }
    string += 1;
  }
  return stringEnd;
}

}  // namespace LatexParser

}  // namespace Poincare::Internal
