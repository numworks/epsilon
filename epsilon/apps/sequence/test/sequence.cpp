#include <apps/global_preferences.h>
#include <apps/shared/global_store.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/shared/sequence_context.h>
#include <apps/shared/sequence_store.h>
#include <assert.h>
#include <omg/float.h>
#include <poincare/print.h>
#include <poincare/src/expression/projection.h>
#include <poincare/test/approximation/helper.h>
#include <poincare/test/float_helper.h>
#include <poincare/test/helpers/symbol_store.h>
#include <quiz.h>
#include <string.h>

#include <cmath>

using namespace Poincare;

namespace Shared {

// Add a Sequence to the global sequence store
Sequence* AddSequence(Sequence::Type type, const char* definition,
                      const char* condition1, const char* condition2) {
  SequenceStore& store = GlobalContextAccessor::SequenceStore();
  const SequenceContext& context = GlobalContextAccessor::SequenceContext();
  Ion::Storage::Record::ErrorStatus err = store.addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);

  Ion::Storage::Record record = store.recordAtIndex(store.numberOfModels() - 1);
  Sequence* u = store.modelForRecord(record);
  u->setType(type);
  err = u->setContent(Layout::Parse(definition), context);
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  if (condition1) {
    err =
        u->setFirstInitialConditionContent(Layout::Parse(condition1), context);
    assert(err == Ion::Storage::Record::ErrorStatus::None);
  }
  if (condition2) {
    err =
        u->setSecondInitialConditionContent(Layout::Parse(condition2), context);
    assert(err == Ion::Storage::Record::ErrorStatus::None);
  }
  (void)err;  // Silence compilation warning.
  // u should be defined if a definition was given
  assert((definition != nullptr) == (u->isDefined()));
  return u;
}

class SequenceDefinitions {
 public:
  Sequence::Type types[SequenceStore::k_maxNumberOfSequences];
  const char* definitions[SequenceStore::k_maxNumberOfSequences];
  const char* conditions1[SequenceStore::k_maxNumberOfSequences];
  const char* conditions2[SequenceStore::k_maxNumberOfSequences];
  void reset() {
    for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
      types[i] = Sequence::Type::Explicit;
      definitions[i] = nullptr;
      conditions1[i] = nullptr;
      conditions2[i] = nullptr;
    }
  }
};

void check_sequences_defined_by(
    double result[SequenceStore::k_maxNumberOfSequences][10],
    const SequenceDefinitions& sequences) {
  Sequence* seqs[SequenceStore::k_maxNumberOfSequences];
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    seqs[i] = AddSequence(sequences.types[i], sequences.definitions[i],
                          sequences.conditions1[i], sequences.conditions2[i]);
  }

  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
      if (!seqs[i]->isDefined()) {
        continue;
      }
      double un = seqs[i]->evaluateXYAtParameter((double)j).y();
      bool isEqual = OMG::Float::RoughlyEqual<double>(
          un, result[i][j], OMG::Float::EpsilonLax<double>(), true);
      constexpr size_t bufferSize = 100;
      char buffer[bufferSize];
      static_assert(SequenceStore::k_maxNumberOfSequences == 3);
      char sequenceName[SequenceStore::k_maxNumberOfSequences] = {'u', 'v',
                                                                  'w'};
      Poincare::Print::CustomPrintf(
          buffer, bufferSize, "%c(%i)=%s=%*.*ed instead of %*.*ed.",
          sequenceName[i], j, sequences.definitions[i], un,
          Preferences::PrintFloatMode::Decimal, 7, result[i][j],
          Preferences::PrintFloatMode::Decimal, 7);
      quiz_assert_print_if_failure(isEqual, buffer);
    }
  }
  GlobalContextAccessor::SequenceStore().removeAll();
  GlobalContextAccessor::SequenceStore().tidyDownstreamPoolFrom();
  GlobalContextAccessor::SequenceCache().resetCache();
}

void check_sum_of_sequence_between_bounds(double result, double start,
                                          double end, Sequence::Type type,
                                          const char* definition,
                                          const char* condition1,
                                          const char* condition2) {
  const Sequence* seq = AddSequence(type, definition, condition1, condition2);
  double sum =
      seq->sumBetweenBounds(start, end).approximateSystemToRealScalar<double>();
  assert_roughly_equal(sum, result);

  GlobalContextAccessor::SequenceStore().removeAll();
  GlobalContextAccessor::SequenceStore().tidyDownstreamPoolFrom();
  GlobalContextAccessor::SequenceCache().resetCache();
}

void check_sequence_notation(Sequence::Type type, const char* definition,
                             const char* condition1, const char* condition2) {
  assert(type != Sequence::Type::Explicit);
  Sequence* seq = AddSequence(type, definition, condition1, condition2);
  quiz_assert_print_if_failure(seq->isDefined(), "Sequence is undefined.");
  for (int j = 0; j < 10; j++) {
    seq->setRecursiveNotation(SequenceHelper::RecursiveNotation::Default);
    double unDefault = seq->evaluateXYAtParameter((double)j).y();
    seq->setRecursiveNotation(SequenceHelper::RecursiveNotation::Shifted);
    double unShifted = seq->evaluateXYAtParameter((double)j).y();
    bool isEqual = OMG::Float::RoughlyEqual<double>(
        unDefault, unShifted, OMG::Float::EpsilonLax<double>(), true);
    constexpr size_t bufferSize = 100;
    char buffer[bufferSize];
    Poincare::Print::CustomPrintf(
        buffer, bufferSize,
        "Default notation gives %*.*ed and shifted notation gives %*.*ed.",
        unDefault, Preferences::PrintFloatMode::Decimal, 7, unShifted,
        Preferences::PrintFloatMode::Decimal, 7);
    quiz_assert_print_if_failure(isEqual, buffer);
  }
  GlobalContextAccessor::SequenceStore().removeAll();
  GlobalContextAccessor::SequenceStore().tidyDownstreamPoolFrom();
  GlobalContextAccessor::SequenceCache().resetCache();
}

QUIZ_CASE(sequence_evaluation) {
  SequenceDefinitions sequences;
  sequences.reset();

  // u(n) = n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {}, {}};
    sequences.definitions[0] = "n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = floor(1200*1.0125^(n-1+160))
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {8649.0, 8757.0, 8867.0, 8977.0, 9090.0, 9203.0, 9318.0, 9435.0, 9553.0,
         9672.0},
        {},
        {}};
    sequences.definitions[0] = "floor(1200*1.0125^(n-1+160))";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  assert(GlobalPreferences::SharedGlobalPreferences()->complexFormat() ==
         ComplexFormat::Real);
  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Cartesian);
  // u(n) = (-1/2)^n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {1, -0.5, 0.25, -0.125, 0.0625, -0.03125, 0.015625, -0.0078125,
         0.00390625, -0.001953125},
        {},
        {}};
    sequences.definitions[0] = "(-1/2)^n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();
  // u(n) = 250+n v(n) = (-1)^u(n)
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {250.0, 251.0, 252.0, 253.0, 254.0, 255.0, 256.0, 257.0, 258.0, 259.0},
        {1, -1, 1, -1, 1, -1, 1, -1, 1, -1},
        {}};
    sequences.definitions[0] = "250+n";
    sequences.definitions[1] = "(-1)^u(n)";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();
  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Real);

  // u(n+1) = u(n)+n, u(0) = 0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {}, {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "u(n)+n";
    sequences.conditions1[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = u(n)+u(0), u(0) = 2
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0}, {}, {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "u(n)+u(0)";
    sequences.conditions1[0] = "2";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = u(n)+n, u(0) = n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {}, {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "u(n)+n";
    sequences.conditions1[0] = "n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = 8/(1+ln(u(n))), u(0) = 3.55
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {3.55, 3.528974374040812, 3.538246010831602, 3.53414472168467,
         3.535956418133212, 3.535155634355861, 3.535509491610179,
         3.535353107491111, 3.535422216448733, 3.535391675246047},
        {},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "8/(1+ln(u(n)))";
    sequences.conditions1[0] = "3.55";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0}, {}, {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "u(n)+u(n+1)+n";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u is independent, v recursive and v(0) depends on u
   * u(n) = n; v(n+1) = v(n)+1; v(0) = u(5) */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "v(n)+1";
    sequences.conditions1[1] = "u(5)";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u independent, v defined with u
   * u(n) = n; v(n) = u(n)+n */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "u(n)+n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = u(n)+n, u(0) = 0; v(n) = u(n)+n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
        {0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0, 45.0},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n+u(n)";
    sequences.conditions1[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n) = u(n)+n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
        {0.0, 1.0, 2.0, 4.0, 7.0, 12.0, 20.0, 33.0, 54.0, 88.0},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "n+u(n)+u(n+1)";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = n; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0, 25.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "n+u(n)+u(n+1)";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();
#if 0
  // u(n+1) = u(n)+n, u(0) = 0; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
        {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n+u(n)";
    sequences.definitions[1] = "n+u(n)+u(n+1)";
    sequences.conditions1[0] = "0";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();
#endif

  /* u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0;
   * v(n+1) = u(n)+u(n+1)+n, v(0) = 0 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
        {0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0, 133.0},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "n+u(n)+u(n+1)";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = n; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 0.0, 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "n";
    sequences.conditions2[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = u(n)+n, u(0) = 0; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
        {0.0, 0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n+u(n)";
    sequences.conditions1[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0;
   * v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0; */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
        {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "u(n+1)+u(n)+n";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // v independent, u defined with v

  // u(n) = v(n); v(n) = u(n)
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "v(n)";
    sequences.definitions[1] = "u(n)";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = v(n)+n, u(0)=0; v(n) = u(n)+n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0},
        {0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "v(n)+n";
    sequences.definitions[1] = "u(n)+n";
    sequences.conditions1[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+2) = v(n+1)+u(n+1)+v(n)+u(n)+n, u(1) = 0; u(0)=0; v(n) = u(n)+n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 6.0, 21.0, 64.0, 183.0, 510.0, 1405.0, 3852.0},
        {0.0, 1.0, 3.0, 9.0, 25.0, 69.0, 189.0, 517.0, 1413.0, 3861.0},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "v(n+1)+v(n)+u(n+1)+u(n)+n";
    sequences.definitions[1] = "u(n)+n";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = v(n)+n; v(n+1) = u(n)+n, v(0)=0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81},
        {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "v(n)+n";
    sequences.definitions[1] = "u(n)+n";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = v(n)+n, u(0)=0; v(n+1) = u(n)+n, v(0)=0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "v(n)+n";
    sequences.definitions[1] = "u(n)+n";
    sequences.conditions1[0] = "0";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u(n+2) = v(n+1)+u(n+1)+v(n)+u(n)+n, u(1) = 0, u(0) = 0;
   * v(n+1) = u(n)+n, v(0)=0 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 0.0, 2.0, 7.0, 19.0, 46.0, 105.0, 233.0, 509.0},
        {0.0, 0.0, 1.0, 2.0, 5.0, 11.0, 24.0, 52.0, 112.0, 241.0},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "v(n+1)+v(n)+u(n+1)+u(n)+n";
    sequences.definitions[1] = "u(n)+n";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "0";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = n; v undefined
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u undefined; v(n) = n
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {}};
    sequences.definitions[1] = "n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u, v, w independent

  // u(n) = n; v(n+1) = 9+n, v(0) = 0; w(n) = n+2
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0},
        {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0}};
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "9+n";
    sequences.definitions[2] = "n+2";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0;
   * v(n) = 9+n;
   * w(n+1) = w(n)+2, w(0) = 0 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
        {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0},
        {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.types[2] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "u(n+1)+u(n)+n";
    sequences.definitions[1] = "9+n";
    sequences.definitions[2] = "2+w(n)";
    sequences.conditions1[0] = "0";
    sequences.conditions1[2] = "0";
    sequences.conditions2[0] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u(n+1) = u(n)+n, u(0) = 0;
   * v(n) = 9+n;
   * w(n+2) = w(n+1)+w(n)+2, w(0) = 0, w(1) = 0 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
        {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0},
        {0.0, 0.0, 2.0, 4.0, 8.0, 14.0, 24.0, 40.0, 66.0, 108.0}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.types[2] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "u(n)+n";
    sequences.definitions[1] = "9+n";
    sequences.definitions[2] = "w(n+1)+w(n)+2";
    sequences.conditions1[0] = "0";
    sequences.conditions1[2] = "0";
    sequences.conditions2[2] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* v depends on u, w depends on v & u independent
   * u(n) = n ; v(n+1) = u(n)+1, v(0) = 0, w(n) = 2+v(n) */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.types[2] = Sequence::Type::Explicit;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "u(n)+1";
    sequences.definitions[2] = "2+v(n)";
    sequences.conditions1[1] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = n ; v(n+1) = u(n)+1, v(0) = 0, w(n+1) = 2+v(n+1), w(0) = 0
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.types[2] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "u(n)+1";
    sequences.definitions[2] = "2+v(n+1)";
    sequences.conditions1[1] = "0";
    sequences.conditions1[2] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* v depends on u, w depends on u and v & u independent
   * u(n) = n
   * v(n+1) = u(n)+1, v(0) = 0
   * w(n+2) = 2+v(n+1)+u(n+1), w(0) = 0, w(1) = 0 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {0.0, 0.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.types[2] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "u(n)+1";
    sequences.definitions[2] = "2+v(n+1)+u(n+1)";
    sequences.conditions1[2] = "0";
    sequences.conditions1[1] = "0";
    sequences.conditions2[2] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* v depends on u, w depends on v & u depends on w
   * u(n+1) = w(n)+1, u(0) = 0
   * v(n+1) = u(n)+1, v(0) = 0
   * w(n+1) = v(n)+2, w(0) = 0 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 3.0, 4.0, 5.0, 7.0, 8.0, 9.0, 11.0, 12.0},
        {0.0, 1.0, 2.0, 4.0, 5.0, 6.0, 8.0, 9.0, 10.0, 12.0},
        {0.0, 2.0, 3.0, 4.0, 6.0, 7.0, 8.0, 10.0, 11.0, 12.0}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.types[2] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "w(n)+1";
    sequences.definitions[1] = "u(n)+1";
    sequences.definitions[2] = "v(n)+2";
    sequences.conditions1[0] = "0";
    sequences.conditions1[1] = "0";
    sequences.conditions1[2] = "0";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* v depends on u, w depends on v & u depends on w
   * u(n) = w(n); v(n) = u(n), w(n) = v(n) */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.types[2] = Sequence::Type::Explicit;
    sequences.definitions[0] = "w(n)";
    sequences.definitions[1] = "u(n)";
    sequences.definitions[2] = "v(n)";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u independent, v depends on u(3)
   * u(n) = n; v(n) = u(5)+n */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "u(5)+n";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u independent, v depends on u(2)
   * u(n) = n; v(n+1) = v(n)-u(2) */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {2.0, 0.0, -2.0, -4.0, -6.0, -8.0, -10.0, -12.0, -14.0, -16.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "v(n)-u(2)";
    sequences.conditions1[1] = "u(2)";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u and v interdependent
   * u(n+2) = n + v(3) + u(n+1) - u(n); v(n) = u(n) - u(1) */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 3.0, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {-3.0, 0.0, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "n+v(3)+u(n+1)-u(n)";
    sequences.definitions[1] = "u(n)-u(1)";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "3";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u is independent, v depends on u(120) and w(5), w depends on u(8)
   * u(n) = n; v(n+2) = v(n+1) + v(n) + u(120); w(n+1) = w(n) - u(8) */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
        {46.0, 6.0, 172.0, 298.0, 590.0, 1008.0, 1718.0, 2846.0, 4684.0,
         7650.0},
        {6.0, 14.0, 22.0, 30.0, 38.0, 46.0, 54.0, 62.0, 70.0, 78.0}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::DoubleRecurrence;
    sequences.types[2] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "n";
    sequences.definitions[1] = "v(n+1)+v(n)+u(120)";
    sequences.definitions[2] = "w(n)+u(8)";
    sequences.conditions1[1] = "w(5)";
    sequences.conditions2[1] = "6";
    sequences.conditions1[2] = "6";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  /* u independent, v depends on u(n+6)
   * u(n) = 9n; v(n+1) = u(n+6)+v(0); v(0) = 9 */
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0, 81.0},
        {9.0, 63.0, 72.0, 81.0, 90.0, 99.0, 108.0, 117.0, 126.0, 135.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::SingleRecurrence;
    sequences.definitions[0] = "9n";
    sequences.definitions[1] = "u(n+6)+v(0)";
    sequences.conditions1[1] = "9";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // Explicit self-referencing sequences
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.types[2] = Sequence::Type::Explicit;
    sequences.definitions[0] = "abs(u(0))";
    sequences.definitions[1] = "floor(v(0))";
    sequences.definitions[2] = "ceil(w(0))";
    check_sequences_defined_by(results, sequences);
    sequences.definitions[0] = "acos(u(0))";
    sequences.definitions[1] = "asin(v(0))";
    sequences.definitions[2] = "atan(w(0))";
    check_sequences_defined_by(results, sequences);
    sequences.definitions[0] = "cos(u(0))";
    sequences.definitions[1] = "sin(v(0))";
    sequences.definitions[2] = "tan(w(0))";
    check_sequences_defined_by(results, sequences);
    sequences.definitions[0] = "1+u(0)";
    sequences.definitions[1] = "2*v(0)";
    sequences.definitions[2] = "2^(u(0))";
    check_sequences_defined_by(results, sequences);
    sequences.definitions[0] = "e^(u(0))";
    sequences.definitions[1] = "√(v(0))";
    sequences.definitions[2] = "log(u(0))";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = log(v(2)); v(n) = 10
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "log(v(2))";
    sequences.definitions[1] = "10";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // Self-reference in the initial condition
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {9., NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.types[1] = Sequence::Type::DoubleRecurrence;
    sequences.definitions[0] = "u(n)";
    sequences.definitions[1] = "u(n+1)+u(n)";
    sequences.conditions1[0] = "u(1)";
    sequences.conditions1[1] = "9";
    sequences.conditions2[1] = "u(u(0))";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+2) = u(n+1) + u(n), v(n) = u(n+1)
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {0., 1., 1., 2., 3., 5., 8., 13., 21., 34.},
        {1., 1., 2., 3., 5., 8., 13., 21., 34., 55.},
        {}};
    sequences.types[0] = Sequence::Type::DoubleRecurrence;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "u(n+1)+u(n)";
    sequences.definitions[1] = "u(n+1)";
    sequences.conditions1[0] = "0";
    sequences.conditions2[0] = "1";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n) = v(n+1), v(n) = u(n+1)
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        {}};
    sequences.types[0] = Sequence::Type::Explicit;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "v(n+1)";
    sequences.definitions[1] = "u(n+1)";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();

  // u(n+1) = u(n)+n-1, v(n) = u(10^n)
  {
    double results[SequenceStore::k_maxNumberOfSequences][10] = {
        {2.0, 1.0, 1.0, 2.0, 4.0, 7.0, 11.0, 16.0, 22.0, 29.0},
        {1.0, 37.0, 4852.0, 498502.0, 49985002.0, NAN, NAN, NAN, NAN, NAN}};
    sequences.types[0] = Sequence::Type::SingleRecurrence;
    sequences.types[1] = Sequence::Type::Explicit;
    sequences.definitions[0] = "u(n)+n-1";
    sequences.definitions[1] = "u(10^n)";
    sequences.conditions1[0] = "2";
    check_sequences_defined_by(results, sequences);
  }
  sequences.reset();
}

QUIZ_CASE(sequence_store) {
  SequenceStore* sequenceStore = &GlobalContextAccessor::SequenceStore();
  GlobalStore& globalStore = GlobalContextAccessor::Store();
  ProjectionContext projCtx = {.m_context = globalStore};

  PoincareTest::store("3→f(x)", GlobalContextAccessor::Store());
  simplified_approximates_to<double>("f(u(0))", "undef", projCtx);

  AddSequence(Sequence::Type::Explicit, "1", nullptr, nullptr);
  simplified_approximates_to<double>("f(u(2))", "3", projCtx);
  globalStore.resetAll();
  AddSequence(Sequence::Type::Explicit, "1/0", nullptr, nullptr);
  simplified_approximates_to<double>("f(u(2))", "undef", projCtx);

  sequenceStore->removeAll();
  PoincareTest::store("3→a", globalStore);
  AddSequence(Sequence::Type::Explicit, "a+1", nullptr, nullptr);
  simplified_approximates_to<double>("u(34)", "4", projCtx);
  PoincareTest::store("-3→a", globalStore);
  globalStore.storageDidChangeForRecord(Ion::Storage::Record("a.exp"));
  simplified_approximates_to<double>("u(34)", "-2", projCtx);
  globalStore.resetAll();
  sequenceStore->tidyDownstreamPoolFrom();
}

QUIZ_CASE(sequence_order) {
  SequenceStore* sequenceStore = &GlobalContextAccessor::SequenceStore();
  const SequenceContext* sequenceContext =
      &GlobalContextAccessor::SequenceContext();

  Sequence* u = AddSequence(Sequence::Type::Explicit, "1", nullptr, nullptr);
  quiz_assert(u->fullName()[0] == 'u');
  Sequence* v = AddSequence(Sequence::Type::Explicit, "2", nullptr, nullptr);
  quiz_assert(v->fullName()[0] == 'v');
  Sequence* w = AddSequence(Sequence::Type::Explicit, "3", nullptr, nullptr);
  quiz_assert(w->fullName()[0] == 'w');
  // Manually destroy u and v (but not w)
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("u.seq").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("v.seq").destroy();
  u = AddSequence(Sequence::Type::Explicit, "0", nullptr, nullptr);
  assert(u->fullName()[0] == 'u');
  v = AddSequence(Sequence::Type::Explicit, "1+w(1)", nullptr, nullptr);
  assert(v->fullName()[0] == 'v');

  sequenceContext->resetCache();
  quiz_assert(v->evaluateXYAtParameter(1.).y() == 4.);

  sequenceStore->removeAll();
  u = AddSequence(Sequence::Type::Explicit, "0", nullptr, nullptr);
  v = AddSequence(Sequence::Type::Explicit, "1", nullptr, nullptr);
  // Manually destroy u (but not v)
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("u.seq").destroy();
  sequenceContext->resetCache();
  quiz_assert(v->evaluateXYAtParameter(1.).y() == 1.);
}

QUIZ_CASE(sequence_sum_evaluation) {
  check_sum_of_sequence_between_bounds(33.0, 3.0, 8.0, Sequence::Type::Explicit,
                                       "n", nullptr, nullptr);
  check_sum_of_sequence_between_bounds(
      70.0, 2.0, 8.0, Sequence::Type::SingleRecurrence, "u(n)+2", "0", nullptr);
  check_sum_of_sequence_between_bounds(92.0, 2.0, 7.0,
                                       Sequence::Type::DoubleRecurrence,
                                       "u(n)+u(n+1)+2", "0", "0");
}

QUIZ_CASE(sequence_suitable_for_cobweb) {
  SequenceStore* store = &GlobalContextAccessor::SequenceStore();
  const SequenceContext& sequenceContext =
      GlobalContextAccessor::SequenceContext();
  quiz_assert(AddSequence(Sequence::Type::SingleRecurrence, "3(u(n)+2)+u(n)",
                          "0", nullptr)
                  ->isSuitableForCobweb());
  store->removeAll();
  quiz_assert(
      !AddSequence(Sequence::Type::SingleRecurrence, "v(n)+2", "0", nullptr)
           ->isSuitableForCobweb());
  store->removeAll();
  quiz_assert(!AddSequence(Sequence::Type::SingleRecurrence, "u(n)+cos(n)", "0",
                           nullptr)
                   ->isSuitableForCobweb());
  store->removeAll();
  quiz_assert(
      !AddSequence(Sequence::Type::SingleRecurrence, "2*u(n-2)", "0", nullptr)
           ->isSuitableForCobweb());
  store->removeAll();
  quiz_assert(
      AddSequence(Sequence::Type::SingleRecurrence, "2*u(n)+u(0)", "0", nullptr)
          ->isSuitableForCobweb());
  store->removeAll();
  sequenceContext.resetCache();  // for computation of u(0)
  quiz_assert(!AddSequence(Sequence::Type::SingleRecurrence, "2*u(n)+u(0)", "n",
                           nullptr)
                   ->isSuitableForCobweb());
}

QUIZ_CASE(sequence_notation_change) {
  check_sequence_notation(Sequence::Type::SingleRecurrence, "n+2", "2",
                          nullptr);
  check_sequence_notation(Sequence::Type::SingleRecurrence, "u(n)+3*(n+1)", "0",
                          nullptr);
  check_sequence_notation(Sequence::Type::DoubleRecurrence,
                          "u(n+1)-u(n)+n+2-n-1", "0", "1");
}

}  // namespace Shared
