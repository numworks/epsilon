#include <poincare/xnt_helpers.h>
#include <poincare_expressions.h>

#include <array>

namespace Poincare {

namespace XNTHelpers {

bool FindXNTSymbol(UnicodeDecoder& decoder, bool* defaultXNTHasChanged,
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
  constexpr static const int k_indexOfMainExpression = 0;
  constexpr static const int k_indexOfParameter = 1;
  // Step 1 : Identify the function the cursor is in
  size_t textStart = decoder.start();
  size_t location = decoder.position();
  CodePoint c = UCodePointUnknown;
  // Analyze glyphs on the left of the cursor
  if (location > textStart) {
    c = decoder.previousCodePoint();
    location = decoder.position();
  }
  int functionLevel = 0;
  int numberOfCommas = 0;
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
        while (location > textStart && decoder.previousCodePoint() == ' ') {
          location = decoder.position();
        }
        // Move back right before the last non whitespace code-point
        decoder.nextCodePoint();
        location = decoder.position();
        // Identify one of the functions
        for (size_t i = 0; i < std::size(sFunctions); i++) {
          const char* name = sFunctions[i].aliasesList.mainAlias();
          size_t length = strlen(name);
          if (location >= textStart + length) {
            size_t savePosition = decoder.position();
            // Move the decoder where the function name could start
            decoder.unsafeSetPosition(savePosition - length);
            constexpr size_t bufferSize = 10;
            char buffer[bufferSize];
            decoder.printInBuffer(buffer, bufferSize, length);
            if (strcmp(buffer, name) == 0) {
              functionFound = true;
              // Update default code point
              *defaultXNTCodePoint = CodePoint(sFunctions[i].xnt);
              *defaultXNTHasChanged = true;
            }
            decoder.unsafeSetPosition(savePosition);
          }
        }
        if (!functionFound) {
          // No function found, reset search parameters
          numberOfCommas = 0;
        }
        break;
      case ',':
        if (functionLevel == 0) {
          numberOfCommas++;
          if (numberOfCommas > k_indexOfParameter) {
            /* We are only interested in the 2 first children.
             * Look for one in level. */
            functionLevel++;
            numberOfCommas = 0;
          }
        }
        break;
      case ')':
        // Skip to the next matching '('.
        functionLevel++;
        break;
    }
    c = decoder.previousCodePoint();
    location = decoder.position();
  }
  if (functionFound) {
    // Put decoder at the beginning of the argument
    c = decoder.nextCodePoint();
    do {
      c = decoder.nextCodePoint();
    } while (c == ' ');
    assert(c == '(');
  }
  return functionFound && numberOfCommas == k_indexOfMainExpression;
}

}  // namespace XNTHelpers

}  // namespace Poincare
