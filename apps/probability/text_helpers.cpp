#include "text_helpers.h"

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "poincare/preferences.h"
#include "shared/poincare_helpers.h"

namespace Probability {

int snprintf(char * buffer, size_t bufferSize, const char * format, ...) {
  char * origin = buffer;
  va_list args;
  va_start(args, format);
  while (*format != 0) {
    if (*format == '%') {
      if (*(format + 1) == 's') {
        // Insert text now
        buffer += strlcpy(buffer, va_arg(args, char *), 10000);
      } else if (*(format + 1) == 'c') {
        // Insert char
        *buffer = static_cast<char>(va_arg(args, int));
        buffer++;
      }
      format += 2;
    } else {
      *(buffer++) = *(format++);
    }
    if (buffer - origin >= bufferSize - 1) {
      assert(false);
      break;
    }
  }
  *buffer = '\0';
  return buffer - origin;
}

const char * testToText(Data::Test t) {
  // TODO replace with messages
  const char * txt;
  switch (t) {
    case Data::Test::OneProp:
      txt = "one proportion";
      break;
    case Data::Test::OneMean:
      txt = "one mean";
      break;
    case Data::Test::TwoProps:
      txt = "two proportions";
      break;
    case Data::Test::TwoMeans:
      txt = "two means";
      break;
    default:
      assert(false);
  }
  return txt;
}

const char * testToTextSymbol(Data::Test t) {
  switch (t) {
    case Data::Test::OneProp:
      return "p";
    case Data::Test::OneMean:
      return "u";  // TODO mu
    case Data::Test::TwoProps:
      return "p1-p2";  // TODO subscript
    case Data::Test::TwoMeans:
      return "u1-u2";
    default:
      assert(false);
      return nullptr;
  }
}

const char * testTypeToText(Data::TestType t) {
  switch (t) {
    case Data::TestType::TTest:
      return "t";
    case Data::TestType::PooledTTest:
      return "Pooled t";
    case Data::TestType::ZTest:
      return "z";
    default:
      assert(false);
      return nullptr;
  }
}

template <typename T>
int defaultParseFloat(T value, char buffer[], int bufferSize) {
  return Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      value,
      buffer,
      bufferSize,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      Poincare::Preferences::PrintFloatMode::Decimal);
}

template int defaultParseFloat(float value, char buffer[], int bufferSize);
template int defaultParseFloat(double value, char buffer[], int bufferSize);

}  // namespace Probability