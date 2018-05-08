#include <quiz.h>
#include <string.h>
#include <assert.h>
#include <cmath>
#include "../sequence_store.h"
#include "../sequence_context.h"

using namespace Poincare;

namespace Sequence {

void check_sequences_defined_by(double result[2][10], Sequence::Type typeU, const char * definitionU, const char * conditionU1 = nullptr, const char * conditionU2 = nullptr, Sequence::Type typeV = Sequence::Type::Explicit, const char * definitionV = nullptr, const char * conditionV1 = nullptr, const char * conditionV2 = nullptr) {
  GlobalContext globalContext;
  SequenceStore store;
  SequenceContext sequenceContext(&globalContext, &store);

  Sequence * u = nullptr;
  Sequence * v = nullptr;
  if (definitionU) {
    u = store.addEmptyFunction();
    assert(u->name()[0] == 'u');
    u->setType(typeU);
    u->setContent(definitionU);
    if (conditionU1) {
      u->setFirstInitialConditionContent(conditionU1);
    }
    if (conditionU2) {
      u->setSecondInitialConditionContent(conditionU2);
    }
  }
  if (definitionV) {
    if (store.numberOfFunctions() == 0) {
      Sequence * tempU = store.addEmptyFunction();
      v = store.addEmptyFunction();
      store.removeFunction(tempU);
      v = store.functionAtIndex(0);
    } else {
      assert(store.numberOfFunctions() == 1);
      v = store.addEmptyFunction();
    }
    v->setType(typeV);
    v->setContent(definitionV);
    if (conditionV1) {
      v->setFirstInitialConditionContent(conditionV1);
    }
    if (conditionV2) {
      v->setSecondInitialConditionContent(conditionV2);
    }
  }
  for (int j = 0; j < 10; j++) {
    if (u && u->isDefined()) {
      double un = u->evaluateAtAbscissa((double)j, &sequenceContext);
      assert((std::isnan(un) && std::isnan(result[0][j])) || (un == result[0][j]));
    }
    if (v && v->isDefined()) {
      double vn = v->evaluateAtAbscissa((double)j, &sequenceContext);
      assert((std::isnan(vn) && std::isnan(result[1][j])) || (vn == result[1][j]));
    }
  }
}

QUIZ_CASE(sequence_evaluation) {
  // u(n) = n
  double result0[2][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {}};
  check_sequences_defined_by(result0, Sequence::Type::Explicit, "n");

  // u(n+1) = u(n)+n, u(0) = 0
  double result1[2][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {}};
  check_sequences_defined_by(result1, Sequence::Type::SingleRecurrence, "u(n)+n", "0");

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0
  double result2[2][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0}, {}};
  check_sequences_defined_by(result2, Sequence::Type::DoubleRecurrence, "u(n)+u(n+1)+n", "0", "0");

  // u independent, v defined with u
  // u(n) = n; v(n) = u(n)+n
  double result3[2][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0}};
  check_sequences_defined_by(result3, Sequence::Type::Explicit, "n", nullptr, nullptr, Sequence::Type::Explicit, "u(n)+n");

  // u(n+1) = u(n)+n, u(0) = 0; v(n) = u(n)+n
  double result4[2][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
    {0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0, 45.0}};
  check_sequences_defined_by(result4, Sequence::Type::SingleRecurrence, "n+u(n)", "0", nullptr, Sequence::Type::Explicit, "u(n)+n");

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n) = u(n)+n
  double result5[2][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
    {0.0, 1.0, 2.0, 4.0, 7.0, 12.0, 20.0, 33.0, 54.0, 88.0}};
  check_sequences_defined_by(result5, Sequence::Type::DoubleRecurrence, "n+u(n)+u(n+1)", "0", "0", Sequence::Type::Explicit, "u(n)+n");

  // u(n) = n; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  double result6[2][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {0.0, 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0, 25.0}};
  check_sequences_defined_by(result6, Sequence::Type::Explicit, "n", nullptr, nullptr, Sequence::Type::SingleRecurrence, "u(n)+u(n+1)+n", "0");

  // u(n+1) = u(n)+n, u(0) = 0; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  double result7[2][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
    {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0}};
  check_sequences_defined_by(result7, Sequence::Type::SingleRecurrence, "n+u(n)", "0", nullptr, Sequence::Type::SingleRecurrence, "u(n)+u(n+1)+n", "0");

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n+1) = u(n)+u(n+1)+n, v(0) = 0
  double result8[2][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
    {0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0, 133.0}};
  check_sequences_defined_by(result8, Sequence::Type::DoubleRecurrence, "n+u(n)+u(n+1)", "0", "0", Sequence::Type::SingleRecurrence, "u(n)+u(n+1)+n", "0");

  // u(n) = n; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  double result9[2][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
    {0.0, 0.0, 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0}};
  check_sequences_defined_by(result9, Sequence::Type::Explicit, "n", nullptr, nullptr, Sequence::Type::DoubleRecurrence, "u(n)+u(n+1)+n", "0", "0");

  // u(n+1) = u(n)+n, u(0) = 0; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  double result10[2][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0},
    {0.0, 0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0}};
  check_sequences_defined_by(result10, Sequence::Type::SingleRecurrence, "n+u(n)", "0", nullptr, Sequence::Type::DoubleRecurrence, "u(n)+u(n+1)+n", "0", "0");

  // u(n+2) = u(n+1)+u(n)+n, u(0) = 0, u(1) = 0; v(n+2) = u(n)+u(n+1)+n, v(0) = 0, v(1)=0
  double result11[2][10] = {{0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0},
    {0.0, 0.0, 0.0, 1.0, 3.0, 7.0, 14.0, 26.0, 46.0, 79.0}};
  check_sequences_defined_by(result11, Sequence::Type::DoubleRecurrence, "n+u(n)+u(n+1)", "0", "0", Sequence::Type::DoubleRecurrence, "u(n)+u(n+1)+n", "0", "0");

  // v independent, u defined with v

  // u(n) = v(n); v(n) = u(n)
  double result12[2][10] = {{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}};
  check_sequences_defined_by(result12, Sequence::Type::Explicit, "v(n)", nullptr, nullptr, Sequence::Type::Explicit, "u(n)");

  // u(n+1) = v(n)+n, u(0)=0; v(n) = u(n)+n
  double result13[2][10] = {{0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0}, {0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81}};
  check_sequences_defined_by(result13, Sequence::Type::SingleRecurrence, "v(n)+n", "0", nullptr, Sequence::Type::Explicit, "u(n)+n");

  // u(n+2) = v(n+1)+u(n+1)+v(n)+u(n)+n, u(1) = 0; u(0)=0; v(n) = u(n)+n
  double result14[2][10] = {{0.0, 0.0, 1.0, 6.0, 21.0, 64.0, 183.0, 510.0, 1405.0, 3852.0}, {0.0, 1.0, 3.0, 9.0, 25.0, 69.0, 189.0, 517.0, 1413.0, 3861.0}};
  check_sequences_defined_by(result14, Sequence::Type::DoubleRecurrence, "v(n+1)+v(n)+u(n+1)+u(n)+n", "0", "0", Sequence::Type::Explicit, "u(n)+n");

  // u(n) = v(n)+n; v(n+1) = u(n)+n, v(0)=0
  double result15[2][10] = {{0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81},
    {0.0, 0.0, 2.0, 6.0, 12.0, 20.0, 30.0, 42.0, 56.0, 72.0}};
  check_sequences_defined_by(result15, Sequence::Type::Explicit, "v(n)+n", nullptr, nullptr, Sequence::Type::SingleRecurrence, "u(n)+n", "0");

  // u(n+1) = v(n)+n, u(0)=0; v(n+1) = u(n)+n, v(0)=0
  double result16[2][10] = {{0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}, {0.0, 0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0}};
  check_sequences_defined_by(result16, Sequence::Type::SingleRecurrence, "v(n)+n", "0", nullptr, Sequence::Type::SingleRecurrence, "u(n)+n", "0");

  // u(n+2) = v(n+1)+u(n+1)+v(n)+u(n)+n, u(1) = 0, u(0) = 0; v(n+1) = u(n)+n, v(0)=0
  double result17[2][10] = {{0.0, 0.0, 0.0, 2.0, 7.0, 19.0, 46.0, 105.0, 233.0, 509.0}, {0.0, 0.0, 1.0, 2.0, 5.0, 11.0, 24.0, 52.0, 112.0, 241.0}};
  check_sequences_defined_by(result17, Sequence::Type::DoubleRecurrence, "v(n+1)+v(n)+u(n+1)+u(n)+n", "0", "0", Sequence::Type::SingleRecurrence, "u(n)+n", "0");

  // u(n) = n; v undefined
  double result18[2][10] = {{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}};
  check_sequences_defined_by(result18, Sequence::Type::Explicit, "n");

  // u undefined; v(n) = n
  double result19[2][10] = {{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}};
  check_sequences_defined_by(result19, Sequence::Type::Explicit, nullptr, nullptr, nullptr, Sequence::Type::Explicit, "n");
}

}
