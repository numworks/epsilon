#include <assert.h>
#include <poincare/print.h>
#include <poincare/print_float.h>
#include <poincare/print_int.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include <cmath>

namespace Poincare {

int Print::PrivateCustomPrintf(char *buffer, size_t bufferSize,
                               const char *format, va_list args) {
  /* We still return the required sizes even if we could not write in the
   * buffer in order to indicate that we overflew the buffer. */
  char *const origin = buffer;
  assert(bufferSize > 0);
  while (*format != 0) {
    if (*format == '%') {
      assert(*(format + 1) != 0);
      int formatLength = 2;
      bool addPlusIfPositive = false;
      if (*(format + 1) == '+') {
        assert(*(format + 2) == 'i' || *(format + 2) == '*');
        format++;
        addPlusIfPositive = true;
      }
      if (*(format + 1) == 's' ||
          (*(format + 1) != 0 && *(format + 2) == 's')) {
        char *insertedText = buffer;
        // Insert text now
        buffer += strlcpy(buffer, va_arg(args, char *),
                          bufferSize - (buffer - origin));
        if (*(format + 1) == 'C') {
          Capitalize(insertedText);
          formatLength = 3;
        } else if (*(format + 1) == 'c') {
          Decapitalize(insertedText);
          formatLength = 3;
        }
      } else if (*(format + 1) == 'c') {
        // Insert char
        *buffer = static_cast<char>(va_arg(args, int));
        buffer++;
      } else if (*(format + 1) == 'i') {
        int value = va_arg(args, int);
        if (addPlusIfPositive && value >= 0) {
          *buffer = '+';
          buffer++;
          if (buffer - origin >= static_cast<int>(bufferSize)) {
            *origin = '\0';
            return buffer - origin;
          }
        }
        buffer += Poincare::PrintInt::Left(value, buffer,
                                           bufferSize - (buffer - origin) - 1);
      } else if (*(format + 1) == '%') {
        *buffer = '%';
        buffer++;
      } else {
        // Format of form %*.*e[d|f]
        assert(*(format + 1) == '*' && *(format + 2) == '.' &&
               *(format + 3) == '*' && *(format + 4) == 'e' &&
               *(format + 5) != 0);
        double value = va_arg(args, double);
        if (addPlusIfPositive && value >= 0.0) {
          *buffer = '+';
          buffer++;
          if (buffer - origin >= static_cast<int>(bufferSize)) {
            *origin = '\0';
            return buffer - origin;
          }
        }
        Preferences::PrintFloatMode displayMode =
            static_cast<Preferences::PrintFloatMode>(va_arg(args, int));
        int numberOfSignificantDigits = va_arg(args, int);
        int glyphLength = PrintFloat::glyphLengthForFloatWithPrecision(
            numberOfSignificantDigits);
        if (*(format + 5) == 'f') {
          buffer += PrintFloat::ConvertFloatToText(
                        static_cast<float>(value), buffer,
                        bufferSize - (buffer - origin), glyphLength,
                        numberOfSignificantDigits, displayMode)
                        .CharLength;
        } else {
          assert(*(format + 5) == 'd');
          buffer += PrintFloat::ConvertFloatToText(
                        value, buffer, bufferSize - (buffer - origin),
                        glyphLength, numberOfSignificantDigits, displayMode)
                        .CharLength;
        }
        formatLength = 6;
      }
      format += formatLength;
    } else {
      *(buffer++) = *(format++);
    }
    if (buffer - origin >= static_cast<int>(bufferSize)) {
      *origin = '\0';
      return buffer - origin;
    }
  }
  *buffer = '\0';
  return buffer - origin;
}

int Print::CustomPrintf(char *buffer, size_t bufferSize, const char *format,
                        ...) {
  va_list args;
  va_start(args, format);
  int length = PrivateCustomPrintf(buffer, bufferSize, format, args);
  va_end(args);
  assert(length < static_cast<int>(bufferSize));
  return length;
}

int Print::SafeCustomPrintf(char *buffer, size_t bufferSize, const char *format,
                            ...) {
  va_list args;
  va_start(args, format);
  int length = PrivateCustomPrintf(buffer, bufferSize, format, args);
  va_end(args);
  return length;
}

void Print::Capitalize(char *text) {
  constexpr static int jumpToUpperCase = 'A' - 'a';
  if (text[0] >= 'a' && text[0] <= 'z') {
    text[0] += jumpToUpperCase;
  }
}

void Print::Decapitalize(char *text) {
  constexpr static int jumpToLowerCase = 'a' - 'A';
  if (text[0] >= 'A' && text[0] <= 'Z') {
    text[0] += jumpToLowerCase;
  }
}

}  // namespace Poincare
