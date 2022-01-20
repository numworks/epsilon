#include <quiz.h>
#include <apps/shared/global_context.h>
#include <string.h>
#include <assert.h>
#include <cmath>
#include "../../shared/sequence_store.h"
#include "../../shared/sequence_context.h"
#include "../../shared/poincare_helpers.h"

using namespace Poincare;

namespace Shared {

Sequence * addSequence(SequenceStore * store, Sequence::Type type, const char * definition, const char * condition1, const char * condition2, Context * context) {
  Ion::Storage::Record::ErrorStatus err = store->addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void) err; // Silence compilation warning about unused variable.
  Ion::Storage::Record record = store->recordAtIndex(store->numberOfModels()-1);
  Sequence * u = store->modelForRecord(record);
  u->setType(type);
  u->setContent(definition, context);
  if (condition1) {
    u->setFirstInitialConditionContent(condition1, context);
  }
  if (condition2) {
    u->setSecondInitialConditionContent(condition2, context);
  }
  return u;
}

void check_sequences_defined_by(double result[MaxNumberOfSequences][10], Sequence::Type types[MaxNumberOfSequences], const char * definitions[MaxNumberOfSequences], const char * conditions1[MaxNumberOfSequences], const char * conditions2[MaxNumberOfSequences]) {
  Shared::GlobalContext globalContext;
  SequenceStore * store = globalContext.sequenceStore();
  SequenceContext sequenceContext(&globalContext, store);

  Sequence * seqs[MaxNumberOfSequences];
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    seqs[i] = addSequence(store, types[i], definitions[i], conditions1[i], conditions2[i], &globalContext);
  }

  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < MaxNumberOfSequences; i++) {
      if (seqs[i]->isDefined()) {
        double un = seqs[i]->evaluateXYAtParameter((double)j, &sequenceContext).x2();
        quiz_assert((std::isnan(un) && std::isnan(result[i][j])) || (un == result[i][j]));
      }
    }
  }
  store->removeAll();
  /* The store is a global variable that has been constructed through
   * GlobalContext::sequenceStore singleton. It won't be destructed. However,
   * we need to make sure that the pool is empty between quiz_cases. */
  store->tidy();
}

void check_sum_of_sequence_between_bounds(double result, double start, double end, Sequence::Type type, const char * definition, const char * condition1, const char * condition2) {
  Shared::GlobalContext globalContext;
  SequenceStore * store = globalContext.sequenceStore();
  SequenceContext sequenceContext(&globalContext, store);

  Sequence * seq = addSequence(store, type, definition, condition1, condition2, &globalContext);

  double sum = PoincareHelpers::ApproximateToScalar<double>(seq->sumBetweenBounds(start, end, &sequenceContext), &globalContext);
  quiz_assert(std::fabs(sum - result) < 0.00000001);

  store->removeAll();
  store->tidy(); // Cf comment above
}

QUIZ_CASE(sequence_evaluation) {
  Sequence::Type types[MaxNumberOfSequences] = {Sequence::Type::Explicit, Sequence::Type::Explicit, Sequence::Type::Explicit};
  const char * definitions[MaxNumberOfSequences] = {nullptr, nullptr, nullptr};
  const char * conditions1[MaxNumberOfSequences] = {nullptr, nullptr, nullptr};
  const char * conditions2[MaxNumberOfSequences] = {nullptr, nullptr, nullptr};

  // u(n) = n
  double results1[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {}, {}};
  definitions[0] = "n";
  check_sequences_defined_by(results1, types, definitions, conditions1, conditions2);

  // u(n+1) = u(n)+n, u(0) = 0
  double results2[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {}, {}};
  types[0] = Sequence::Type::SingleRecurrence;
  definitions[0] = "u(n)+n";
  conditions1[0] = "0";
  check_sequences_defined_by(results2, types, definitions, conditions1, conditions2);

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0
  double results3[MaxNumberOfSequences][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0}, {}, {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "u(n)+u(n+1)+n";
  conditions1[0] = "0";
  conditions2[0] = "0";
  check_sequences_defined_by(results3, types, definitions, conditions1, conditions2);

  // u independent, v defined with u
  // u(n) = n; v(n) = u(n)+n
  double results4[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0},
    {}};
  types[0] = Sequence::Type::Explicit;
  definitions[0] = "n";
  definitions[1] = "u(n)+n";
  conditions1[0] = nullptr;
  conditions2[0] = nullptr;
  check_sequences_defined_by(results4, types, definitions, conditions1, conditions2);

  // u(n+1) = u(n)+n, u(0) = 0; v(n) = u(n)+n
  double results5[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
    {0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0, 45.0}, {}};
  types[0] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n+u(n)";
  conditions1[0] = "0";
  check_sequences_defined_by(results5, types, definitions, conditions1, conditions2);

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n) = u(n)+n
  double results6[MaxNumberOfSequences][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
    {0.0, 1.0, 2.0, 4.0, 7.0, 12.0, 20.0, 33.0, 54.0, 88.0}, {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "n+u(n)+u(n+1)";
  conditions2[0] = "0";
  check_sequences_defined_by(results6, types, definitions, conditions1, conditions2);

  // u(n) = n; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  double results7[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {0.0, 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0, 25.0}, {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n";
  definitions[1] = "n+u(n)+u(n+1)";
  conditions1[0] = nullptr;
  conditions1[1] = "0";
  conditions2[0] = nullptr;
  check_sequences_defined_by(results7, types, definitions, conditions1, conditions2);

  // u(n+1) = u(n)+n, u(0) = 0; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  double results8[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
    {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0}, {}};
  types[0] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n+u(n)";
  definitions[1] = "n+u(n)+u(n+1)";
  conditions1[0] = "0";
  check_sequences_defined_by(results8, types, definitions, conditions1, conditions2);

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  double results9[MaxNumberOfSequences][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
    {0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0, 133.0}, {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "n+u(n)+u(n+1)";
  conditions2[0] = "0";
  check_sequences_defined_by(results9, types, definitions, conditions1, conditions2);

  // u(n) = n; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  double results10[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {0.0, 0.0, 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0}, {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "n";
  conditions1[0] = nullptr;
  conditions2[0] = nullptr;
  conditions2[1] = "0";
  check_sequences_defined_by(results10, types, definitions, conditions1, conditions2);

  // u(n+1) = u(n)+n, u(0) = 0; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  double results11[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
    {0.0, 0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0}, {}};
  types[0] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n+u(n)";
  conditions1[0] = "0";
  check_sequences_defined_by(results11, types, definitions, conditions1, conditions2);

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  double results12[MaxNumberOfSequences][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
    {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0}, {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "u(n+1)+u(n)+n";
  conditions2[0] = "0";
  check_sequences_defined_by(results12, types, definitions, conditions1, conditions2);

  // v independent, u defined with v

  // u(n) = v(n); v(n) = u(n)
  double results13[MaxNumberOfSequences][10] = {{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::Explicit;
  definitions[0] = "v(n)";
  definitions[1] = "u(n)";
  conditions1[0] = nullptr;
  conditions1[1] = nullptr;
  conditions2[0] = nullptr;
  conditions2[1] = nullptr;
  check_sequences_defined_by(results13, types, definitions, conditions1, conditions2);

  // u(n+1) = v(n)+n, u(0)=0; v(n) = u(n)+n
  double results14[MaxNumberOfSequences][10] = {{0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0}, {0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81}, {}};
  types[0] = Sequence::Type::SingleRecurrence;
  definitions[0] = "v(n)+n";
  definitions[1] = "u(n)+n";
  conditions1[0] = "0";
  check_sequences_defined_by(results14, types, definitions, conditions1, conditions2);

  // u(n+2) = v(n+1)+u(n+1)+v(n)+u(n)+n, u(1) = 0; u(0)=0; v(n) = u(n)+n
  double results15[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 6.0, 21.0, 64.0, 183.0, 510.0, 1405.0, 3852.0}, {0.0, 1.0, 3.0, 9.0, 25.0, 69.0, 189.0, 517.0, 1413.0, 3861.0}, {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "v(n+1)+v(n)+u(n+1)+u(n)+n";
  conditions1[0] = "0";
  conditions2[0] = "0";
  check_sequences_defined_by(results15, types, definitions, conditions1, conditions2);

  // u(n) = v(n)+n; v(n+1) = u(n)+n, v(0)=0
  double results16[MaxNumberOfSequences][10] = {{0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81},
    {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0}, {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::SingleRecurrence;
  definitions[0] = "v(n)+n";
  conditions1[0] = nullptr;
  conditions1[1] = "0";
  conditions2[0] = nullptr;
  check_sequences_defined_by(results16, types, definitions, conditions1, conditions2);

  // u(n+1) = v(n)+n, u(0)=0; v(n+1) = u(n)+n, v(0)=0
  double results17[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {}};
  types[0] = Sequence::Type::SingleRecurrence;
  conditions1[0] = "0";
  check_sequences_defined_by(results17, types, definitions, conditions1, conditions2);

  // u(n+2) = v(n+1)+u(n+1)+v(n)+u(n)+n, u(1) = 0, u(0) = 0; v(n+1) = u(n)+n, v(0)=0
  double results18[MaxNumberOfSequences][10] = {{0.0, 0.0, 0.0, 2.0, 7.0, 19.0, 46.0, 105.0, 233.0, 509.0}, {0.0, 0.0, 1.0, 2.0, 5.0, 11.0, 24.0, 52.0, 112.0, 241.0}, {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "v(n+1)+v(n)+u(n+1)+u(n)+n";
  conditions2[0] = "0";
  check_sequences_defined_by(results18, types, definitions, conditions1, conditions2);

  // u(n) = n; v undefined
  double results19[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::Explicit;
  definitions[0] = "n";
  definitions[1] = nullptr;
  conditions1[0] = nullptr;
  conditions1[1] = nullptr;
  conditions2[0] = nullptr;
  conditions2[1] = nullptr;
  check_sequences_defined_by(results19, types, definitions, conditions1, conditions2);

  // u undefined; v(n) = n
  double results20[MaxNumberOfSequences][10] = {{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {}};
  definitions[0] =  nullptr;
  definitions[1] = "n";
  check_sequences_defined_by(results20, types, definitions, conditions1, conditions2);

  // u, v, w independent

  // u(n) = n; v(n+1) = 9+n, v(0) = 0; w(n) = n+2
  double results21[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {0.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0}, {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0}};
  types[1] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n";
  definitions[1] = "9+n";
  definitions[2] = "n+2";
  conditions1[1] = "0";
  check_sequences_defined_by(results21, types, definitions, conditions1, conditions2);

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n) = 9+n; w(n+1) = w(n)+2, w(0) = 0
  double results22[MaxNumberOfSequences][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0}, {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0}, {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0}};
  types[0] = Sequence::Type::DoubleRecurrence;
  types[1] = Sequence::Type::Explicit;
  types[2] = Sequence::Type::SingleRecurrence;
  definitions[0] = "u(n+1)+u(n)+n";
  definitions[2] = "2+w(n)";
  conditions1[0] = "0";
  conditions1[1] = nullptr;
  conditions1[2] = "0";
  conditions2[0] = "0";
  check_sequences_defined_by(results22, types, definitions, conditions1, conditions2);

  // u(n+1) = u(n)+n, u(0) = 0; v(n) = 9+n; w(n+2) = w(n+1)+w(n)+2, w(0) = 0, w(1) = 0
  double results23[MaxNumberOfSequences][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0}, {0.0, 0.0, 2.0, 4.0, 8.0, 14.0, 24.0, 40.0, 66.0, 108.0}};
  types[0] = Sequence::Type::SingleRecurrence;
  types[2] = Sequence::Type::DoubleRecurrence;
  definitions[0] = "u(n)+n";
  definitions[2] = "w(n+1)+w(n)+2";
  conditions2[2] = "0";
  conditions2[0] = nullptr;
  check_sequences_defined_by(results23, types, definitions, conditions1, conditions2);

  // v depends on u, w depends on v & u independent
  // u(n) = n ; v(n+1) = u(n)+1, v(0) = 0, w(n) = 2+v(n)
  double results24[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::SingleRecurrence;
  types[2] = Sequence::Type::Explicit;
  definitions[0] = "n";
  definitions[1] = "u(n)+1";
  definitions[2] = "2+v(n)";
  conditions1[1] = "0";
  check_sequences_defined_by(results24, types, definitions, conditions1, conditions2);

  // u(n) = n ; v(n+1) = u(n)+1, v(0) = 0, w(n+1) = 2+v(n+1), w(0) = 0
  double results25[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {0.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0}};
  types[2] = Sequence::Type::SingleRecurrence;
  definitions[2] = "2+v(n+1)";
  conditions1[2] = "0";
  check_sequences_defined_by(results25, types, definitions, conditions1, conditions2);

  // v depends on u, w depends on u and v & u independent
  // u(n) = n ; v(n+1) = u(n)+1, v(0) = 0, w(n+2) = 2+v(n+1)+u(n+1), w(0) = 0, w(1) = 0
  double results26[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {0.0, 0.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0}};
  types[2] = Sequence::Type::DoubleRecurrence;
  definitions[2] = "2+v(n+1)+u(n+1)";
  conditions1[2] = "0";
  conditions2[2] = "0";
  check_sequences_defined_by(results26, types, definitions, conditions1, conditions2);

  // v depends on u, w depends on v & u depends on w
  // u(n+1) = w(n)+1, u(0) = 0; v(n+1) = u(n)+1, v(0) = 0, w(n+1) = v(n)+2, w(0) = 0
  double results27[MaxNumberOfSequences][10] = {{0.0, 1.0, 3.0, 4.0, 5.0, 7.0, 8.0, 9.0, 11.0, 12.0}, {0.0, 1.0, 2.0, 4.0, 5.0, 6.0, 8.0, 9.0, 10.0, 12.0}, {0.0, 2.0, 3.0, 4.0, 6.0, 7.0, 8.0, 10.0, 11.0, 12.0}};
  types[0] = Sequence::Type::SingleRecurrence;
  types[2] = Sequence::Type::SingleRecurrence;
  definitions[0] = "w(n)+1";
  definitions[1] = "u(n)+1";
  definitions[2] = "v(n)+2";
  conditions1[0] = "0";
  conditions1[1] = "0";
  conditions1[2] = "0";
  conditions2[2] = nullptr;
  check_sequences_defined_by(results27, types, definitions, conditions1, conditions2);

  // v depends on u, w depends on v & u depends on w
  // u(n) = w(n); v(n) = u(n), w(n) = v(n)
  double results28[MaxNumberOfSequences][10] = {{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::Explicit;
  types[2] = Sequence::Type::Explicit;
  definitions[0] = "w(n)";
  definitions[1] = "u(n)";
  definitions[2] = "v(n)";
  conditions1[0] = nullptr;
  conditions1[1] = nullptr;
  conditions1[2] = nullptr;
  conditions2[2] = nullptr;
  check_sequences_defined_by(results28, types, definitions, conditions1, conditions2);

  // u independent, v depends on u(3)
  // u(n) = n; v(n) = u(5)+n
  double results29[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0},
    {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::Explicit;
  definitions[0] = "n";
  definitions[1] = "u(5)+n";
  definitions[2] = nullptr;
  conditions1[0] = nullptr;
  conditions2[0] = nullptr;
  conditions1[1] = nullptr;
  conditions2[1] = nullptr;
  conditions1[2] = nullptr;
  conditions2[2] = nullptr;
  check_sequences_defined_by(results29, types, definitions, conditions1, conditions2);

// u independent, v depends on u(2)
// u(n) = n; v(n+1) = v(n)-u(2)
  double results30[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {2.0, 0.0, -2.0, -4.0, -6.0, -8.0, -10.0, -12.0, -14.0, -16.0},
    {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n";
  definitions[1] = "v(n)-u(2)";
  conditions1[0] = nullptr;
  conditions2[0] = nullptr;
  conditions1[1] = "u(2)";
  check_sequences_defined_by(results30, types, definitions, conditions1, conditions2);

// u and v interdependent
// u(n+2) = n + v(3) + u(n+1) - u(n); v(n) = u(n) - u(1)
  double results31[MaxNumberOfSequences][10] = {{0.0, 3.0, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    {-3.0, 0.0, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    {}};
  types[0] = Sequence::Type::DoubleRecurrence;
  types[1] = Sequence::Type::Explicit;
  definitions[0] = "n+v(3)+u(n+1)-u(n)";
  definitions[1] = "u(n)-u(1)";
  conditions1[0] = "0";
  conditions2[0] = "3";
  check_sequences_defined_by(results31, types, definitions, conditions1, conditions2);

// u is independent, v depends on u(120) and w(5), w depends on u(8)
// u(n) = n; v(n+2) = v(n+1) + v(n) + u(120); w(n+1) = w(n) - u(8)
  double results32[MaxNumberOfSequences][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {46.0, 6.0, 172.0, 298.0, 590.0, 1008.0, 1718.0, 2846.0, 4684.0, 7650.0},
    {6.0, 14.0, 22.0, 30.0, 38.0, 46.0, 54.0, 62.0, 70.0, 78.0}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::DoubleRecurrence;
  types[2] = Sequence::Type::SingleRecurrence;
  definitions[0] = "n";
  definitions[1] = "v(n+1)+v(n)+u(120)";
  definitions[2] = "w(n)+u(8)";
  conditions1[0] = nullptr;
  conditions2[0] = nullptr;
  conditions1[1] = "w(5)";
  conditions2[1] = "6";
  conditions1[2] = "6";
  conditions2[2] = nullptr;
  check_sequences_defined_by(results32, types, definitions, conditions1, conditions2);

  // u independent, v depends on u(n+6)
  // u(n) = 9n; v(n+1) = u(n+6)+v(0); v(0) = 9
  double results33[MaxNumberOfSequences][10] = {{0.0, 9.0, 18.0, 27.0, 36.0, 45.0, 54.0, 63.0, 72.0, 81.0},
    {9.0, 63.0, 72.0, 81.0, 90.0, 99.0, 108.0, 117.0, 126.0, 135.0},
    {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::SingleRecurrence;
  definitions[0] = "9n";
  definitions[1] = "u(n+6)+v(0)";
  definitions[2] = nullptr;
  conditions1[1] = "9";
  check_sequences_defined_by(results33, types, definitions, conditions1, conditions2);

  // Explicit self-referencing sequences
  double results34[MaxNumberOfSequences][10] = {
    {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}
  };
  types[0] = Sequence::Type::Explicit; types[1] = Sequence::Type::Explicit; types[2] = Sequence::Type::Explicit;
  conditions1[0] = nullptr; conditions1[1] = nullptr; conditions1[2] = nullptr;
  conditions2[0] = nullptr; conditions2[1] = nullptr; conditions2[2] = nullptr;
  definitions[0] = "|u(0)|";
  definitions[1] = "floor(v(0))";
  definitions[2] = "ceil(w(0))";
  check_sequences_defined_by(results34, types, definitions, conditions1, conditions2);
  definitions[0] = "acos(u(0))";
  definitions[1] = "asin(v(0))";
  definitions[2] = "atan(w(0))";
  check_sequences_defined_by(results34, types, definitions, conditions1, conditions2);
  definitions[0] = "cos(u(0))";
  definitions[1] = "sin(v(0))";
  definitions[2] = "tan(w(0))";
  check_sequences_defined_by(results34, types, definitions, conditions1, conditions2);
  definitions[0] = "1+u(0)";
  definitions[1] = "2*v(0)";
  definitions[2] = "2^(u(0))";
  check_sequences_defined_by(results34, types, definitions, conditions1, conditions2);
  definitions[0] = "ℯ^(u(0))";
  definitions[1] = "√(v(0))";
  definitions[2] = "log(u(0))";
  check_sequences_defined_by(results34, types, definitions, conditions1, conditions2);

  // u(n) = log(v(2)); v(n) = 10
  double results35[MaxNumberOfSequences][10] = {{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
    {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0},
    {}};
  types[0] = Sequence::Type::Explicit;
  types[1] = Sequence::Type::Explicit;
  definitions[0] = "log(v(2))";
  definitions[1] = "10";
  definitions[2] = nullptr;
  check_sequences_defined_by(results35, types, definitions, conditions1, conditions2);
}

QUIZ_CASE(sequence_sum_evaluation) {
  check_sum_of_sequence_between_bounds(33.0, 3.0, 8.0, Sequence::Type::Explicit, "n", nullptr, nullptr);
  check_sum_of_sequence_between_bounds(70.0, 2.0, 8.0, Sequence::Type::SingleRecurrence, "u(n)+2", "0", nullptr);
  check_sum_of_sequence_between_bounds(92.0, 2.0, 7.0, Sequence::Type::DoubleRecurrence, "u(n)+u(n+1)+2", "0", "0");
}

}
