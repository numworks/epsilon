#include "history_view_cell.h"
#include <apps/global_preferences.h>
#include <poincare/layout_helper.h>

constexpr static int sNumberOfFakeResults = 9;

static const char * sFakeResultsFR[sNumberOfFakeResults] = {
  "Bonne question !",
  "12, peut-être ?",
  "Aucune idée...",
  "Beaucoup !",
  "Je ne sais pas.",
  "Facile !"
  "J'ai oublié.",
  "42, je crois.",
  "Trop dur."
};

static const char * sFakeResultsEN[sNumberOfFakeResults]= {
  "Good question!",
  "12, maybe?",
  "No idea...",
  "A lot!",
  "I don't know.",
  "Surprise!",
  "I forgot.",
  "42, I think.",
  "Easy!"
};

void Calculation::HistoryViewCell::customizeForIndex(int i) {
  constexpr int foolishFrequency = 3;

  if (i % foolishFrequency != 0) {
    return;
  }

  // Prevent re-using the cell as-is
  m_calculationCRC32 = 0;

  m_calculationAdditionInformation = Calculation::AdditionalInformationType::None;

  const char ** fakeResults = sFakeResultsEN;
  if (GlobalPreferences::sharedGlobalPreferences()->language() == I18n::Language::FR) {
    fakeResults = sFakeResultsFR;
  }

  int fakeResultIndex = (i/foolishFrequency) % sNumberOfFakeResults;
  const char * fakeResult = fakeResults[fakeResultIndex];

  m_calculationDisplayOutput = Calculation::DisplayOutput::ApproximateOnly;
  m_scrollableOutputView.setDisplayCenter(false);
  m_scrollableOutputView.setLayouts(
    Poincare::Layout(),
    Poincare::Layout(),
    Poincare::LayoutHelper::String(fakeResult, strlen(fakeResult))
  );

  // Make sure the view is refreshed
  layoutSubviews();
  reloadScroll();
}
