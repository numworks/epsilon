#include "text_helpers.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <stdarg.h>
#include <string.h>

#include "poincare/preferences.h"
#include "shared/poincare_helpers.h"

using namespace Poincare;

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
    if (buffer - origin >= (int)bufferSize - 1) {
      assert(false);
      break;
    }
  }
  *buffer = '\0';
  return buffer - origin;
}

const char * testToText(Data::Test t) {
  I18n::Message msg;
  switch (t) {
    case Data::Test::OneProp:
      msg = I18n::Message::OneProportion;  // TODO these are very similar to
                                           // I18n::Message::TestOneProp
      break;
    case Data::Test::OneMean:
      msg = I18n::Message::OneMean;
      break;
    case Data::Test::TwoProps:
      msg = I18n::Message::TwoProportions;
      break;
    case Data::Test::TwoMeans:
      msg = I18n::Message::TwoMeans;
      break;
    default:
      assert(false);
  }
  return I18n::translate(msg);
}

const char * testToTextSymbol(Data::Test t) {
  switch (t) {
    case Data::Test::OneProp:
      return "p";
    case Data::Test::OneMean:
      return "μ";
    case Data::Test::TwoProps:
      return "p1-p2";  // TODO subscript
    case Data::Test::TwoMeans:
      return "μ1-μ2";
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
      return I18n::translate(I18n::Message::PooledT);
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

Poincare::Layout XOneMinusXTwoLayout() {
  HorizontalLayout x1 = HorizontalLayout::Builder(
      ConjugateLayout::Builder(CodePointLayout::Builder('x')),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout x2 = HorizontalLayout::Builder(
      ConjugateLayout::Builder(CodePointLayout::Builder('x')),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout res = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  res.addOrMergeChildAtIndex(x2, 1, true);
  res.addOrMergeChildAtIndex(x1, 0, true);
  return std::move(res);
}

Poincare::HorizontalLayout codePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript) {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(base),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder(subscript, KDFont::LargeFont),
                                    VerticalOffsetLayoutNode::Position::Subscript));
}

}  // namespace Probability