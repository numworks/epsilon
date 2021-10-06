#include <poincare/print.h>
#include <poincare/print_float.h>
#include <poincare/print_int.h>
#include <assert.h>
#include <cmath>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

namespace Poincare {
namespace Print {

int customPrintf(char * buffer, size_t bufferSize, const char * format, ...) {
  char * origin = buffer;
  va_list args;
  va_start(args, format);
  while (*format != 0) {
    if (*format == '%') {
      assert(*(format + 1) != 0);
      int formatLength = 2;
      if (*(format + 1) == 's' || (*(format + 1) == '*' && *(format + 2) == 's')) {
        char * insertedText = buffer;
        // Insert text now
        buffer += strlcpy(buffer, va_arg(args, char *), bufferSize - (buffer - origin));
        if (*(format + 1) == '*') {
          StringFormat format = static_cast<StringFormat>(va_arg(args, int));
          if (format == StringFormat::Capitalized) {
            capitalize(insertedText);
          } else {
            assert(format == StringFormat::Decapitalized);
            decapitalize(insertedText);
          }
          formatLength = 3;
        }
      } else if (*(format + 1) == 'c') {
        // Insert char
        *buffer = static_cast<char>(va_arg(args, int));
        buffer++;
      } else if (*(format + 1) == 'i') {
        buffer += Poincare::PrintInt::Left(va_arg(args, int), buffer, bufferSize - (buffer - origin) - 1);
      } else {
        // Format of form %*.*e[d|f]
        assert(*(format + 1) == '*'
            && *(format + 2) == '.'
            && *(format + 3) == '*'
            && *(format + 4) == 'e'
            && *(format + 5) != 0);
        double value = va_arg(args, double);
        Preferences::PrintFloatMode displayMode = static_cast<Preferences::PrintFloatMode>(va_arg(args, int));
        int numberOfSignificantDigits = va_arg(args, int);
        int glyphLength = PrintFloat::glyphLengthForFloatWithPrecision(numberOfSignificantDigits);
        if (*(format + 5) == 'f') {
          buffer += PrintFloat::ConvertFloatToText(static_cast<float>(value), buffer, bufferSize - (buffer - origin), glyphLength, numberOfSignificantDigits, displayMode).CharLength;
        } else {
          assert(*(format + 5) == 'd');
          buffer += PrintFloat::ConvertFloatToText(value, buffer, bufferSize - (buffer - origin), glyphLength, numberOfSignificantDigits, displayMode).CharLength;
        }
        formatLength = 6;
      }
      format += formatLength;
    } else {
      *(buffer++) = *(format++);
    }
    if (buffer - origin >= (int)bufferSize - 1) {
      assert(false);
      break;
    }
  }
  va_end(args);
  *buffer = '\0';
  return buffer - origin;
}

void capitalize(char * text) {
  constexpr static int jumpToUpperCase = 'A' - 'a';
  if (text[0] >= 'a' && text[0] <= 'z') {
    text[0] += jumpToUpperCase;
  }
}

void decapitalize(char * text) {
  constexpr static int jumpToLowerCase = 'a' - 'A';
  if (text[0] >= 'A' && text[0] <= 'Z') {
    text[0] += jumpToLowerCase;
  }
}

}
}
