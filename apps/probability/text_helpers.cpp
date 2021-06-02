#include "text_helpers.h"

#include <stdarg.h>
#include <string.h>

#include "app.h"

namespace Probability {

int sprintf(char * buffer, const char * format, ...) {
  char * origin = buffer;
  va_list args;
  va_start(args, format);
  while (format != 0) {
    if (*format == '%' && *(format + 1) == 's') {
      // Insert text now
      buffer += strlcpy(buffer, va_arg(args, char *), 10000);
      buffer += 2;
    }
    *(buffer++) = *(format++);
  }
  return buffer - origin;
}

int testToText(Data::Test t, char * buffer, int bufferLength) {
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
      txt = "";
      break;
  }
  assert(strlen(txt) < bufferLength);
  memcpy(buffer, txt, strlen(txt) + 1);

  return strlen(txt) + 1;
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

int testTypeToText(Data::TestType t, char * buffer, int bufferLength) {
  const char * txt;
  switch (t) {
    case Data::TestType::TTest:
      txt = "t-test";
      break;
    case Data::TestType::PooledTTest:
      txt = "Pooled t-test";
      break;
    case Data::TestType::ZTest:
      txt = "z-test";
      break;
  }
  assert(strlen(txt) + 1 < bufferLength);
  memcpy(buffer, txt, strlen(txt) + 1);
  return strlen(txt) + 1;
}

}