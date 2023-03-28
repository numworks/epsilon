#include <poincare/xnt_helpers.h>
#include <poincare_expressions.h>

#include <array>

namespace Poincare {

static bool Contains(UnicodeDecoder& string, UnicodeDecoder& pattern) {
  while (CodePoint c = pattern.nextCodePoint()) {
    if (string.nextCodePoint() != c) {
      return false;
    }
  }
  return true;
}

bool FindXNTSymbol(UnicodeDecoder& functionDecoder, bool* defaultXNTHasChanged,
                   CodePoint* defaultXNTCodePoint) {
  /* If cursor is in one of the following functions, and everything before the
   * cursor is correctly nested, the default XNTCodePoint will be improved.
   * These functions all have the following structure :
   * functionName(argument, variable, additonalOutOfContextFields ...)
   * If the cursor is in an argument field, and the variable is well nested and
   * defined, the variable will be inserted into the given buffer. Otherwise,
   * the (improved or not) defaultXNTCodePoint is inserted. */
  constexpr static struct {
    AliasesList aliasesList;
    char xnt;
  } sFunctions[] = {
      {Derivative::s_functionHelper.aliasesList(),
       Derivative::k_defaultXNTChar},
      {Integral::s_functionHelper.aliasesList(), Integral::k_defaultXNTChar},
      {Product::s_functionHelper.aliasesList(), Product::k_defaultXNTChar},
      {Sum::s_functionHelper.aliasesList(), Sum::k_defaultXNTChar}};
  // Step 1 : Identify the function the cursor is in
  size_t textStart = functionDecoder.start();
  size_t location = functionDecoder.position();
  CodePoint c = UCodePointUnknown;
  // Analyze glyphs on the left of the cursor
  if (location > textStart) {
    c = functionDecoder.previousCodePoint();
    location = functionDecoder.position();
  }
  int functionLevel = 0;
  int cursorLevel = 0;
  bool cursorInVariableField = false;
  bool functionFound = false;
  while (location > textStart && !functionFound) {
    switch (c) {
      case '(':
        // Check if we are skipping to the next matching '('.
        if (functionLevel > 0) {
          functionLevel--;
          break;
        }
        // Skip over whitespace.
        while (location > textStart &&
               functionDecoder.previousCodePoint() == ' ') {
          location = functionDecoder.position();
        }
        // Move back right before the last non whitespace code-point
        functionDecoder.nextCodePoint();
        location = functionDecoder.position();
        // Identify one of the functions
        for (size_t i = 0; i < std::size(sFunctions); i++) {
          const char* name = sFunctions[i].aliasesList.mainAlias();
          size_t length = UTF8Helper::StringCodePointLength(name);
          if (location >= textStart + length) {
            UTF8Decoder nameDecoder(name);
            size_t savePosition = functionDecoder.position();
            // Move the decoder where the function name could start
            functionDecoder.unsafeSetPosition(savePosition - length);
            if (Contains(functionDecoder, nameDecoder)) {
              functionFound = true;
              // Update default code point
              *defaultXNTCodePoint = CodePoint(sFunctions[i].xnt);
              *defaultXNTHasChanged = true;
            }
            functionDecoder.unsafeSetPosition(savePosition);
          }
        }
        if (!functionFound) {
          // No function found, reset search parameters
          cursorInVariableField = false;
          cursorLevel += 1;
        }
        break;
      case ',':
        if (functionLevel == 0) {
          if (cursorInVariableField) {
            // Cursor is out of context, skip to the next matching '('
            functionLevel++;
            cursorLevel++;
          }
          // Update cursor's position status
          cursorInVariableField = !cursorInVariableField;
        }
        break;
      case ')':
        // Skip to the next matching '('.
        functionLevel++;
        break;
    }
    c = functionDecoder.previousCodePoint();
    location = functionDecoder.position();
  }

  return functionFound && !cursorInVariableField;
}

}  // namespace Poincare
