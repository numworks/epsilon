#include "text_helpers.h"

#include <stdarg.h>
#include <string.h>

#include "app.h"

namespace Probability {

int sprintf(char * buffer, const char * format, ...) {
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
  }
  assert(txt != nullptr);
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
      return "t-test";
    case Data::TestType::PooledTTest:
      return "Pooled t-test";
    case Data::TestType::ZTest:
      return "z-test";
  }
  assert(false);
  return nullptr;
}

}  // namespace Probability