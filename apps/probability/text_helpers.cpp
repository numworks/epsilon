#include "text_helpers.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <stdarg.h>
#include <string.h>

#include "poincare/preferences.h"
#include "shared/poincare_helpers.h"

using namespace Poincare;

namespace Probability {

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

I18n::Message titleFormatForTest(Data::Test test, Data::TestType type) {
  switch (test) {
    case Data::Test::OneProp:
      return I18n::Message::HypothesisControllerTitleOneProp;
    case Data::Test::TwoProps:
      return I18n::Message::HypothesisControllerTitleTwoProps;
    case Data::Test::OneMean:
      switch (type) {
        case Data::TestType::ZTest:
          return I18n::Message::HypothesisControllerTitleOneMeanZ;
        case Data::TestType::TTest:
          return I18n::Message::HypothesisControllerTitleOneMeanT;
        default:
          assert(false);
          return I18n::Message::Default;
      }
    case Data::Test::TwoMeans:
      switch (type) {
        case Data::TestType::ZTest:
          return I18n::Message::HypothesisControllerTitleTwoMeansZ;
        case Data::TestType::TTest:
          return I18n::Message::HypothesisControllerTitleTwoMeansT;
        case Data::TestType::PooledTTest:
          return I18n::Message::HypothesisControllerTitleTwoMeansPooledT;
        default:
          assert(false);
          return I18n::Message::Default;
      }

    default:
      assert(false);
      return I18n::Message::Default;
  }
}

// TODO: virtualize!!
I18n::Message graphTitleFormatForTest(Data::Test test, Data::TestType type, Data::CategoricalType categoricalType) {
  switch (test) {
    case Data::Test::Categorical:
      switch (categoricalType) {
        case Data::CategoricalType::Goodness:
          return I18n::Message::StatisticGraphControllerTestTitleFormatGoodnessTest;
        case Data::CategoricalType::Homogeneity:
          return I18n::Message::StatisticGraphControllerTestTitleFormatHomogeneityTest;
        default:
          assert(false);
          return I18n::Message::Default;
      }
    default:
      switch (type) {
        case Data::TestType::ZTest:
          return I18n::Message::StatisticGraphControllerTestTitleFormatZtest;
        case Data::TestType::PooledTTest:
        case Data::TestType::TTest:
          return I18n::Message::StatisticGraphControllerTestTitleFormatTTest;
        default:
          assert(false);
          return I18n::Message::Default;
      }
  }
}

int defaultConvertFloatToText(double value, char buffer[], int bufferSize) {
  return Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      value,
      buffer,
      bufferSize,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      Poincare::Preferences::PrintFloatMode::Decimal);
}

Poincare::Layout XOneMinusXTwoLayout() {
  HorizontalLayout x1 = HorizontalLayout::Builder(
      CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout x2 = HorizontalLayout::Builder(
      CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
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

Poincare::Layout setSmallFont(Poincare::Layout layout) {
  if (layout.type() == Poincare::LayoutNode::Type::CodePointLayout) {
    static_cast<Poincare::CodePointLayout *>(&layout)->setFont(KDFont::SmallFont);
  }
  for (int i = 0; i < layout.numberOfChildren(); i++) {
    setSmallFont(layout.childAtIndex(i));
  }
  return layout;
}

}  // namespace Probability
