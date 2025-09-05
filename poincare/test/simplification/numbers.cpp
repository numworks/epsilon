#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/systematic_reduction.h>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare::Internal;
using Poincare::Strategy;

QUIZ_CASE(pcj_simplification_decimal) {
  Tree* tree = SharedTreeStack->pushDecimal();
  (124_e)->cloneTree();
  (-2_e)->cloneTree();
  Poincare::ProjectionContext ctx = realCtx;
  simplify(tree, ctx);
  assert_trees_are_equal(tree, 12400_e);
  tree->removeTree();
  tree = SharedTreeStack->pushDecimal();
  (124_e)->cloneTree();
  (2_e)->cloneTree();
  simplify(tree, ctx);
  assert_trees_are_equal(tree, KDiv(31_e, 25_e));
  tree->removeTree();
  // Decimal with integers larger than ints
  simplifies_to(
      ".99999999999999999999999999999999 - 3*.33333333333333333333333333333333",
      "0");

  simplifies_to("-2.3", "-23/10");
  simplifies_to("-232.2ᴇ-4", "-1161/50000");
  simplifies_to("0000.000000ᴇ-2", "0");
  simplifies_to(".000000", "0");
  simplifies_to("0000", "0");
}

QUIZ_CASE(pcj_simplification_float) {
  simplifies_to("2", "2", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("2.3", "2.3", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π", "4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π+x", "x+4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("cos(x-x)", "1", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("random()-random()", "random()-random()",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("y^3*x^-2", "y^3/x^2",
                {.m_strategy = Strategy::ApproximateToFloat});

  // This was raising asserts because of float approximation on flatten.
  Tree* u = (KMult(KPow(180_e, -1_e), π_e, KMult(180_de, "x"_e)))->cloneTree();
  SystematicReduction::ShallowReduce(u->child(0));
  QUIZ_ASSERT(SystematicReduction::ShallowReduce(u));
  QUIZ_ASSERT(!SystematicReduction::ShallowReduce(u));
}

QUIZ_CASE(pcj_simplification_constants) {
  simplifies_to("i", "nonreal", realCtx);
  simplifies_to("i", "i", cartesianCtx);
  simplifies_to("π", "π");
  simplifies_to("e", "e");
  simplifies_to("_mn + _mp", "3.34754942651ᴇ-27×_kg");
  simplifies_to("_mn + _G", "undef");
  simplifies_to("_c", "299792458×_m×_s^(-1)");
  simplifies_to("_e", "1.602176634ᴇ-19×_C");
  simplifies_to("_G", "6.6743ᴇ-11×_m^3×_kg^(-1)×_s^(-2)");
  simplifies_to("_g0", "9.80665×_m×_s^(-2)");
  simplifies_to("_k", "1.380649ᴇ-23×_J×_K^(-1)");
  simplifies_to("_ke", "8987551792.3×_m×_F^(-1)");
  simplifies_to("_me", "9.1093837139ᴇ-31×_kg");
  simplifies_to("_mn", "1.67492750056ᴇ-27×_kg");
  simplifies_to("_mp", "1.67262192595ᴇ-27×_kg");
  simplifies_to("_Na", "6.02214076ᴇ23×_mol^(-1)");
  simplifies_to("_R", "8.3144626181532×_J×_K^(-1)×_mol^(-1)");
  simplifies_to("_ε0", "8.8541878188ᴇ-12×_F×_m^(-1)");
  simplifies_to("_μ0", "1.25663706127ᴇ-6×_N×_A^(-2)");
  simplifies_to("_hplanck", "6.62607015ᴇ-34×_s×_J");
}

QUIZ_CASE(pcj_simplification_based_integer) {
  simplifies_to("0b10011", "19");
  simplifies_to("0x2A", "42");
}

QUIZ_CASE(pcj_simplification_large_integer_no_crash) {
  simplifies_to("diff(x,x,0,100000000000)", "diff(x,x,0,100000000000)");
  simplifies_to("40000000000ln(10)", "40000000000ln(10)");
}

QUIZ_CASE(pcj_simplification_large_numbers) {
  simplifies_to("123450000000000000000000000000", "1.2345×10^29");
  simplifies_to("123450000000000000000000000000/(5×7)", "(2.469×10^28)/7");
  simplifies_to("π/(12345×10^5)^2", "π/(1.52399025×10^18)");
  simplifies_to("10^14+i", "10^14+i", cartesianCtx);
  simplifies_to("e^(3×10^15)", "e^(3×10^15)");

  /* Edge cases */
  simplifies_to("1×10^12", "1000000000000");
  simplifies_to("1234567890123000", "1.234567890123×10^15");
  simplifies_to("12345678901234×10^15", "12345678901234000000000000000");
  simplifies_to("123456789012300", "123456789012300");
  simplifies_to("1234567890123456789012345678000",
                "1.234567890123456789012345678×10^30");
  // Hidden in Epsilon's calculation app exact results
  simplifies_to("123456789012345678901234567800",
                "123456789012345678901234567800");
  simplifies_to("1234567890123456789012345678901×10^15",
                "1234567890123456789012345678901000000000000000");
  simplifies_to("2^101*2", "5070602400912917605986812821504");
  simplifies_to("2.3×10^45", "2.3×10^45");
}

QUIZ_CASE(poincare_simplification_rational) {
  // 1/MaxParsedIntegerString()
  constexpr static size_t k_bufferSizeOfMax = 32;
  char bufferMax[k_bufferSizeOfMax] = "1/";
  size_t bufferLengthOfMax = strlen(bufferMax);
  strlcpy(bufferMax + bufferLengthOfMax, MaxParsedIntegerString(),
          k_bufferSizeOfMax - bufferLengthOfMax);
  simplifies_to(bufferMax, bufferMax);
  // 1/OverflowedIntegerString()
  constexpr static size_t k_bufferSizeOfInf = 400;
  char bufferInf[k_bufferSizeOfInf] = "1/";
  size_t bufferLengthOfInf = strlen(bufferInf);
  strlcpy(bufferInf + bufferLengthOfInf, BigOverflowedIntegerString(),
          k_bufferSizeOfInf - bufferLengthOfInf);
  assert_parse_to_integer_overflow(bufferInf);
  // MaxParsedIntegerString()
  simplifies_to(MaxParsedIntegerString(), MaxParsedIntegerString());
  // OverflowedIntegerString()
  assert_parse_to_integer_overflow(OverflowedIntegerString());
  assert_parse_to_integer_overflow(BigOverflowedIntegerString());
  // ApproximatedParsedIntegerString()
  simplifies_to(ApproximatedParsedIntegerString(), "10^30");
  // -OverflowedIntegerString()
  bufferInf[0] = '-';
  bufferLengthOfInf = 1;
  strlcpy(bufferInf + bufferLengthOfInf, BigOverflowedIntegerString(),
          k_bufferSizeOfInf - bufferLengthOfInf);
  assert_parse_to_integer_overflow(bufferInf);

  simplifies_to("-1/3", "-1/3");
  simplifies_to("22355/45325", "4471/9065");
  simplifies_to("0000.000000", "0");
  simplifies_to(".000000", "0");
  simplifies_to("0000", "0");
  simplifies_to("0.1234567", "1234567/10000000");
  simplifies_to("123.4567", "1234567/10000");
  simplifies_to("0.1234", "617/5000");
  simplifies_to("0.1234000", "617/5000");
  simplifies_to("001234000", "1234000");
  simplifies_to("001.234000ᴇ3", "1234");
  simplifies_to("001234000ᴇ-4", "617/5");
  simplifies_to("3/4+5/4-12+1/567", "-5669/567");
  simplifies_to("34/78+67^(-1)", "1178/2613");
  simplifies_to("12348/34564", "3087/8641");
  simplifies_to("1-0.3-0.7", "0");
  simplifies_to("123456789123456789+112233445566778899", "235690234690235688");
  simplifies_to("56^56",
                "79164324866862966607842406018063254671922245312646690223362402"
                "918484170424104310169552592050323456");
  simplifies_to("999^999", "undef");
  simplifies_to("999^-999", "undef");
  simplifies_to("0^0", "undef");
  simplifies_to("π^0", "1");
  simplifies_to("(-3)^0", "1");
  simplifies_to("2ᴇ150/2ᴇ150", "1");
}

QUIZ_CASE(pcj_simplification_infinity) {
  simplifies_to("inf", "∞");
  simplifies_to("inf(-1)", "-∞");
  simplifies_to("inf-1", "∞");
  simplifies_to("-inf+1", "-∞");
  simplifies_to("inf+inf", "∞");
  simplifies_to("-inf-inf", "-∞");
  simplifies_to("inf-inf", "undef");
  simplifies_to("-inf+inf", "undef");
  simplifies_to("inf-inf+3*inf", "undef");
  simplifies_to("inf*(-π)", "-∞");
  simplifies_to("inf*2*inf", "∞");
  simplifies_to("0×inf", "undef");
  simplifies_to("3×inf", "∞");
  simplifies_to("-3×inf", "-∞");
  simplifies_to("inf×(-inf)", "-∞");
  simplifies_to("x×(-inf)", "∞×sign(-x)");
  simplifies_to("(abs(x)+1)*inf", "∞");
  simplifies_to("cos(x)+inf", "∞");
  simplifies_to("1/inf", "0");
  simplifies_to("0/inf", "0");
  simplifies_to("inf×i×i×i×i", "∞");
  simplifies_to("inf×2i", "∞×i", cartesianCtx);
  simplifies_to("-i×inf", "-∞×i", cartesianCtx);
  simplifies_to("inf×cos(3)×i", "∞×sign(cos(3)×i)", cartesianCtx);

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("x→f(x)", symbolStore);
  Poincare::ProjectionContext ctx = {
      .m_symbolic = Poincare::SymbolicComputation::KeepAllSymbols,
      .m_context = symbolStore,
  };
  simplifies_to("f(x)-inf", "f(x)-∞", ctx);

  // x^inf
  // TODO simplifies_to("(-2)^inf", "undef");  // complex inf
  // TODO simplifies_to("(-2)^(-inf)", "0");
  simplifies_to("(-1)^inf", "undef");
  simplifies_to("(-1)^(-inf)", "undef");
  // TODO simplifies_to("(-0.3)^inf", "0");
  // TODO simplifies_to("(-0.3)^(-inf)", "undef");  // complex inf
  simplifies_to("0^inf", "0");
  simplifies_to("0^(-inf)", "undef");  // complex inf
  simplifies_to("0.3^inf", "0");
  simplifies_to("0.3^(-inf)", "∞");
  simplifies_to("1^inf", "undef");
  simplifies_to("1^(-inf)", "undef");
  simplifies_to("2^inf", "∞");
  simplifies_to("2^(-inf)", "0");
  simplifies_to("x^inf", "e^(∞×sign(ln(x)))");

  // inf^x
  simplifies_to("inf^i", "undef");
  simplifies_to("inf^6", "∞");
  simplifies_to("(-inf)^6", "∞");
  simplifies_to("inf^7", "∞");
  simplifies_to("(-inf)^7", "-∞");
  simplifies_to("inf^-6", "0");
  simplifies_to("(-inf)^-6", "0");
  simplifies_to("inf^0", "undef");
  simplifies_to("(-inf)^0", "undef");
  simplifies_to("inf^inf", "∞");
  simplifies_to("inf^(-inf)", "0");
  simplifies_to("(-inf)^inf", "undef");  // complex inf
  simplifies_to("(-inf)^(-inf)", "0");
  simplifies_to("inf^x", "e^(∞×sign(x))");

  // functions
  simplifies_to("exp(inf)", "∞");
  simplifies_to("exp(-inf)", "0");
  simplifies_to("log(inf,0)", "undef");
  simplifies_to("log(-inf,0)", "undef");
  simplifies_to("log(inf,1)", "undef");
  simplifies_to("log(-inf,1)", "undef");
  simplifies_to("log(inf,x)", "dep(∞×sign(1/ln(x)),{nonNull(x),realPos(x)})");
  simplifies_to("log(-inf,x)",
                "dep(nonreal,{nonNull(x),realPos(x),ln(-∞)/ln(x)})");
  simplifies_to("log(-inf,x)", "dep((∞+π×i)/ln(x),{nonNull(x)})", cartesianCtx);
  /* Should be nonreal, TODO return NonReal when evaluating PowReal(x) with x
   * non real */
  simplifies_to("log(inf,-3)", "undef");
  simplifies_to("log(inf,-3)", "∞×sign(1/ln(-3))", cartesianCtx);
  simplifies_to("log(0,inf)", "undef");
  simplifies_to("log(0,-inf)", "undef", cartesianCtx);
  simplifies_to("log(1,inf)", "0");
  simplifies_to("log(1,-inf)", "0", cartesianCtx);
  simplifies_to("log(x,inf)", "dep(0,{0×ln(x),nonNull(x),realPos(x)})");
  simplifies_to("log(x,-inf)", "dep(log(x,-∞),{nonNull(x)})", cartesianCtx);
  simplifies_to("log(inf,inf)", "undef");
  // TODO_PCJ simplifies_to("log(-inf,inf)", "undef", cartesianCtx);
  // TODO_PCJ simplifies_to("log(inf,-inf)", "undef", cartesianCtx);
  // TODO_PCJ simplifies_to("log(-inf,-inf)", "undef", cartesianCtx);
  simplifies_to("ln(inf)", "∞");
  simplifies_to("ln(-inf)", "nonreal");
  simplifies_to("cos(inf)", "undef");
  simplifies_to("cos(-inf)", "undef");
  simplifies_to("sin(inf)", "undef");
  simplifies_to("sin(-inf)", "undef");
  simplifies_to("atan(inf)", "π/2");
  simplifies_to("atan(-inf)", "-π/2");
  simplifies_to("atan(e^inf)", "π/2");
}

QUIZ_CASE(pcj_mixed_fraction) {
  simplifies_to("1 2/3", "5/3");
  simplifies_to("-1 2/3", "-5/3");
}
